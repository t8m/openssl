/*
 * Copyright 2023 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#define OSSL_FORCE_ERR_STATE

#include <openssl/err.h>
#include "err_local.h"

/*
 * Save and restore error state.
 * We are using CRYPTO_zalloc(.., NULL, 0) instead of OPENSSL_malloc() in
 * these functions to prevent mem alloc error loop.
 */

ERR_STATE *OSSL_ERR_STATE_new(void)
{
    return CRYPTO_zalloc(sizeof(ERR_STATE), NULL, 0);
}

void OSSL_ERR_STATE_save(ERR_STATE *es)
{
    size_t i;
    ERR_STATE *thread_es;

    if (es == NULL)
        return;

    for (i = 0; i < ERR_NUM_ERRORS; i++)
        err_clear(es, i, 1);

    thread_es = ossl_err_get_state_int();
    if (thread_es == NULL)
        return;

    memcpy(es, thread_es, sizeof(*es));
    /* Taking over the pointers, just clear the thread state. */
    memset(thread_es, 0, sizeof(*thread_es));
}

void OSSL_ERR_STATE_restore(const ERR_STATE *es)
{
    size_t i;
    ERR_STATE *thread_es;

    if (es == NULL || es->bottom == es->top)
        return;

    thread_es = ossl_err_get_state_int();
    if (thread_es == NULL)
        return;

    for (i = (size_t)es->bottom; i != (size_t)es->top;) {
        size_t top;

        i = (i + 1) % ERR_NUM_ERRORS;
        if ((es->err_flags[i] & ERR_FLAG_CLEAR) != 0)
            continue;

        err_get_slot(thread_es);
        top = thread_es->top;
        err_clear(thread_es, top, 0);

        thread_es->err_flags[top] = es->err_flags[i] & ~ERR_FLAG_MARK;
        thread_es->err_buffer[top] = es->err_buffer[i];

        err_set_debug(thread_es, top, es->err_file[i], es->err_line[i],
                      es->err_func[i]);

        if (es->err_data[i] != NULL && es->err_data_size[i] != 0) {
            void *data;
            size_t data_sz = es->err_data_size[i];

            data = CRYPTO_malloc(data_sz, NULL, 0);
            if (data != NULL) {
                memcpy(data, es->err_data[i], data_sz);
                err_set_data(thread_es, top, data, data_sz,
                             es->err_data_flags[i] | ERR_TXT_MALLOCED);
            }
        } else {
            err_clear_data(thread_es, top, 0);
        }
    }
}
