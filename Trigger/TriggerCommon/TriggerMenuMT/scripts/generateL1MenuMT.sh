#!/usr/bin/env sh
# Script to generate the trigger LVL1 & L1Topo menu XML files
# Based on TriggerMenuXML/XMLDumperFromAthena.sh

help() {
    echo "Syntax: generateL1MenuMT.sh [-r VERSION] MENU [DEST]"
}

if [ $# -lt 1 ]; then
    help
    exit 0
fi

while true; do
    case "$1" in
        -r)
            release=$2
            shift 2
            ;;
        -h)
            help
            exit 0
            ;;
        -*)
            echo "Invalid parameter"
            exit 1
            ;;
        *)
            break
    esac
done

#if [ -z ${PYTHONDONTWRITEBYTE+x} ]; then export PYTHONDONTWRITEBYTECODE=1; fi # don't write .pyc files, keep source directory clean

menu=$1
dest=$2
if [ -z "$dest" ]; then
    dest=$PWD
fi

# Get absolute path name
dest=`cd $dest; pwd`

if [ -z "$release" ]; then
    release=${AtlasVersion}    # for interactive use
fi

# Temporary run directroy and cleanup traps in case of termination
rundir=`mktemp -t -d tmxml.${menu}.XXXXXXXXXX`

TRAPINT() {
    rm -rf $rundir
    return 130 # 128+SIGINT
}
TRAPTERM() {
    rm -rf $rundir
    return 143 # 128+SIGTERM
}


## menu generation starts here
echo "generateL1MenuMT: Building menu: ${menu} for ${release}"

logfiletopo=topo_${menu}.log
logfilelvl1=lvl1_${menu}.log

cd $rundir

generateLVL1MenuMT.py   $menu 2>&1 >> $logfilelvl1 
generateL1TopoMenuMT.py $menu 2>&1 >> $logfiletopo 

cp L1Topoconfig_*.xml ${dest}
cp LVL1config_*.xml ${dest}
cp $logfilelvl1 ${dest}
cp $logfiletopo ${dest}

if [[ -e outputLVL1config.xml ]]; then
    cp outputLVL1config.xml ${dest}/LVL1config_${menu}_${release}.xml
fi

#this gives some more sensitivity to hidden problems
grep --colour ERROR ${dest}/$logfile/*.log
grep --colour -A 100 "Shortened traceback" ${dest}/$logfile/*.log

rm -rf $rundir

# Do not return real athena exit code as we want to pretend everything was fine
#unset PYTHONDONTWRITEBYTECODE
