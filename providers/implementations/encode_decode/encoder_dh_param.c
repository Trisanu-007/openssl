/*
 * Copyright 2019-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/*
 * DH low level APIs are deprecated for public use, but still ok for
 * internal use.
 */
#include "internal/deprecated.h"

#include <openssl/core_dispatch.h>
#include <openssl/pem.h>
#include <openssl/dh.h>
#include <openssl/types.h>
#include <openssl/params.h>
#include "prov/bio.h"
#include "prov/implementations.h"
#include "prov/providercommonerr.h"
#include "prov/provider_ctx.h"
#include "encoder_local.h"

static OSSL_FUNC_encoder_newctx_fn dh_param_newctx;
static OSSL_FUNC_encoder_freectx_fn dh_param_freectx;
static OSSL_FUNC_encoder_encode_data_fn dh_param_der_data;
static OSSL_FUNC_encoder_encode_object_fn dh_param_der;
static OSSL_FUNC_encoder_encode_data_fn dh_param_pem_data;
static OSSL_FUNC_encoder_encode_object_fn dh_param_pem;

static OSSL_FUNC_encoder_encode_data_fn dh_param_print_data;
static OSSL_FUNC_encoder_encode_object_fn dh_param_print;

/* Parameters : context */

/*
 * There's no specific implementation context, so we use the provider context
 */
static void *dh_param_newctx(void *provctx)
{
    return provctx;
}

static void dh_param_freectx(void *ctx)
{
}

/* Public key : DER */
static int dh_param_der_data(void *ctx, const OSSL_PARAM params[],
                             OSSL_CORE_BIO *out,
                             OSSL_PASSPHRASE_CALLBACK *cb, void *cbarg)
{
    OSSL_FUNC_keymgmt_new_fn *dh_new = ossl_prov_get_keymgmt_dh_new();
    OSSL_FUNC_keymgmt_free_fn *dh_free = ossl_prov_get_keymgmt_dh_free();
    OSSL_FUNC_keymgmt_import_fn *dh_import = ossl_prov_get_keymgmt_dh_import();
    int ok = 0;

    if (dh_import != NULL) {
        DH *dh;

        /* ctx == provctx */
        if ((dh = dh_new(ctx)) != NULL
            && dh_import(dh, OSSL_KEYMGMT_SELECT_DOMAIN_PARAMETERS, params)
            && dh_param_der(ctx, dh, out, cb, cbarg))
            ok = 1;
        dh_free(dh);
    }
    return ok;
}

static int dh_param_der(void *ctx, void *dh, OSSL_CORE_BIO *cout,
                        OSSL_PASSPHRASE_CALLBACK *cb, void *cbarg)
{
    BIO *out = bio_new_from_core_bio(ctx, cout);
    int ret;

    if (out == NULL)
        return 0;
    ret = i2d_DHparams_bio(out, dh);
    BIO_free(out);

    return ret;
}

/* Public key : PEM */
static int dh_param_pem_data(void *ctx, const OSSL_PARAM params[],
                             OSSL_CORE_BIO *out,
                             OSSL_PASSPHRASE_CALLBACK *cb, void *cbarg)
{
    OSSL_FUNC_keymgmt_new_fn *dh_new = ossl_prov_get_keymgmt_dh_new();
    OSSL_FUNC_keymgmt_free_fn *dh_free = ossl_prov_get_keymgmt_dh_free();
    OSSL_FUNC_keymgmt_import_fn *dh_import = ossl_prov_get_keymgmt_dh_import();
    int ok = 0;

    if (dh_import != NULL) {
        DH *dh;

        /* ctx == provctx */
        if ((dh = dh_new(ctx)) != NULL
            && dh_import(dh, OSSL_KEYMGMT_SELECT_DOMAIN_PARAMETERS, params)
            && dh_param_pem(ctx, dh, out, cb, cbarg))
            ok = 1;
        dh_free(dh);
    }
    return ok;
}

static int dh_param_pem(void *ctx, void *dh, OSSL_CORE_BIO *cout,
                        OSSL_PASSPHRASE_CALLBACK *cb, void *cbarg)
{
    BIO *out = bio_new_from_core_bio(ctx, cout);
    int ret;

    if (out == NULL)
        return 0;

    ret = PEM_write_bio_DHparams(out, dh);
    BIO_free(out);

    return ret;
}

static int dh_param_print_data(void *ctx, const OSSL_PARAM params[],
                               OSSL_CORE_BIO *out,
                               OSSL_PASSPHRASE_CALLBACK *cb, void *cbarg)
{
    OSSL_FUNC_keymgmt_new_fn *dh_new = ossl_prov_get_keymgmt_dh_new();
    OSSL_FUNC_keymgmt_free_fn *dh_free = ossl_prov_get_keymgmt_dh_free();
    OSSL_FUNC_keymgmt_import_fn *dh_import = ossl_prov_get_keymgmt_dh_import();
    int ok = 0;

    if (dh_import != NULL) {
        DH *dh;

        /* ctx == provctx */
        if ((dh = dh_new(ctx)) != NULL
            && dh_import(dh, OSSL_KEYMGMT_SELECT_DOMAIN_PARAMETERS, params)
            && dh_param_print(ctx, dh, out, cb, cbarg))
            ok = 1;
        dh_free(dh);
    }
    return ok;
}

static int dh_param_print(void *ctx, void *dh, OSSL_CORE_BIO *cout,
                          OSSL_PASSPHRASE_CALLBACK *cb, void *cbarg)
{
    BIO *out = bio_new_from_core_bio(ctx, cout);
    int ret;

    if (out == NULL)
        return 0;

    ret = ossl_prov_print_dh(out, dh, dh_print_params);
    BIO_free(out);

    return ret;
}

const OSSL_DISPATCH dh_param_der_encoder_functions[] = {
    { OSSL_FUNC_ENCODER_NEWCTX, (void (*)(void))dh_param_newctx },
    { OSSL_FUNC_ENCODER_FREECTX, (void (*)(void))dh_param_freectx },
    { OSSL_FUNC_ENCODER_ENCODE_DATA, (void (*)(void))dh_param_der_data },
    { OSSL_FUNC_ENCODER_ENCODE_OBJECT, (void (*)(void))dh_param_der },
    { 0, NULL }
};

const OSSL_DISPATCH dh_param_pem_encoder_functions[] = {
    { OSSL_FUNC_ENCODER_NEWCTX, (void (*)(void))dh_param_newctx },
    { OSSL_FUNC_ENCODER_FREECTX, (void (*)(void))dh_param_freectx },
    { OSSL_FUNC_ENCODER_ENCODE_DATA, (void (*)(void))dh_param_pem_data },
    { OSSL_FUNC_ENCODER_ENCODE_OBJECT, (void (*)(void))dh_param_pem },
    { 0, NULL }
};

const OSSL_DISPATCH dh_param_text_encoder_functions[] = {
    { OSSL_FUNC_ENCODER_NEWCTX, (void (*)(void))dh_param_newctx },
    { OSSL_FUNC_ENCODER_FREECTX, (void (*)(void))dh_param_freectx },
    { OSSL_FUNC_ENCODER_ENCODE_OBJECT, (void (*)(void))dh_param_print },
    { OSSL_FUNC_ENCODER_ENCODE_DATA,
      (void (*)(void))dh_param_print_data },
    { 0, NULL }
};
