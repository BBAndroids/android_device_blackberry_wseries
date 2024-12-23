#ifndef __BBRY_VS__
#define __BBRY_VS__

#include <stdint.h>

int bbry_log_message(int prio, const char *tag, const char *fmt, ...);

void wait_for_modem();

int radionv_qct_readnv_item(uint16_t item, uint8_t *nv_data, uint32_t *nv_data_len, int *status);
int radionv_qct_writenv_item(uint16_t item, uint8_t *nv_data, uint32_t nv_data_len, int *status);

int radionv_qct_read_efs_large_nv_item(const char *path, uint8_t *nv_data, uint32_t *v_requested_resp_len, int *status);
// radionv_qct_write_efs_large_nv_item

// radionv_qct_efs_delete

int radionv_qct_read_efs_e_nv_item(const char *path, uint8_t *nv_data, uint32_t *v_requested_resp_len, int *status);
// radionv_qct_write_efs_e_nv_item

int radionv_qct_read_efs_nv_item(const char *path, uint8_t *nv_data, uint32_t v_requested_resp_len, int *status);
// radionv_qct_write_efs_nv_item

void bbry_efs_force_sync_util();

void bbry_reset_modem_util();

#endif
