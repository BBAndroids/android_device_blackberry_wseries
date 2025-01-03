#!/vendor/bin/sh

################################################################################
# helper functions to allow Android init like script

function write() {
    echo -n $2 > $1
}

################################################################################



# Configure the hardware properly
write /sys/module/lpm_levels/enable_low_power/l2 4
write /sys/module/msm_pm/modes/cpu0/power_collapse/suspend_enabled 1
write /sys/module/msm_pm/modes/cpu1/power_collapse/suspend_enabled 1
write /sys/module/msm_pm/modes/cpu2/power_collapse/suspend_enabled 1
write /sys/module/msm_pm/modes/cpu3/power_collapse/suspend_enabled 1
write /sys/module/msm_pm/modes/cpu0/power_collapse/idle_enabled 1
write /sys/module/msm_pm/modes/cpu1/power_collapse/idle_enabled 1
write /sys/module/msm_pm/modes/cpu2/power_collapse/idle_enabled 1
write /sys/module/msm_pm/modes/cpu3/power_collapse/idle_enabled 1
write /sys/module/msm_pm/modes/cpu0/standalone_power_collapse/suspend_enabled 1
write /sys/module/msm_pm/modes/cpu1/standalone_power_collapse/suspend_enabled 1
write /sys/module/msm_pm/modes/cpu2/standalone_power_collapse/suspend_enabled 1
write /sys/module/msm_pm/modes/cpu3/standalone_power_collapse/suspend_enabled 1
write /sys/module/msm_pm/modes/cpu0/standalone_power_collapse/idle_enabled 1
write /sys/module/msm_pm/modes/cpu1/standalone_power_collapse/idle_enabled 1
write /sys/module/msm_pm/modes/cpu2/standalone_power_collapse/idle_enabled 1
write /sys/module/msm_pm/modes/cpu3/standalone_power_collapse/idle_enabled 1
write /sys/module/msm_pm/modes/cpu0/retention/idle_enabled 1
write /sys/module/msm_pm/modes/cpu1/retention/idle_enabled 1
write /sys/module/msm_pm/modes/cpu2/retention/idle_enabled 1
write /sys/module/msm_pm/modes/cpu3/retention/idle_enabled 1

# Disable thermal hotplug to switch governor
write /sys/module/msm_thermal/core_control/enabled 0

write /sys/devices/system/cpu/cpu1/online 1
write /sys/devices/system/cpu/cpu2/online 1
write /sys/devices/system/cpu/cpu3/online 1

write /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor interactive
write /sys/devices/system/cpu/cpu1/cpufreq/scaling_governor interactive
write /sys/devices/system/cpu/cpu2/cpufreq/scaling_governor interactive
write /sys/devices/system/cpu/cpu3/cpufreq/scaling_governor interactive
write /sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay "19000 1400000:39000 1700000:19000"
write /sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load 99
write /sys/devices/system/cpu/cpufreq/interactive/hispeed_freq 1190400
write /sys/devices/system/cpu/cpufreq/interactive/io_is_busy 1
write /sys/devices/system/cpu/cpufreq/interactive/target_loads "85 1500000:90 1800000:70"
write /sys/devices/system/cpu/cpufreq/interactive/min_sample_time 40000
write /sys/devices/system/cpu/cpufreq/interactive/timer_rate 30000
write /sys/devices/system/cpu/cpufreq/interactive/sampling_down_factor 100000
write /sys/devices/system/cpu/cpufreq/interactive/timer_slack 30000
write /sys/devices/system/cpu/cpufreq/interactive/up_threshold_any_cpu_load 50
write /sys/devices/system/cpu/cpufreq/interactive/sync_freq 1036800
write /sys/devices/system/cpu/cpufreq/interactive/up_threshold_any_cpu_freq 1190400

write /sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq 300000
write /sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq 300000
write /sys/devices/system/cpu/cpu2/cpufreq/scaling_min_freq 300000
write /sys/devices/system/cpu/cpu3/cpufreq/scaling_min_freq 300000
chown root system /sys/devices/system/cpu/cpu1/online
chown root system /sys/devices/system/cpu/cpu2/online
chown root system /sys/devices/system/cpu/cpu3/online
chmod 664 /sys/devices/system/cpu/cpu1/online
chmod 664 /sys/devices/system/cpu/cpu2/online
chmod 664 /sys/devices/system/cpu/cpu3/online

# Re-enable thermal hotplug
write /sys/module/msm_thermal/core_control/enabled 1

write /sys/class/devfreq/qcom,cpubw.42/governor "cpubw_hwmon"
write /sys/class/kgsl/kgsl-3d0/devfreq/governor "msm-adreno-tz"

# Input boost config
write /sys/module/cpu_boost/parameters/boost_ms 20
write /sys/module/cpu_boost/parameters/sync_threshold 1728000
write /sys/module/cpu_boost/parameters/input_boost_freq 1497600
write /sys/module/cpu_boost/parameters/input_boost_ms 40

write /dev/cpuctl/cpu.notify_on_migrate 1

setprop vendor.post_boot.parsed 1
