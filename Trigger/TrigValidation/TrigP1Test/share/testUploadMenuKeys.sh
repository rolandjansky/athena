#!/bin/bash
       
echo 'Testing SMKey upload'
if [ $# -ge 1 ]; then
   type=$1
   echo "Trying to upload Menu generated  with test "${type}"_menu" 
else
   type=""
fi

#setup the TT
export _JAVA_OPTIONS="-Xms512m -Xmx1048m"
export DBConn="TRIGGERDBATN"

export PATH=$PATH:$TDAQ_JAVA_HOME/bin

export TNS_ADMIN=/afs/cern.ch/atlas/offline/external/oracle/latest/admin

##get the right pattern to load LVl1 xml file
if [ "$type" == "HLT_LS1V1" ]; then
  stump="LS1_v1"
elif [ "$type" == "HLT_physicsV5" ]; then
  stump="Physics_pp_v5"
elif [ "$type" == "HLT_physicsV5_rerunLVL1" ]; then
  stump="Physics_pp_v5"
elif [ "$type" == "HLT_physicsV6" ]; then
  stump="Physics_pp_v6"
elif [ "$type" == "HLT_physicsV6_rerunLVL1" ]; then
  stump="Physics_pp_v6"
elif [ "$type" == "HLT_physicsV6" ]; then
  stump="Physics_pp_v6"
elif [ "$type" == "HLT_physicsV6_rerunLVL1" ]; then
  stump="Physics_pp_v6"
elif [ "$type" == "HLT_physicsV7" ]; then
  stump="Physics_pp_v7"
elif [ "$type" == "HLT_physicsV7_rerunLVL1" ]; then
  stump="Physics_pp_v7"
elif [ "$type" == "HLT_physicsV7" ]; then
  stump="Physics_pp_v7"
elif [ "$type" == "HLT_physicsV7_rerunLVL1" ]; then
  stump="Physics_pp_v7"
else 
  stump=""
fi

#get the L1 file (common) cosmic and IB the same
get_files -xmls -copy LVL1config_"${stump}".xml
l1menu=`find .  -name LVL1config_${stump}.xml` 

#get the L1 Topo configuration
get_files -xmls -copy L1Topoconfig_"${stump}".xml 
l1topo=`find . -name L1Topoconfig_${stump}.xml` 

#prepare files for first key: l2 and ef menus are the same (full menu)
hltmenu1=`find ../"${type}"_menu/ -name outputHLTconfig_\*.xml`


# copy the setup files to the local directory to have tests independent of each other
cp ../"${type}"_menu/ef_Default_setup.txt ../"${type}"_menu/ef_Default_setup_setup.txt .
echo "ConvertHLTSetup_txt2xml.py ef_Default_setup.txt ef_Default_setup_setup.txt > convertHLT1"
ConvertHLTSetup_txt2xml.py ef_Default_setup.txt ef_Default_setup_setup.txt > convertHLT1

convertLog=convertHLT1

if grep -q /afs/ "$convertLog" || grep -q /cvmfs/ "$convertLog" ; then
   echo "afs or cvmfs path found in setup, upload failed, please check the logfiles "
   exit 1
fi

hlt__setup1=ef_Default_setup.xml


# get dtd file for L1 menu
get_files -xmls LVL1config.dtd



p1_rel="AthenaP1"
if [ $NICOS_ATLAS_RELEASE ]
then
    p1_rel=$NICOS_ATLAS_RELEASE
fi

nightly="AthenaP1Test"
if [ $NICOS_NIGHTLY_NAME ]
then
    nightly="$NICOS_NIGHTLY_NAME"
fi

rel=""
if [ $NICOS_PROJECT_RELNAME_COPY ]
then
    rel=$NICOS_PROJECT_RELNAME_COPY
fi

#menuname=`grep menu_name $hltmenu1 | cut -f2 -d= | cut -f1 -d" "`
rundate=`date +%F" "%H:%M" "`

# Upload SMK

cmd="/afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh -up -release $p1_rel --l1_menu $l1menu --topo_menu $l1topo -hlt $hltmenu1 --hlt_setup $hlt__setup1 --name 'AthenaP1Test' -l INFO --SMcomment \"${rundate}${nightly}_${rel}\" --dbConn $DBConn -w_n 50 -w_t 60"

echo $cmd "&> uploadSMK.log"
eval $cmd &> uploadSMK.log

if [ ! -f MenusKeys.txt ]
then
    echo 'ERROR Upload of SMKey failed'
    echo 'In ./uploadSMK.log:'
    grep "Can't obtain write lock" uploadSMK.log
    grep "SEVERE" uploadSMK.log
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
costxml=TriggerCosts_Physics_pp_v6.xml
get_files -xmls -symlink $costxml

cmdps="running: processRules.py --rulebook=Physics_pp_v6_rules --log=rulebook.log --force-rates-metadata --use_lowest_rule --target_lumi=$lumi --target_filled=1318 --target_empty=350 --target_unp_iso=60 --target_unp_noniso=60 --lvl1-xml=$l1menu --hlt-xml=$hltmenu1 --rates-xml=$costxml"
echo $cmd
eval $cmd 


# Convert to uploadable XML file
echo "Generating RuleBook_HLTPS_Physics${lumi}.xml by running\ncnvXML.py --ps_name=Physics${lumi} --ps_xml=prescales${lumi}.xml"
cnvXML.py --ps_name=Physics${lumi} --ps_xml=prescales${lumi}.xml

# Upload
afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh -dbConn $DBConn -psup RuleBook_HLTPS_Physics${lumi}.xml -smk $smk -w_n 50 -w_t 60 &> uploadPSK.log
hltpsk2=`grep 'HLT Prescale set saved with id' uploadPSK.log | sed 's#.*: \([0-9]*\)\.#\1#'`
if [ -z "$hltpsk2" ]; then
    echo "ERROR Upload of prescale key failed"
    echo 'In ./uploadSMK.log:'
    grep "Can't obtain write lock" uploadPSK.log
    grep "SEVERE" uploadPSK.log
    exit 1
fi

echo $hltpsk1 > "prescales.txt"
echo $hltpsk2 >> "prescales.txt"
