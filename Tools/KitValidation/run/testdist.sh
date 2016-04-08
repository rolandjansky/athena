#!/bin/sh

#############################################################
#               Run tests against release                   #
#############################################################

ATLAS_ROOT=/afs/cern.ch/atlas/software
# uncomment to use with nightlies
#releasebase=dist/nightlies/rel
# uncomment to use with release
releasebase=dist
# set release number
#release=latest
release=9.0.2

here=`dirname $0`
cd ${here}

#export JOBTRANSFORMSROOT=${here}/../../../JobTransforms/JobTransforms-*
export JOBTRANSFORMSROOT=${here}/../../JobTransforms

../share/KitValidation \
  -p ${ATLAS_ROOT} -b ${releasebase} -r ${release} -x dist --bw -kn -t $HOME/scratch0/tmp

