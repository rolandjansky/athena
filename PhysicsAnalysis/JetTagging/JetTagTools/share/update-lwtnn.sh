#!/usr/bin/env bash

# destination package settings
INC_DIR=JetTagTools

# do the stuff!
cd ../
BASE=$(pwd)
INCLUDE=$BASE/$INC_DIR
SRC=$BASE/src
DIR=$(mktemp -d)
cd $DIR

UPSTREAM=https://github.com/lwtnn/lwtnn.git
TAG=v2.0
# TAG=master

# this won't work with git version < 1.7.10
# git clone -b $TAG --depth 1 $UPSTREAM lwtnn

git clone $UPSTREAM lwtnn
cd lwtnn
git checkout $TAG
REV=$(git describe)

# only copy some files
HEADERS=(
    parse_json.hh LightweightNeuralNetwork.hh Stack.hh
    NNLayerConfig.hh Exceptions.hh)
SRC_FILES=(
    LightweightNeuralNetwork.cxx parse_json.cxx Stack.cxx)


# put a banner on every copied file to warn people not to edit it
BANNER="// WARNING: this code was copied automatically from\n"
BANNER+="// $UPSTREAM (rev $REV)\n"
BANNER+="// Please don't edit it! To get the latest version, run\n"
BANNER+="// > ./${0##*/}\n"
BANNER+="// from ${INC_DIR}/share\n"

# copy over headers (change extension from .hh to .h)
SEDSTR='s:^#include "(.*)\.hh":#include "\1\.h":'
for HEADER in ${HEADERS[@]}; do
    NEWHEADER=$INCLUDE/${HEADER%.hh}.h
    echo -e $BANNER >| $NEWHEADER
    sed -r "$SEDSTR" include/lwtnn/$HEADER >> $NEWHEADER
done

# copy over src files (change headers to match)
SEDSTR='s:^#include "lwtnn/(.*)\.hh":#include "JetTagTools/\1\.h":'
for SRC_FILE in ${SRC_FILES[@]}; do
    NEWSRC=$SRC/$SRC_FILE
    echo -e $BANNER >| $NEWSRC
    sed -r "$SEDSTR" src/$SRC_FILE >> $NEWSRC
done
