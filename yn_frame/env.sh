#!/bin/bash
export SERVER_ROOT=`pwd`
cd ..
export BASEBASEROOT=`pwd`
cd ..
export BASEBASEBASEROOT=`pwd`

cd ${SERVER_ROOT}

export FRAMECOMMON=${BASEBASEBASEROOT}/framework/framecommon
export SRVFRAME=${BASEBASEBASEROOT}/framework/srvframe

