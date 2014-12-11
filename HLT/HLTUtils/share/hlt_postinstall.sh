#!/usr/bin/env bash
#
# Fixup HLT release after relocation (used in post_install_script macro)
# Author: Frank Winklmeier
#

if [ $# -gt 2 ] || [ "$1" == "-h" ]; then
   cat <<EOF
Usage: hlt_postinstall.sh [RELEASEDIR]

  RELEASEDIR   AtlasHLT release directory (e.g. /afs/cern.ch/atlas/software/builds/AtlasHLT/15.3.0)

Relocates HLT OKS files to the new location given by RELEASEDIR.
If run without arguments RELEASEDIR is deduced from the location of this script.
EOF
   exit 1
fi

if [ $# -eq 1 ]; then
    relhlt=`echo $1|sed "s;/$;;"`  # Remove trailing /
else
    if [ -h $0 ]; then    # if called from InstallArea
        path=`readlink $0`
    else                  # if called via absolute path
        path=$0
    fi
    # Derive release directory from this scripts location
    relhlt=`echo $(cd $(dirname $path); pwd -P)/$(basename $0) | sed -r 's;(.*)/HLT/HLTUtils.*;\1;'`
fi

# Base directory of all Atlas projects
hltrelbase=`echo ${relhlt} | sed -r 's;(.*)/AtlasHLT/.*;\1;'`

hltoks="${relhlt}/HLT/HLToks"
basedir=$AtlasBaseDir

if [ -z "$NICOS_PROJECT_RELNAME_COPY" ]; then  # for kit installation
    version=$AtlasVersion
else                                           # for NICOS
    version=$NICOS_PROJECT_RELNAME_COPY
fi

echo "AtlasHLT dir: $relhlt"
echo "AtlasBaseDir: $basedir"
echo "Version: $version"

source ${AtlasSetup}/scripts/asetup.sh AtlasHLT,${version},setup,notest --brief --input=None --cmtconfig=${CMTCONFIG} --releasebase=${hltrelbase} --appendarea=${basedir}

# HLT-Environment
cd ${hltoks}/${CMTCONFIG}/daq/segments
make_hlt_env.py --tags=AtlasHLT,${version} --asetupargs="--cmtconfig=${CMTCONFIG} --releasebase=${hltrelbase} --appendarea=${basedir}" HLT-Environment.data.xml

# SW Repositories
cd ${hltoks}/${CMTCONFIG}/daq/sw
export TDAQ_DB_PATH=${hltoks}/${CMTCONFIG}:${TDAQ_DB_PATH}
make_hlt_rep.py
make_coralServer_rep.py


