/*
 * Copyright 2016-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OPENSSL_FIPS_H
# define OPENSSL_FIPS_H
# pragma once

# include <openssl/macros.h>

# ifdef __cplusplus
extern "C" {
# endif

# define FIPS_mode() EVP_default_properties_is_fips_enabled(NULL)

# ifdef __cplusplus
}
# endif
#endif
