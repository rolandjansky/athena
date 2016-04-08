#!/bin/sh

#############################################################
#                 Run tests against kit                     #
#############################################################

# set path to kit installation
ATLAS_ROOT=$HOME/scratch0/sw/9.0.2-0
# set release number
release=9.0.2

here=`dirname $0`
cd ${here}

#export JOBTRANSFORMSROOT=${here}/../../../JobTransforms/JobTransforms-*
export JOBTRANSFORMSROOT=${here}/../../JobTransforms

../share/KitValidation \
  -p ${ATLAS_ROOT} -b dist -r ${release} --bw -kn -t $HOME/scratch0/tmp

#  -p ${ATLAS_ROOT} -b ${releasebase} -r ${release} --bw -kn -t $HOME/scratch0/tmp

