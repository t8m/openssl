#!/bin/sh
#
# Copyright 2017-2021 The OpenSSL Project Authors. All Rights Reserved.
# Copyright (c) 2017, Oracle and/or its affiliates.  All rights reserved.
#
# Licensed under the Apache License 2.0 (the "License").  You may not use
# this file except in compliance with the License.  You can obtain a copy
# in the file LICENSE in the source distribution or at
# https://www.openssl.org/source/license.html

#
# OpenSSL external testing using the Python Cryptography module
#
set -e

SRCTOP="$(cd $SRCTOP; pwd)"
BLDTOP="$(cd $BLDTOP; pwd)"

O_EXE="$BLDTOP/apps"
O_BINC="$BLDTOP/include"
O_SINC="$SRCTOP/include"
O_LIB="$BLDTOP"

export PATH=$O_EXE:$PATH
export LD_LIBRARY_PATH=$O_LIB:$LD_LIBRARY_PATH

# Check/Set openssl version
OPENSSL_VERSION=`openssl version | cut -f 2 -d ' '`

echo "------------------------------------------------------------------"
echo "Testing OpenSSL using Python Cryptography:"
echo "   CWD:                $PWD"
echo "   SRCTOP:             $SRCTOP"
echo "   BLDTOP:             $BLDTOP"
echo "   OpenSSL version:    $OPENSSL_VERSION"
echo "------------------------------------------------------------------"

cd $SRCTOP

# Create a python virtual env and activate
rm -rf venv-pycrypto
python3 -m venv venv-pycrypto
. ./venv-pycrypto/bin/activate

cd pyca-cryptography

git fetch https://github.com/reaperhulk/cryptography.git 300
git checkout FETCH_HEAD

pip3 install setuptools-rust

pip3 install ./vectors/

pip3 install .[test]

echo "------------------------------------------------------------------"
echo "Building cryptography"
echo "------------------------------------------------------------------"
python3 ./setup.py clean

CFLAGS="-I$O_BINC -I$O_SINC -L$O_LIB" python3 ./setup.py build

echo "------------------------------------------------------------------"
echo "Running tests"
echo "------------------------------------------------------------------"

CFLAGS="-I$O_BINC -I$O_SINC -L$O_LIB" python3 ./setup.py test

cd ../
deactivate
rm -rf venv-pycrypto

exit 0
