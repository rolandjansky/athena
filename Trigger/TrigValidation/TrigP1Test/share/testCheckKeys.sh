#!/bin/bash
       
echo 'Testing duplicate key upload'
if [ $# -ge 1 ]; then
   type=$1
   echo "Trying to upload Menu generated with tests with "${type}"_menu and "${type}"_rerun in name" 
else
   type=""
fi

#setup the TT
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
get_files -xmls -copy LVL1config_"${stump}"\*.xml
l1menu=`find .  -name LVL1config_\*.xml`  

#prepare files for first key: l2 and ef menus are the same (full menu)
hltmenu1=`find ../"${type}"_menu/ -name HLTconfig_\*.xml` 
#ConvertHLTSetup_txt2xml.py ../AllMT_physicsV5_menu/l2_Default_setup.txt ../AllMT_physicsV5_menu/l2_Default_setup_setup.txt > convertMT1
ConvertHLTSetup_txt2xml.py ../"${type}"_menu/ef_Default_setup.txt ../"${type}"_menu/ef_Default_setup_setup.txt > convertHLT1
#l2__setup1=../AllMT_physicsV5_menu/l2_Default_setup.xml
ef__setup1=../"${type}"_menu/ef_Default_setup.xml


#upload the first key
echo "upload the first key"
#echo java -jar TriggerTool.jar -up -release "P1HLT" -l1_menu $l1menu -hlt_menu $hltmenu1 -hlt_l2_setup $l2__setup1 -hlt_ef_setup $ef__setup1 -name "P1HLTtest" -l FINE -dbConn $DBConn -w_n 25 -w_t 60
#java -jar TriggerTool.jar -up -release "P1HLT" -l1_menu $l1menu -hlt_menu $hltmenu1 -hlt_l2_setup $l2__setup1 -hlt_ef_setup $ef__setup1 -name "P1HLTtest" -l FINE -dbConn $DBConn -w_n 25 -w_t 60  >& uploadSMK1

echo java -jar TriggerTool.jar -up -release "P1HLT" -l1_menu $l1menu -hlt_menu $hltmenu1  -hlt_ef_setup $ef__setup1 -name "P1HLTtest" -l FINE -dbConn $DBConn -w_n 25 -w_t 60
java -jar TriggerTool.jar -up -release "P1HLT" -l1_menu $l1menu -hlt_menu $hltmenu1 -hlt_ef_setup $ef__setup1 -name "P1HLTtest" -l FINE -dbConn $DBConn -w_n 25 -w_t 60  >& uploadSMK1



if [ ! -f MenusKeys.txt ]
then
    echo '... ERROR Upload of key 1 failed'
    exit 1
fi

mv MenusKeys.txt MenusKeys1.txt

#prepare files for second key: l2 and ef menus are the same (full menu)
hltmenu2=`find ../"${type}"_rerun/ -name HLTconfig_\*.xml`
ConvertHLTSetup_txt2xml.py ../"${type}"_rerun/ef_Default_setup.txt ../"${type}"_rerun/ef_Default_setup_setup.txt > convertHLT2
ef__setup2=../"${type}"_rerun/ef_Default_setup.xml


#upload the second key
echo "upload the second key"
echo java -jar TriggerTool.jar -up -release "P1HLT" -l1_menu $l1menu -hlt_menu $hltmenu2  -hlt_ef_setup $ef__setup2 -name "P1HLTtest" -l FINE -dbConn $DBConn -w_n 25 -w_t 60 >& uploadSMK

java -jar TriggerTool.jar -up -release "P1HLT" -l1_menu $l1menu -hlt_menu $hltmenu2  -hlt_ef_setup $ef__setup2 -name "P1HLTtest" -l FINE -dbConn $DBConn -w_n 25 -w_t 60 >& uploadSMK

if [ ! -f MenusKeys.txt ]
then
    echo '... ERROR Upload of key 2 failed'
    exit 1
fi

mv MenusKeys.txt MenusKeys2.txt

smk1=`grep SM MenusKeys1.txt | cut -f8 -d" "| cut -f1 -d":"`
smk2=`grep SM MenusKeys2.txt | cut -f8 -d" "| cut -f1 -d":"`

echo "diff key 1 vs key 2"
java -jar TriggerTool.jar -diff -smk1 $smk1 -smk2 $smk2 -name "P1HLTtest" -dbConn $DBConn -xml diff_smk_${smk1}_${smk2}.xml -w_n 25 -w_t 60

echo "checking L2" 

#sort $l2__setup1 | grep -v MuonCalBufferName | grep -v DataFlowConfig > l2_1
#sort $l2__setup2 | grep -v MuonCalBufferName | grep -v DataFlowConfig > l2_2

#echo "sorted properties"

# make diffs
#echo "diffing L2 setups , first pass , second pass "
#diff l2_1 l2_2 > l2_diff.txt
#l2_diff_status=$?
#echo "diff status " $l2_diff_status

echo "checking EF"

sort $ef__setup1 | grep -v DataFlowConfig > ef_1
sort $ef__setup2 | grep -v DataFlowConfig > ef_2

echo "diffing EF setups , first pass , second pass "

diff ef_1 ef_2 > ef_diff.txt
ef_diff_status=$?

echo "diff status " $ef_diff_status

#if [ $l2_diff_status != 0 -o $ef_diff_status != 0 ]
if [  $ef_diff_status != 0 ]
then
#    echo "... ERROR Non identical keys found. See diff_smk_${smk1}_${smk2}.xml, l2_diff.txt and ef_diff.txt in test directory"
    echo "... ERROR Non identical keys found. See diff_smk_${smk1}_${smk2}.xml,   ef_diff.txt in test directory"
    echo "... diff_smk_${smk1}_${smk2}.xml ..."
    cat diff_smk_${smk1}_${smk2}.xml
 #   echo "... l2_diff.txt ..."
 #   cat l2_diff.txt
    echo "... ef_diff.txt ..."
    cat ef_diff.txt
    exit 1
else
    echo 'Identical keys found: PASS'
    echo "... diff_smk_${smk1}_${smk2}.xml ..."
    cat diff_smk_${smk1}_${smk2}.xml
#    echo "... l2_diff.txt ..."
#    cat l2_diff.txt
    echo "... ef_diff.txt ..."
    cat ef_diff.txt
    exit 0
fi
