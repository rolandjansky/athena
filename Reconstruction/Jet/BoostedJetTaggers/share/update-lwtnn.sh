#!/usr/bin/env bash

# ---- destination package settings ---
# THIS SHOULD BE ALL YOU HAVE TO EDIT
# -------------------------------------
INC_DIR=BoostedJetTaggers
SRC_DIR=Root

# make sure we're not being stupid
# interactive shell check
if [[ $- == *i* ]] ; then
    echo "DONT SOURCE THIS! (it's executable!)" >&2
    return 1
fi
# directory check
if [[ ! -f $(basename $0) ]]; then
    echo "Run this script from the dir where it lives" >&2
    exit 1
fi

cd ../
mkdir -p $INC_DIR/lwtnn
mkdir -p $SRC_DIR

# mark this directory and jump into the
BASE=$(pwd)
INCLUDE=$BASE/$INC_DIR
SRC=$BASE/$SRC_DIR
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

# annoying check for BSD vs GNU
if ! $(sed -r '/s/p' <<< zork &> /dev/null); then
    OPT=-E
else
    OPT=-r
fi

# copy over headers (change extension from .hh to .h)
SEDSTR='s:^#include "(.*)\.hh":#include "\1\.h":'
for HEADER in ${HEADERS[@]}; do
    NEWHEADER=$INCLUDE/lwtnn/${HEADER%.hh}.h
    echo -e $BANNER >| $NEWHEADER
    sed $OPT "$SEDSTR" include/lwtnn/$HEADER >> $NEWHEADER
done

# copy over src files (change headers to match)
SEDSTR="s:^#include \"(.*)\.hh\":#include \"$INC_DIR/\1\.h\":"
for SRC_FILE in ${SRC_FILES[@]}; do
    NEWSRC=$SRC/lwtnn_$SRC_FILE
    echo -e $BANNER >| $NEWSRC
    sed $OPT "$SEDSTR" src/$SRC_FILE >> $NEWSRC
done
