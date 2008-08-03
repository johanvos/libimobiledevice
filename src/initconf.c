/*
 * userpref.c
 * contains methods to access user specific certificates IDs and more.
 *
 * Copyright (c) 2008 Jonathan Beck All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA 
 */

#include <gnutls/gnutls.h>
#include <glib.h>
#include "userpref.h"

int debug = 1;

int main(int argc, char *argv[]) {

	gnutls_global_init();

	char* host_id = NULL;
	gnutls_x509_privkey_t root_privkey;
	gnutls_x509_privkey_t host_privkey;

	gnutls_x509_crt_t root_cert;
	gnutls_x509_crt_t host_cert;

	gnutls_x509_privkey_init(&root_privkey);
	gnutls_x509_privkey_init(&host_privkey);

	gnutls_x509_crt_init(&root_cert);
	gnutls_x509_crt_init(&host_cert);

	/* generate keys */
	int ret1 = gnutls_x509_privkey_generate(root_privkey, GNUTLS_PK_RSA, 2048, 0);
	int ret2 = gnutls_x509_privkey_generate(host_privkey, GNUTLS_PK_RSA, 2048, 0);

	/* generate certificates */
	gnutls_x509_crt_set_key(root_cert, root_privkey);
	gnutls_x509_crt_set_serial(root_cert, "\x00", 1);
	gnutls_x509_crt_set_version(root_cert, 3);
	gnutls_x509_crt_set_ca_status(root_cert, 1);
	gnutls_x509_crt_set_activation_time(root_cert, time(NULL));
	gnutls_x509_crt_set_expiration_time(root_cert, time(NULL) + (60 * 60 * 24 * 365 * 10));
	gnutls_x509_crt_sign(root_cert, root_cert, root_privkey);


	gnutls_x509_crt_set_key(host_cert, host_privkey);
	gnutls_x509_crt_set_serial(host_cert, "\x00", 1);
	gnutls_x509_crt_set_version(host_cert, 3);
	gnutls_x509_crt_set_ca_status(host_cert, 1);
	gnutls_x509_crt_set_activation_time(host_cert, time(NULL));
	gnutls_x509_crt_set_expiration_time(host_cert, time(NULL) + (60 * 60 * 24 * 365 * 10));
	gnutls_x509_crt_sign(host_cert, root_cert, root_privkey);


	/* export to PEM format */
	gnutls_datum_t root_key_pem = {NULL, 0};
	gnutls_datum_t host_key_pem = {NULL, 0};

	gnutls_x509_privkey_export (root_privkey, GNUTLS_X509_FMT_PEM,  NULL, &root_key_pem.size);
	gnutls_x509_privkey_export (host_privkey, GNUTLS_X509_FMT_PEM,  NULL, &host_key_pem.size);

	root_key_pem.data = gnutls_malloc(root_key_pem.size);
	root_key_pem.data = gnutls_malloc(root_key_pem.size);

	gnutls_x509_privkey_export (root_privkey, GNUTLS_X509_FMT_PEM,  root_key_pem.data, &root_key_pem.size);
	gnutls_x509_privkey_export (host_privkey, GNUTLS_X509_FMT_PEM,  host_key_pem.data, &host_key_pem.size);

	gnutls_datum_t root_cert_pem = {NULL, 0};
	gnutls_datum_t host_cert_pem = {NULL, 0};

	gnutls_x509_crt_export (root_cert, GNUTLS_X509_FMT_PEM,  NULL, &root_cert_pem.size);
	gnutls_x509_crt_export (host_cert, GNUTLS_X509_FMT_PEM,  NULL, &host_cert_pem.size);

	root_cert_pem.data = gnutls_malloc(root_cert_pem.size);
	root_cert_pem.data = gnutls_malloc(root_cert_pem.size);

	gnutls_x509_crt_export (root_cert, GNUTLS_X509_FMT_PEM,  root_cert_pem.data, &root_cert_pem.size);
	gnutls_x509_crt_export (host_cert, GNUTLS_X509_FMT_PEM,  host_cert_pem.data, &host_cert_pem.size);

	/* encode in base64 for storage */
	char* root_key_b64 = g_base64_encode (root_key_pem.data,root_key_pem.size);
	char* host_key_b64 = g_base64_encode (host_key_pem.data,host_key_pem.size);

	char* root_cert_b64 = g_base64_encode (root_cert_pem.data,root_cert_pem.size);
	char* host_cert_b64 = g_base64_encode (host_cert_pem.data,host_cert_pem.size);

	/* store values in config file */
	
	init_config_file(host_id, root_key_b64, host_key_b64, root_cert_b64, host_cert_b64);

	gnutls_free(root_key_pem.data);
	gnutls_free(host_key_pem.data);
	gnutls_free(root_cert_pem.data);
	gnutls_free(host_cert_pem.data);
	g_free(root_key_b64);
	g_free(host_key_b64);
	g_free(root_cert_b64);
	g_free(host_cert_b64);

	return 0;
}
