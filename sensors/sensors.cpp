#define LOG_TAG "sensors_wrapper"
#define LOG_NDEBUG 0

#include <log/log.h>
#include <hardware/sensors.h>
#include <string>
#include <dlfcn.h>
#include <errno.h>

static void *lib_handle = NULL;
static struct sensors_module_t *lib_sensors_module;

struct sensor_t sensor_list[20];

int wrapper_get_sensors_list(struct sensors_module_t* module, struct sensor_t const** list)
{
	const struct sensor_t *lib_list;
	int lib_count = lib_sensors_module->get_sensors_list(lib_sensors_module, &lib_list);
	if (lib_count < 0)
		return lib_count;
	
	if (lib_count > sizeof(sensor_list) / sizeof(sensor_list[0]))
	{
		ALOGE("%s: vendor sensors reported too many sensors! (%d)", __func__, lib_count);
		return -EINVAL;
	}
	
	int count = 0;
	for (int i = 0; i < lib_count; i++)
	{
		const struct sensor_t *s = &lib_list[i];
		ALOGI("%s: N: %s V: %s H: %d T: %d S: %s P: %s",
			__func__, s->name, s->vendor, s->handle,
			s->type, s->stringType, s->requiredPermission);
		
		memcpy(&sensor_list[count], s, sizeof(struct sensor_t));
		
		if (sensor_list[count].type == SENSOR_TYPE_PROXIMITY)
			sensor_list[count].flags = SENSOR_FLAG_WAKE_UP | SENSOR_FLAG_ON_CHANGE_MODE;

		count++;
	}
	
	*list = sensor_list;
	
	return count;
}

int wrapper_set_operation_mode(unsigned int mode)
{
	return lib_sensors_module->set_operation_mode(mode);
}


typedef struct sensors_poll_device_wrapper {
    union {
        /* sensors_poll_device_wrapper is compatible with sensors_poll_device_1,
         * and can be down-cast to it
         */
        struct sensors_poll_device_1 v1;

        struct {
            struct hw_device_t common;
            int (*activate)(struct sensors_poll_device_wrapper *dev, int sensor_handle, int enabled); 
            int (*setDelay)(struct sensors_poll_device_wrapper *dev, int sensor_handle, int64_t sampling_period_ns);
            int (*poll)(struct sensors_poll_device_wrapper *dev, sensors_event_t* data, int count);
	    int (*batch)(struct sensors_poll_device_wrapper *dev, int sensor_handle, int flags, int64_t sampling_period_ns, int64_t max_report_latency_ns);
	    int (*flush)(struct sensors_poll_device_wrapper *dev, int sensor_handle);
	    int (*inject_sensor_data)(struct sensors_poll_device_wrapper *dev, const sensors_event_t *data);
	    int (*register_direct_channel)(struct sensors_poll_device_wrapper *dev, const struct sensors_direct_mem_t* mem, int channel_handle);
	    int (*config_direct_report)(struct sensors_poll_device_wrapper *dev, int sensor_handle, int channel_handle, const struct sensors_direct_cfg_t *config);
	    void (*reserved_procs[5])(void);
        };
    };
    struct sensors_poll_device_t *wrapped_device;
} sensors_poll_device_wrapper_t;

int wrapper_close(struct hw_device_t* device)
{
	struct sensors_poll_device_wrapper *dev = (struct sensors_poll_device_wrapper *) device;
	
	int ret = dev->wrapped_device->common.close(&dev->wrapped_device->common);
	if (ret < 0)
		return ret;

	free(dev);

	return 0;
}

int wrapper_activate(struct sensors_poll_device_wrapper *dev, int sensor_handle, int enabled)
{
	return dev->wrapped_device->activate(dev->wrapped_device, sensor_handle, enabled);
}
 
int wrapper_set_delay(struct sensors_poll_device_wrapper *dev, int sensor_handle, int64_t sampling_period_ns)
{
	return dev->wrapped_device->setDelay(dev->wrapped_device, sensor_handle, sampling_period_ns);
}

int wrapper_poll(struct sensors_poll_device_wrapper *dev, sensors_event_t* data, int count)
{
	return dev->wrapped_device->poll(dev->wrapped_device, data, count);
}

int wrapper_batch(struct sensors_poll_device_wrapper *dev, int sensor_handle, int flags, int64_t sampling_period_ns, int64_t max_report_latency_ns)
{
	(void) flags;
	(void) max_report_latency_ns;
	return dev->wrapped_device->setDelay(dev->wrapped_device, sensor_handle, sampling_period_ns);
}

int wrapper_flush(struct sensors_poll_device_wrapper *dev, int sensor_handle)
{
	(void) dev;
	(void) sensor_handle;
	return -EINVAL;
}


int wrapper_open(const struct hw_module_t* module, const char* id, struct hw_device_t** device)
{
	struct sensors_poll_device_wrapper *dev;

	if (strcmp(id, SENSORS_HARDWARE_POLL) != 0)
		return 0;
	
	dev = (struct sensors_poll_device_wrapper *) malloc(sizeof(*dev));
	if (!dev)
		return -ENOMEM;

 	memset(dev, 0, sizeof(*dev));

	if (lib_handle == NULL)
	{
		lib_handle = dlopen("/vendor/lib/hw/sensors.default.so", RTLD_LAZY);
	 	if (lib_handle == NULL)
	 	{
			ALOGW("dlerror(): %s", dlerror());
			return -EINVAL;
	 	}

	 	lib_sensors_module = (sensors_module_t *) dlsym(lib_handle, HAL_MODULE_INFO_SYM_AS_STR);
	 	if (!lib_sensors_module)
	 	{
			ALOGW("no lib_sensors_module");
			return -EINVAL;
	 	}

	 	void (* extended_logging_setlevel)(int loglevel);
	 	extended_logging_setlevel = (void (*)(int)) dlsym(lib_handle, "extended_logging_setlevel");
	 	if (extended_logging_setlevel)
	 		extended_logging_setlevel(5);

	 	void (* extended_logging_init)(const char *logfile);
	 	extended_logging_init = (void (*)(const char *)) dlsym(lib_handle, "extended_logging_init");
	 	if (extended_logging_init)
	 		extended_logging_init("/dev/null");
	}

	dev->common.tag = HARDWARE_DEVICE_TAG;
	dev->common.version = SENSORS_DEVICE_API_VERSION_1_3;
	dev->common.module = (struct hw_module_t *) module;
	dev->common.close = wrapper_close;
	dev->activate = wrapper_activate;
	dev->setDelay = wrapper_set_delay;
	dev->poll = wrapper_poll;
	dev->batch = wrapper_batch;
	dev->flush = wrapper_flush;

 	int ret = lib_sensors_module->common.methods->open(&lib_sensors_module->common, id, (struct hw_device_t **) &dev->wrapped_device);
 	if (ret < 0)
 	{
 		dlclose(lib_handle);
 		lib_handle = NULL;
 		free(dev);
 	}
 	else
 		*device = (struct hw_device_t *) dev;
 	
 	return ret;
}

static struct hw_module_methods_t wrapper_module_methods = {
    .open = wrapper_open
};

struct sensors_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = SENSORS_MODULE_API_VERSION_0_1,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = SENSORS_HARDWARE_MODULE_ID,
        .name = "BlackBerry Sensors Wrapper Module",
        .author = "balika011",
        .methods = &wrapper_module_methods,
        .dso = NULL,
        .reserved = {0},
    },
    .get_sensors_list = wrapper_get_sensors_list,
    .set_operation_mode = wrapper_set_operation_mode
};
