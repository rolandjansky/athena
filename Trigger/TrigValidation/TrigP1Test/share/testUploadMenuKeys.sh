#!/bin/bash
       
echo 'Testing SMKey upload'
if [ $# -ge 1 ]; then
   type=$1
   echo 'Trying to upload Menu generated  with test "${type}"_menu' 
else
   type=""
fi

#setup the TT
get_files -data -symlink TrigDb.jar
get_files -data -symlink TriggerTool.jar
source /afs/cern.ch/sw/lcg/external/Java/bin/setup.sh
export _JAVA_OPTIONS="-Xms256m -Xmx1048m"
export DBConn="TRIGGERDBATN"


##get the right pattern to load Lvl1 xml file
if [ "$type" == "HLT_LS1V1" ]; then
  stump="LS1_v1"
elif [ "$type" == "HLT_physicsV5" ]; then
  stump="Physics_pp_v5"
else 
  stump=""
fi

#get the L1 file (common) cosmic and IB the same
get_files -xmls -copy LVL1config_"${stump}_"\*.xml
l1menu=`find .  -name LVL1config_${stump}_\*.xml` 

#get the L1 Topo configuration

#get_files -xmls -copy L1Topoconfig_"${stump}_"\*.xml 
l1topo=`find ../"${type}"_menu/ -name L1Topoconfig_\*.xml` 

#prepare files for first key: l2 and ef menus are the same (full menu)
hltmenu1=`find ../"${type}"_menu/ -name outputHLTconfig_\*.xml`

 

 
ConvertHLTSetup_txt2xml.py ../"${type}"_menu/ef_Default_setup.txt ../"${type}"_menu/ef_Default_setup_setup.txt > convertHLT1
ef__setup1=../"${type}"_menu/ef_Default_setup.xml


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

echo "before grep"
#menuname=`grep menu_name $hltmenu1 | cut -f2 -d= | cut -f1 -d" "`
rundate=`date +%F" "%H:%M" "`

# Upload SMK

cmd="java -cp TriggerTool.jar:TrigDb.jar triggertool.TriggerTool -up -release $p1_rel -l1_menu $l1menu -topo_menu $l1topo -hlt_menu $hltmenu1 -hlt_ef_setup $ef__setup1 -name 'P1HLTtest'  -l FINE --SMcomment \"${rundate}${nightly}_${rel}\" -dbConn $DBConn -w_n 25 -w_t 60  >& uploadSMK1"

#cmd="java -jar TriggerTool.jar -up -release $p1_rel -l1_menu $l1menu -hlt_menu $hltmenu1 -hlt_l2_setup $l2__setup1 -hlt_ef_setup $ef__setup1 -name $menuname -l FINE --SMcomment \"${rundate}${nightly}_${rel}\" -dbConn $DBConn -w_n 25 -w_t 60"
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

# Generate and upload prescales
echo "Uploading prescale keys..."
echo 'Skipping this step.. not configured yet'
exit 0

# Run rule book
lumi=1000
costxml=TriggerCosts_Physics_pp_v5.xml
get_files -xmls -symlink $costxml

cmdps="running: processRules.py --rulebook=Physics_pp_v5_rules --log=rulebook.log --force-rates-metadata --use_lowest_rule --target_lumi=$lumi --target_filled=1318 --target_empty=350 --target_unp_iso=60 --target_unp_noniso=60 --lvl1-xml=$l1menu --hlt-xml=$hltmenu1 --rates-xml=$costxml"
echo $cmd
eval $cmd 


# Convert to uploadable XML file
echo "Generating RuleBook_HLTPS_Physics${lumi}.xml by running\ncnvXML.py --ps_name=Physics${lumi} --ps_xml=prescales${lumi}.xml"
cnvXML.py --ps_name=Physics${lumi} --ps_xml=prescales${lumi}.xml

# Upload
java -jar TriggerTool.jar -dbConn $DBConn -psup RuleBook_HLTPS_Physics${lumi}.xml -smk $smk -w_n 5 -w_t 60 &> uploadPSK.log
hltpsk2=`grep 'HLT Prescale set saved with id' uploadPSK.log | sed 's#.*: \([0-9]*\)\.#\1#'`
if [ -z "$hltpsk2" ]; then
    echo "ERROR Upload of prescale key failed"
    exit 1
fi

echo $hltpsk1 > "prescales.txt"
echo $hltpsk2 >> "prescales.txt"
