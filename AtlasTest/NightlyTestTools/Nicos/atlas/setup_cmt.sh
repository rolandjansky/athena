#!/bin/sh

echo "------------------------------------------------------------------"
echo "[NICOS] Setting initial environment"
echo "------------------------------------------------------------------"

DIR_FOR_REQ=/tmp/tmpdir_req1
if [ "$NICOS_SUFFIX" != "" ]; then
DIR_FOR_REQ=/tmp/tmpdir_req1_${NICOS_SUFFIX}
fi
if [ ! -d "$DIR_FOR_REQ" ]; then
mkdir -p $DIR_FOR_REQ
fi

dn1=`dirname $NICOS_COPY_HOME`
dn2=`dirname $dn1`

prevdir=`pwd`
cd $DIR_FOR_REQ

#cat > requirements <<EOF
#macro ATLAS_DIST_AREA "${NICOS_PROJECT_HOME}"
#macro ATLAS_RELEASE ${NICOS_PROJECT_RELNAME}
#use AtlasSettings v* ${dn2}
#EOF

cat > requirements <<EOF
macro ATLAS_DIST_AREA "${NICOS_PROJECT_HOME}"
macro ATLAS_RELEASE ${NICOS_PROJECT_RELNAME}
use AtlasSettings v* /afs/cern.ch/atlas/software/dist
EOF

CMTPATH="${NICOS_RELHOME}"
if [ "${NICOS_ATLAS_BASE_RELEASE}" != "" ]; then
CMTPATH="${CMTPATH}:${NICOS_ATLAS_BASE_RELEASE}"
bname=`basename ${NICOS_ATLAS_BASE_RELEASE}`
dname=`dirname ${NICOS_ATLAS_BASE_RELEASE}`

cat > requirements <<EOF
macro ATLAS_DIST_AREA "${dname}"
macro ATLAS_RELEASE ${bname}
use AtlasSettings v* ${dn2}
EOF

fi
export CMTPATH

echo ">>>>>generated 'initial' req file"
cat requirements
echo ">>>>>"

cmt config

CVSROOT_INI=$CVSROOT
echo ${CMTCONFIG} | grep -i "opt" > /dev/null 2>&1; stat1=$?
echo ${CMTCONFIG} | grep -i "344" > /dev/null 2>&1; stat344=$?
echo ${NICOS_SUFFIX} | grep -i "opt" > /dev/null 2>&1; stat2=$?
echo ${NICOS_SUFFIX} | grep -i "344" > /dev/null 2>&1; stat3441=$?

if [ "$stat1" -eq 0 -o "$stat2" -eq 0 ]; then
  if [ "$stat344" -eq 0 -o "$stat3441" -eq 0 ]; then
  source setup.sh -tag=gcc344,opt
  CMTCONFIG=i686-slc3-gcc344-opt
  else
  source setup.sh -tag=opt
  CMTCONFIG=i686-slc3-gcc323-opt
  fi
else
  if [ "$stat344" -eq 0 -o "$stat3441" -eq 0 ]; then
  source setup.sh -tag=gcc344
  CMTCONFIG=i686-slc3-gcc344-dbg
  else
  source setup.sh
  CMTCONFIG=i686-slc3-gcc323-dbg
  fi 
fi
if [ "${CVSROOT_INI}" != "" ]; then
CVSROOT="${CVSROOT_INI}"
export CVSROOT
fi
                                                                              
rm -rf $DIR_FOR_REQ
cd $prevdir