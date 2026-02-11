#!/bin/bash

SOURCE="docker-bose-build-env-image.tar.part-*"
TARGET="docker-bose-build-env-image.tar"

cat ../../assets/$SOURCE > $TARGET

if tar -tf $TARGET > /dev/null 2>&1; then
    echo "Archive $TARGET OK"
else
    echo "Archive $TARGET corrupted or unreadable"
fi
