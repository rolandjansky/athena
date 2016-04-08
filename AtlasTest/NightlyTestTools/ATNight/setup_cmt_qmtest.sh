#!/bin/sh

echo "------------------------------------------------------------------"
echo "[ATN] Setting test environment"
echo "------------------------------------------------------------------"

prevdir=`pwd`

cmtprpath=${CMTPROJECTPATH}
source $NICOS_HOME/atlas/setup_cmt.sh
export CMTPROJECTPATH=$cmtprpath

export NIGHTLYAREA=${NICOS_RELHOME}

cd ${NICOS_RELHOME}
if [ "${CMTSTRUCTURINGSTYLE}" = without_version_directory ]; then
cd Atlas${NICOS_ATLAS_PROJECT}Release/cmt
else
cd AtlasRelease/*/cmt
fi
source setup.sh
PYTHONPATH=`${NICOS_HOME}/atlas/rmPyXML_from_PYTHONPATH.pl`
export PYTHONPATH

#DIR_FOR_REQ=/tmp/tmpdir_req2
#if [ "$NICOS_SUFFIX" != "" ]; then
#DIR_FOR_REQ=/tmp/tmpdir_req2_${NICOS_SUFFIX}
#fi
#if [ ! -d "$DIR_FOR_REQ" ]; then
#mkdir -p $DIR_FOR_REQ
#fi
#
#cd $DIR_FOR_REQ
#cat > requirements <<EOF
#path_remove PYTHONPATH "/PyXML/"
#EOF
#cmt config
#source setup.sh

#rm -rf $DIR_FOR_REQ

# setting qmtest specific env
export PATH=/afs/cern.ch/sw/lcg/external/QMtest/2.0.3/rh73_gcc32/bin:$PATH
 
# the string 0.3.1 is modified automatically by the release script
 
export LCGSPI_TOP=/afs/cern.ch/sw/lcg/app/spi
abc=`ls -l ${LCGSPI_TOP}/tools/latest | sed 's/ /\//g'`
LCGSPI_VERSION=`basename $abc`
export LCGSPI_VERSION
export LCGSPI_RELDIR=${LCGSPI_TOP}/tools/${LCGSPI_VERSION}
 
# path to scram is not versioned -- scram takes care of swicthing to
# the right version automatically
export PATH=${LCGSPI_RELDIR}/scripts:${LCGSPI_TOP}/scram:${PATH}
 
export PYTHONPATH=${LCGSPI_RELDIR}/python:${PYTHONPATH}
 
unset PYTHONHOME

cd $prevdir







