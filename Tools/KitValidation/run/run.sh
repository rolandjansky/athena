#!/bin/sh

ATLAS_ROOT=/afs/cern.ch/atlas/software
releasebase=dist/nightlies/rel
release=latest

here=`dirname $0`
cd ${here}

#export JOBTRANSFORMSROOT=${here}/../../../JobTransforms/JobTransforms-00-00-00

#ls $JOBTRANSFORMSROOT/share

../share/KitValidation \
  -p ${ATLAS_ROOT} -b ${releasebase} -r ${release} -x dist --bw -gkn

