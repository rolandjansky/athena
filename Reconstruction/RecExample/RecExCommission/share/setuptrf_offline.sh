#!/usr/bin/env zsh

#########################################################################
##
##  Script for setting up ATLAS/Athena run-time environment via CMT
##
##  - input parameters:
##     1) CMT version; e.g. 'v1r20p20080222'
##     2) patch directory (CMT test area)
##     3) transuses string, i.e. hyphen-separated 'project-release' string; 
##        e.g. 'AtlasTier0-14.2.10.2'
##     4) special setup to be sourced
##
## (C) Luc Goossens, Armin Nairz (July 2008)
##
#########################################################################

if test $# -ne 4 ; then
     echo "ERROR incorrect nr of args"
     echo "USAGE:  setuptrf.sh CMTVERSION PATCHDIR TRANSUSES SPECIALSETUP"
     echo
     exit 1
fi

export CMTVERSION=$1
export PATCHDIR=$2
export TRANSUSES=$3
export SPECIALSETUP=$4

export PROJECT=`echo ${TRANSUSES} | awk -F '-' '{print $1}'`
export RELEASE=`echo ${TRANSUSES} | awk -F '-' '{print $2}'`

export WORKDIR=`pwd`

# set AthenaPOOL output level
#-------------------------------------------------------------------------------
export POOL_OUTMSG_LEVEL=5


# assemble CMT requirements file
#-------------------------------------------------------------------------------
cat > requirements << EOF
set   CMTSITE  CERN
set   SITEROOT /afs/cern.ch
macro ATLAS_DIST_AREA \${SITEROOT}/atlas/software/dist                           
macro ATLAS_TEST_AREA ${PATCHDIR}
use AtlasLogin AtlasLogin-* \$(ATLAS_DIST_AREA)
EOF

# set up CMT version
#-------------------------------------------------------------------------------
source /afs/cern.ch/sw/contrib/CMT/${CMTVERSION}/mgr/setup.sh ""
cmt config

# set up run environment
#-------------------------------------------------------------------------------
source ${WORKDIR}/setup.sh -tag=${PROJECT},${RELEASE},32,oneTest,runtime

# source special/custom setup
#-------------------------------------------------------------------------------
if test $SPECIALSETUP != 'none' ;
then
  source $SPECIALSETUP
fi
