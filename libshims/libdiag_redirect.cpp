/*
 * Copyright (C) 2024 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <log/log.h>

#define LOG_TAG 

typedef struct {
	uint16_t line;			/*!< Line number in source file */
	uint16_t ss_id;			/*!< Subsystem ID               */
	uint32_t ss_mask;			/*!< Subsystem Mask             */
} msg_desc_type;

typedef struct
{
	msg_desc_type desc;       /* ss_mask, line, ss_id */
	const char *fmt;      /* Printf style format string */
	const char *fname;        /* Pointer to source file name */
}
msg_const_type;

extern "C" void msg_sprintf(msg_const_type *const_blk, ...) {
	char buffer[4096];

	const char *tag = strrchr(const_blk->fname, '/');
	if (tag)
		tag += 1;
	else
		tag = const_blk->fname;

	va_list args;
	va_start(args, const_blk);
	vsnprintf(buffer, sizeof(buffer), const_blk->fmt, args);
	va_end(args);
	
	__android_log_buf_print(LOG_ID_RADIO, ANDROID_LOG_INFO, "qcdiag", "%s:%d %s", tag, const_blk->desc.line, buffer);
}

