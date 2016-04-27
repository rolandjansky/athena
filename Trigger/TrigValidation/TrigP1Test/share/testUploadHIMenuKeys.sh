#!/bin/bash
       
echo 'Testing SMKey upload for HIV2 menu'

#setup the TT
get_files -data -symlink TriggerTool.jar
export JAVA_VER="1.8.0"
source /afs/cern.ch/sw/lcg/external/Java/bin/setup.sh
export _JAVA_OPTIONS="-Xms256m -Xmx1048m"
export DBConn="TRIGGERDBATN"

#get the L1 file (common) cosmic and IB the same
l1menu=LVL1config_Physics_HI_v2.xml
get_files -xmls -copy $l1menu

#prepare files for first key: l2 and ef menus are the same (full menu)
hltmenu1=`find ../AllMT_HIV2_menu/ -name outputHLTconfig_L2PU\*.xml`

cp ../AllMT_HIV2_menu/l2_Default_setup.txt .
cp ../AllMT_HIV2_menu/l2_Default_setup_setup.txt .
cp ../AllPT_HIV2_menu/ef_Default_setup.txt .
cp ../AllPT_HIV2_menu/ef_Default_setup_setup.txt .

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

# Upload SMK

cmd="java -jar TriggerTool.jar -up -release $p1_rel -l1_menu $l1menu -hlt_menu $hltmenu1 -hlt_l2_setup $l2__setup1 -hlt_ef_setup $ef__setup1 -name $menuname -l FINE --SMcomment \"${rundate}${nightly}_${rel}\" -dbConn $DBConn -w_n 50 -w_t 60" 
echo $cmd
eval $cmd &> uploadSMK.log

if [ ! -f MenusKeys.txt ]
then
    echo 'ERROR Upload of SMKey failed'
    exit 1
fi
smk=`grep SM MenusKeys.txt | awk '{print $3}' | sed 's#:##'`
l1psk=`grep 'L1 PS' MenusKeys.txt | awk '{print $4}' | sed 's#:##'`
hltpsk1=`grep 'HLT PS' MenusKeys.txt | awk '{print $4}' | sed 's#:##'`
echo "Successfully uploaded SMK:"
cat MenusKeys.txt

# Created shell script to be sourced by other ATN tests using these keys
echo "smk=${smk}" > exportMenuKeys.sh
echo "l1psk=${l1psk}" >> exportMenuKeys.sh
echo "hltpsk=${hltpsk1}" >> exportMenuKeys.sh

echo "finished uploading"