#!/usr/bin/env zsh
# Script to generate generate the trigger menu XML files
#

if [ $# -ne 1 ]; then
    echo "Syntax:" `basename $0` "<menu>"
    echo "e.g." `basename $0` "Physics_pp_v4"
    exit 1
fi

menu=$1
jo=TriggerMenuXML/runHLT_forXMLgeneration.py
release=`python -c "from TriggerJobOpts import TriggerFlags; print(TriggerFlags.menuVersion().get_Value())" | tail -1`
logfile=${menu}.log
origdir=`pwd -P`


# Temporary run directory and cleanup traps in case of termination
rundir=`mktemp -t -d tmxml.${menu}.XXXXXXXXXX`
cd $rundir
TRAPINT() {
    rm -rf $rundir
    return 130 # 128+SIGINT
}
TRAPTERM() {
    rm -rf $rundir
    return 143 # 128+SIGTERM
}


echo "CreateTriggerMenuXML: *** Building menu $menu for $release ***"

athena.py -c "TriggerMenuSetup='$menu'" TriggerMenuXML/runHLT_forXMLgeneration.py  2>&1  >&!  $logfile
athena_exit=$?
cp $logfile ${origdir}
if [ $athena_exit -eq 0 ]; then
    echo "CreateTriggerMenuXML: *** $menu DONE | Exit code: $athena_exit | Log: $logfile ***"
else
    echo "CreateTriggerMenuXML: *** $menu FAILED | Exit code: $athena_exit | Log: $logfile ***"
fi


if [[ -e outputLVL1config.xml ]]; then
    cp outputLVL1config.xml ${origdir}/LVL1config_${menu}_${release}.xml
    cp outputHLTconfig.xml ${origdir}/HLTconfig_${menu}_${release}.xml
fi

#this gives some more sensitivity to hidden problems
grep --colour ERROR $logfile
grep --colour -A 100 "Shortened traceback" $logfile

cd $origdir
rm -rf $rundir
exit $athena_exit
