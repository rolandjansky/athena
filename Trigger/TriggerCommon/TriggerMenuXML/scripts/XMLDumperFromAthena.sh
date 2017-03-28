#!/usr/bin/env zsh
# Script to generate the trigger menu XML files
#

if [ $# -lt 1 ]; then
    echo "Syntax: XMLDumperFromAthena.sh MENU [DEST]"
    exit 1
fi

menu=$1
dest=$2
if [ -z "$dest" ]; then
    dest=$PWD
fi

# Get absolute path name
dest=`cd $dest; pwd`

jo=TriggerMenuXML/runHLT_forXMLgeneration.py
release=${AtlasVersion}

# The following would be better but doesn't seem to work (yet) in git builds
#release=`python -c "from TriggerJobOpts import TriggerFlags; print(TriggerFlags.menuVersion().get_Value())" | tail -1`

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
echo "XMLDumperFromAthena: Building menu: ${menu} for ${release}"
logfiletopo=topo${menu}.log
logfile=${menu}.log
cd $rundir

# L1Topo config file
generateL1TopoMenu.py $menu >&! $logfiletopo
cp L1Topoconfig_*.xml ${dest}

# L1 + HLT config file
if [ -z "$TMXML_DEBUG" ]; then
    MSGLVL=""
else
    MSGLVL="-lDEBUG"
fi
athena.py $MSGLVL -c "TriggerMenuSetup='$menu'" $jo >&! $logfile
athena_exit=$?

cp $logfile $logfiletopo ${dest}
if [ $athena_exit -eq 0 ]; then
    echo "XMLDumperFromAthena: $menu DONE | Exit code: $athena_exit | Log: $dest/$logfile"
else
    echo "XMLDumperFromAthena: $menu FAILED | Exit code: $athena_exit | Log: $dest/$logfile"
fi

if [[ -e outputLVL1config.xml ]]; then
    cp outputLVL1config.xml ${dest}/LVL1config_${menu}_${release}.xml
    cp outputHLTconfig.xml ${dest}/HLTconfig_${menu}_${release}.xml
fi

#this gives some more sensitivity to hidden problems
grep --colour ERROR ${dest}/$logfile
grep --colour -A 100 "Shortened traceback" ${dest}/$logfile

rm -rf $rundir

# Do not return real athena exit code as we want to pretend everything was fine

