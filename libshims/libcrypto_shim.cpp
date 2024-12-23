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

#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>

extern "C" int EVP_EncryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
{
	return EVP_EncryptFinal_ex(ctx, out, outl);
}

extern "C" int EVP_DecryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl)
{
	return EVP_DecryptFinal_ex(ctx, out, outl);
}

extern "C" long SSL_ctrl(SSL *s, int cmd, long larg, void *parg) {
	BIO *bio = BIO_new(BIO_f_ssl());
	BIO_set_data(bio, s);
	
	long ret = BIO_ctrl(bio, cmd, larg, parg);

	BIO_set_data(bio, NULL);
	BIO_free(bio);
	
	return ret;
}

extern "C" void ENGINE_cleanup()
{
}
