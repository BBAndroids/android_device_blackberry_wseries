#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <cutils/properties.h>

int main()
{
	uint8_t mac[6];
	memset(mac, 0xff, sizeof(mac));
	FILE *btf = fopen("/nvram/boardid/macaddressbt", "rb");
	if (!btf)
		return -1;
	fread(mac, 1, sizeof(mac), btf);
	fclose(btf);

	char mac_str[20];
	snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	property_set("persist.service.bdroid.bdaddr", mac_str);

	property_set("ro.hwf.keypadtype", "3 3row");
	
	char kl[128];
	memset(kl, 0, sizeof(kl));
	FILE *klf = fopen("/persist/mfg/keypadlanguage", "rb");
	if (!klf)
		return -2;
	fread(kl, 1, sizeof(kl), klf);
	fclose(klf);
	property_set("ro.hwf.keypadlanguage", kl);
	
	return 0;
}
