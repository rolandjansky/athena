#!/usr/bin/env bash
#
# Fixup HLT release after relocation (used in post_install_script macro)
# Author: Frank Winklmeier
#

if [ $# -gt 2 ] || [ "$1" == "-h" ]; then
   cat <<EOF
Usage: hlt_relocate.sh [RELEASEDIR]

  RELEASEDIR   AtlasHLT release directory (e.g. /afs/cern.ch/atlas/software/builds/AtlasHLT/15.3.0)

Relocates HLT-Environment.data.xml to the new location given by RELEASEDIR.
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
relbase=`echo ${relhlt} | sed -r 's;(.*)/AtlasHLT/.*;\1;'`

# Get version
version=`echo $relhlt | sed -r 's;.*/(.*)$;\1;'`

echo "Release directory: $relbase"
echo "Release version: $version"


getValue()
{
    file=$1
    name=$2
    value=`cat $file | grep -A4 "id=\"${name}\""\
        | grep \"Value\" | sed 's;\s*<attr.*>"\(.*\)"</attr>;\1;'`

    if [ -z "$value" ]; then
        echo "Could not find value of variable $name"
        exit 1
    fi
    echo "$value"
}

relocate_env()
{
    hltenv=$1
    echo "Processing $hltenv"

    instpath=`getValue $hltenv HLT-ATLAS_BASE` || { echo $instpath; exit 1; }
    oldversion=`getValue $hltenv HLT-AtlasArea` || { echo $oldversion; exit 1; }
    oldversion=`basename $oldversion`
    
    echo "  ATLAS_BASE: $instpath --> $relbase"
    TMPFILE=`mktemp` || exit 1
    trap 'rm -f $TMPFILE' INT TERM EXIT
    echo "  Version: $oldversion --> $version"
    cat $hltenv | sed "s;${instpath};${relbase};" | sed "s;${oldversion};${version};g" > $TMPFILE
    
    cp $TMPFILE $hltenv || { echo "Saving modified file:"; cp -v $TMPFILE ./HLT-Environment.data.xml ; exit 1; }
}

relocate_sw()
{
    swrep=$1
    echo "Processing $swrep"
    
    TMPFILE=`mktemp` || exit 1
    trap 'rm -f $TMPFILE' INT TERM EXIT
    echo "  Version: $oldversion --> $version"
    cat $swrep | sed "s;${oldversion};${version};g" > $TMPFILE
    
    cp $TMPFILE $swrep || { echo "Saving modified file:"; cp -v $TMPFILE ./HLT_SW_Repository.data.xml ; exit 1; }
}

hltenv=`ls ${relhlt}/InstallArea/*/data/daq/segments/HLT-Environment.data.xml`
if [ -z "$hltenv" ]; then
    echo "No HLT-Environment.data.xml found"
    exit 1
fi

for f in $hltenv; do
    relocate_env $f
done

hltswrep="${relhlt}/InstallArea/share/data/daq/sw/HLT_SW_Repository.data.xml"
if [ ! -f $hltswrep ]; then
    echo "No HLT_SW_Repository found"
    exit 1
fi
relocate_sw $hltswrep

#
