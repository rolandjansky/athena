#!/bin/bash
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# This script will return the path to $ATLAS_REFERENCE_TAG.

# First try to find it through DATAPATH:
get_files -data -symlink $ATLAS_REFERENCE_TAG > /dev/null
refdir=`basename $ATLAS_REFERENCE_TAG`
if [ -r ${refdir} ]; then
    echo ${refdir}
    exit 0
fi

# Alternatively use ATLAS_REFERENCE_DATA:
refdir=${ATLAS_REFERENCE_DATA}/${ATLAS_REFERENCE_TAG}
if [ -r "${ATLAS_REFERENCE_DATA}" ]; then
    echo ${refdir}
fi
