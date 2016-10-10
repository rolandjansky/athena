#!/bin/bash
       
echo 'Preparing SMKey for CAF reprocessing'

#setup the TT
get_files -data -symlink TriggerTool.jar
export JAVA_VER="1.8.0"
source /afs/cern.ch/sw/lcg/external/Java/bin/setup.sh
export _JAVA_OPTIONS="-Xms256m -Xmx1048m"
export DBConn="TRIGGERDBREPR"

#get the L1 file (common) for v4 menu
l1menu=LVL1config_Physics_pp_v4.xml
get_files -xmls -copy $l1menu

#prepare files for first key: l2 and ef menus are the same (full menu)
hltmenu1=`find ../AllMT_mcV4_caf/ -name outputHLTconfig_L2PU\*.xml`

cp ../AllMT_mcV4_caf/l2_Default_setup.txt .
cp ../AllMT_mcV4_caf/l2_Default_setup_setup.txt .
cp ../AllPT_mcV4_caf/ef_Default_setup.txt .
cp ../AllPT_mcV4_caf/ef_Default_setup_setup.txt .

ConvertHLTSetup_txt2xml.py ./l2_Default_setup.txt ./l2_Default_setup_setup.txt > convertMT1
ConvertHLTSetup_txt2xml.py ./ef_Default_setup.txt ./ef_Default_setup_setup.txt > convertPT1
l2__setup1=./l2_Default_setup.xml
ef__setup1=./ef_Default_setup.xml


p1_rel="P1HLT"
if [ $NICOS_ATLAS_RELEASE ]
then
    p1_rel=$NICOS_ATLAS_RELEASE
fi

nightly="P1HLTTest"
if [ $NICOS_NIGHTLY_NAME ]
then
    nightly="$NICOS_NIGHTLY_NAME"
fi

rel=""
if [ $NICOS_PROJECT_RELNAME_COPY ]
then
    rel=$NICOS_PROJECT_RELNAME_COPY
fi

menuname=`grep menu_name $hltmenu1 | cut -f2 -d= | cut -f1 -d" "`
rundate=`date +%F" "%H:%M" "`

# Prepare SMK command
echo "export TRIGGER_EXP_CORAL_PATH=/afs/cern.ch/user/a/attrgcnf/.expertauth" > uploadSMK.sh
cmd="java -jar TriggerTool.jar -up -release $p1_rel -l1_menu $l1menu -hlt_menu $hltmenu1 -hlt_l2_setup $l2__setup1 -hlt_ef_setup $ef__setup1 -name $menuname -l FINE --SMcomment \"${rundate}${nightly}_${rel}\" -dbConn $DBConn"
echo $cmd >> uploadSMK.sh
if [ ! -f uploadSMK.sh ]
then
    echo ' ERROR preparation of SMKey failed'
    exit 1
fi

