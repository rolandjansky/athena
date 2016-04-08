#!/bin/sh

echo "------------------------------------------------------------------"
echo "[NICOS] Setting test environment"
echo "------------------------------------------------------------------"

redflag=0
if [ "$ATLAS_BASE_RELEASE" = "" ]; then
echo "ATLAS_BASE_RELEASE is not defined"
redflag=1
fi

if [ "$redflag" = "0" ]; then
echo ${ATLAS_BASE_RELEASE} | grep 'atlrel_[0-9]$' > /dev/null 2>&1; statn=$?
DIR_FOR_REQ=/tmp/tmpdir_req1
if [ "$NICOS_SUFFIX" != "" ]; then
DIR_FOR_REQ=/tmp/tmpdir_req1_${NICOS_SUFFIX}
fi
if [ "${NICOS_PROJECT_NAME}" = "WORK_RELEASE" ]; then
DIR_FOR_REQ=/tmp/atn_tmpdir_req_$$
fi
if [ ! -d "$DIR_FOR_REQ" ]; then
mkdir -p $DIR_FOR_REQ
fi

project_basedir=`basename $NICOS_PROJECT_HOME`

prevdir=`pwd`
cd $DIR_FOR_REQ

if [ "${NICOS_PROJECT_NAME}" = "WORK_RELEASE" ]; then

cat > requirements <<EOF
set CMTSITE CERN
macro ATLAS_DIST_AREA "/afs/cern.ch/atlas/software/dist"
macro ATLAS_RELEASE ${ATLAS_BASE_RELEASE}
use AtlasSettings v* /afs/cern.ch/atlas/software/dist
EOF

else

if [ "$statn" -ne 0 ]; then

cat > requirements <<EOF
set CMTSITE CERN
macro ATLAS_DIST_AREA "$NICOS_PROJECT_HOME"
macro ATLAS_RELEASE ${ATLAS_BASE_RELEASE}
use AtlasSettings v* /afs/cern.ch/atlas/software/dist
EOF

else

cat > requirements <<EOF
set CMTSITE CERN
macro ATLAS_DIST_AREA "$NICOS_PROJECT_HOME/.."
macro ATLAS_RELEASE nightlies/${project_basedir}/${ATLAS_BASE_RELEASE}
use AtlasSettings v* /afs/cern.ch/atlas/software/dist
EOF

fi
fi # if [ "${NICOS_PROJECT_NAME}" = "WORK_RELEASE" ]; then

statc=1
if [ "$CMTCONFIG" != "" ]; then
echo $CMTCONFIG | grep dbg > /dev/null; statc=$?
fi 
if [ "$statc" -eq 0 ]; then
source /afs/cern.ch/atlas/scripts/setup_cmt_cern.sh -a ${ATLAS_BASE_RELEASE} dbg
else
source /afs/cern.ch/atlas/scripts/setup_cmt_cern.sh -a ${ATLAS_BASE_RELEASE} opt 
fi

if [ "${NICOS_PROJECT_NAME}" = "WORK_RELEASE" ]; then
export NIGHTLYAREA=${NICOS_PROJECT_HOME}
export CMTPATH=${NICOS_PROJECT_HOME}:/afs/cern.ch/atlas/software/dist/${ATLAS_BASE_RELEASE}
else
if [ "$statn" -ne 0 ]; then
export NIGHTLYAREA=${NICOS_PROJECT_HOME}/${ATLAS_BASE_RELEASE}
export CMTPATH=${NICOS_PROJECT_HOME}/${ATLAS_BASE_RELEASE}
else
atlas_base=`basename ${ATLAS_BASE_RELEASE}`
export NIGHTLYAREA=${NICOS_PROJECT_HOME}/${atlas_base}
export CMTPATH=${NICOS_PROJECT_HOME}/${atlas_base}
fi
fi # if [ "${NICOS_PROJECT_NAME}" = "WORK_RELEASE" ]; th

if [ "${NICOS_PROJECT_NAME}" = "WORK_RELEASE" ]; then
cd ${NICOS_PROJECT_HOME}/$ATN_TEST_PACKAGE/*/cmt
source setup.sh
cd ${NICOS_PROJECT_HOME}/$ATN_TEST_PACKAGE
else
cd $CMTPATH/AtlasRelease/*/cmt
source setup.sh
cd $CMTPATH
fi

rm -rf DIR_FOR_REQ

pyth_path=`$NICOS_HOME/nicos_path_cleaner.pl $PYTHONPATH PyXML`
export PYTHONPATH="$pyth_path"

if [ "${NICOS_PROJECT_NAME}" = "WORK_RELEASE" ]; then
cd ${NICOS_PROJECT_HOME}
else
if [ "$statn" -ne 0 ]; then
cd ${NICOS_PROJECT_HOME}/${ATLAS_BASE_RELEASE}
else
atlas_base=`basename ${ATLAS_BASE_RELEASE}`
cd ${NICOS_PROJECT_HOME}/${atlas_base}
fi
fi

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

#cd $prevdir

fi # if [ "$redflag" = "0" ]






