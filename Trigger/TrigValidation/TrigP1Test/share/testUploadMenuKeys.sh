#!/bin/bash
       
echo 'Testing SMKey upload'

# defaults
uploadPrescale=0
noUpload=0
database="ATN"
type=""

while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    --menu)
    type=$2
    echo "Trying to upload Menu generated  with test "${type}"_menu" 
    shift # past argument
    shift # past value
    ;;
    --uploadPrescale)
    uploadPrescale=1
    shift # past argument
    ;;
    --notUploadMenu)
    noUpload=1
    shift # past argument
    ;;
    --art)
    database="ART"
    shift # past argument
    ;;
    *)    # unknown option
    echo "unkown option "$key
    exit 1
    ;;
esac
done

echo "menu "$type
echo "uploadPrescale "$uploadPrescale
echo "noUpload "$noUpload
echo "database "$database

#setup the TT
export DBConn="TRIGGERDB${database}"
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
elif [ "$type" == "HLT_physicsV7" ]; then
  stump="Physics_pp_v7"
elif [ "$type" == "HLT_physicsV7_rerunLVL1" ]; then
  stump="Physics_pp_v7"
elif [ "$type" == "HLT_mcV7" ]; then
  stump="MC_pp_v7"
elif [ "$type" == "HLT_mcV7_rerunLVL1" ]; then
  stump="MC_pp_v7"
elif [ "$type" == "HLT_HIV5" ]; then
  stump="Physics_HI_v5"
elif [ "$type" == "HLT_HIV5_rerunLVL1" ]; then
  stump="Physics_HI_v5"
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



#echo $AthenaP1_VERSION -> 21.1.23
echo "AthenaP1_VERSION, ${AthenaP1_VERSION}"
#get prescale files

prescale_type="tight"
if [[ $stump =~ .*MC.* ]]; then
  menu_type="mc"
  prescale_type="TriggerValidation"
elif [[ $stump =~ .*Physics.* ]]; then
  menu_type="physics"
  prescale_type="tight"
fi

get_files -xmls HLTconfig_"${stump}"_"${prescale_type}"_"${menu_type}"_prescale_"${AthenaP1_VERSION}".xml
hltmenu1_prescale=`find . -name HLTconfig_\*_${prescale_type}_\*.xml`

mkdir PS_${prescale_type}
cp $l1menu PS_${prescale_type}/.
cp $hltmenu1_prescale PS_${prescale_type}/.

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
l1_schema=`find . -name LVL1config.dtd`
cp $l1_schema PS_${prescale_type}/.

p1_rel="AthenaP1"
if [ $NICOS_ATLAS_RELEASE ]
then
    p1_rel=$NICOS_ATLAS_RELEASE
elif [ $AtlasBuildBranch ]
then
    p1_rel=${AtlasBuildBranch}-${AtlasBuildStamp}
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
elif [ $AtlasBuildStamp ]
then
    rel=${AtlasBuildStamp}
fi

#menuname=`grep menu_name $hltmenu1 | cut -f2 -d= | cut -f1 -d" "`
rundate=`date +%F","%H:%M","`

echo "p1_rel=${p1_rel}"
echo "nightly=${nightly}"
echo "rel=${rel}"
echo "rundate=${rundate}"

# Upload SMK

cmd="/afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh -up -release $p1_rel --l1_menu $l1menu --topo_menu $l1topo -hlt $hltmenu1 --hlt_setup $hlt__setup1 --name 'AthenaP1Test' -l INFO --SMcomment \"${rundate}${nightly}_${rel}\" --dbConn $DBConn -w_n 50 -w_t 60"

# create script to upload keys
echo "# setup release" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "asetup Athena,21.3,r${rel}" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "# creating SMK" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "if [ -f MenusKeys.txt ]; then" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "   rm MenusKeys.txt" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "fi" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "echo 'Uploading SMK. It may take a while...'" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "/afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh -up -release "$p1_rel" --l1_menu "$l1menu" --topo_menu "$l1topo" -hlt "$hltmenu1" --hlt_setup "$hlt__setup1" --name '"$stump"_"$prescale_type",AthenaP1Test' -l INFO --SMcomment '"${rundate}${nightly}_${rel}"' --dbConn TRIGGERDBREPR -onl  -w_n 50 -w_t 60 &> SMK_upload.log" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "if grep --quiet SEVERE SMK_upload.log; then" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "  echo 'SEVERE error occured, maybe existing write lock. Please check SMK_upload.log' " >> uploadSMK_"$stump"_"$prescale_type".sh
echo "  return " >> uploadSMK_"$stump"_"$prescale_type".sh
echo "elif [ ! -f MenusKeys.txt ]; then" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "  echo 'MenusKeys.txt does not exist. Something went wrong. Please check SMK_upload.log' " >> uploadSMK_"$stump"_"$prescale_type".sh
echo "  return " >> uploadSMK_"$stump"_"$prescale_type".sh
echo "fi" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "smk=\`grep SM MenusKeys.txt | awk '{print \$3}' | sed 's#:##'\`" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "echo 'Created SMK ' \$smk" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "# upload prescaled" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "echo 'Uploading prescale keys. It may take another while...'" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "/afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh --dbConn TRIGGERDBREPR -psup PS_"$prescale_type" -smk \${smk} -w_n 50 -w_t 60 &> PS_upload.log" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "if grep -v 'LVL1config.dtd' PS_upload.log | grep --quiet SEVERE ; then" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "  echo 'SEVERE error occured, maybe existing write lock. Please check PS_upload.log' " >> uploadSMK_"$stump"_"$prescale_type".sh
echo "  return " >> uploadSMK_"$stump"_"$prescale_type".sh
echo "fi" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "l1psk=\`grep  'INFO: Prescale set saved with id' PS_upload.log | awk '{print \$7}' | sed 's/,//g' | sed 's/\.//g'\`" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "hltpsk=\`grep 'INFO: HLT Prescale set saved with id' PS_upload.log | awk '{print \$8}' | sed 's/,//g' | sed 's/\.//g'\`" >> uploadSMK_"$stump"_"$prescale_type".sh
echo "echo 'L1 PSK ' \$l1psk ', HLT PSK ' \$hltpsk" >> uploadSMK_"$stump"_"$prescale_type".sh


if [ $noUpload -eq 1 ]; then
  exit 0
fi

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
if ! [ $uploadPrescale -eq 1 ]; then
  exit 0
fi

echo "Uploading prescale keys..."
# the upload of the xmls is now done standalone following the discussion on ATR-16799

# test checking out RB with atnight user
TrigP1Test_ART_dir=${PWD}
echo 'TrigP1Test_ART_dir: '${TrigP1Test_ART_dir}
MENU='Physics_pp_v7'
echo 'Menu:' ${MENU}
export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source $ATLAS_LOCAL_ROOT_BASE/packageSetups/localSetup.sh git
#TODO: at the moment working on RB master
git clone https://:@gitlab.cern.ch:8443/atlas-trigger-menu/TrigMenuRulebook.git
RB_dir=${PWD}/TrigMenuRulebook
echo 'RB_dir: '${RB_dir}
echo 'l1menu: '${l1menu}
echo 'l1topo: '${l1topo}
echo 'hltmenu: '${hltmenu1}
cd TrigMenuRulebook/scripts

#try to remove problematic link
rm TrigMenuRulebook
cp -r ../python TrigMenuRulebook

rm -f l1.xml hlt.xml

ln -s ../../${l1menu}   l1.xml
ln -s ../../${hltmenu1}   hlt.xml
ls -alhtr

# RB configured to ingnore errors and leave unprescaled any new chain not yet defined in RB. It won't try to copy keys to P1
sed -i -e 's/ignoreErrors\s*=\s*False/ignoreErrors = True/g' runOptions.py
sed -i -e 's/doUnprescaledIfUndefined\s*=\s*False/doUnprescaledIfUndefined = True/g' runOptions.py
sed -i -e 's/doUseOnline\s*=\s*True/doUseOnline = False/g' runOptions.py
sed -i -e 's/doPbPb\s*=\s*True/doPbPb = False/g' runOptions.py

./runRuleBook.py 17000
cd ../..
PSdir=`find TrigMenuRulebook/scripts -name "prescales_*" -type d`
echo "PSdir: "${PSdir}
rm $PSdir/Set_*.xml
rm $PSdir/*Emittance*
rm $PSdir/*Standby*
ls $PSdir

# upload PS keys
cmd="/afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh --dbConn $DBConn -psup $PSdir -smk $smk -w_n 50 -w_t 60"
echo $cmd " &> uploadPSK_prescaled.log"
eval $cmd &> uploadPSK_prescaled.log

hltpsk2=`grep 'INFO: HLT Prescale set saved with id' uploadPSK_prescaled.log | sed 's#.*: \([0-9]*\)\.#\1#'`
l1psk2=`grep 'INFO: Prescale set saved with id' uploadPSK_prescaled.log | sed 's#.*: \([0-9]*\)\.#\1#'`
if [ -z "$hltpsk2" ] || [ -z "$l1psk2" ]; then
    echo "ERROR Upload of prescale key failed"
    echo 'In ./uploadPSK_prescaled.log:'
    grep "Can't obtain write lock" uploadPSK_prescaled.log
    grep "SEVERE" uploadPSK_prescaled.log
    rm -rf TrigMenuRulebook
    exit 1
fi

echo "smk=${smk}" > prescaleKeys_17000.txt
echo "l1psk=${l1psk2}" >> prescaleKeys_17000.txt
echo "hltpsk=${hltpsk2}" >> prescaleKeys_17000.txt
echo "echo 'setting these keys: SMK ' \$smk ', L1 PSK ' \$l1psk ', HLT PSK ' \$hltpsk  " >> prescaleKeys_17000.txt
echo "Successfully uploaded prescales: l1psk=${l1psk2} and hltpsk=${hltpsk2}"

rm uploadPSK_prescaled.log

cd TrigMenuRulebook/scripts
rm -r prescales_*

./runRuleBook.py 9000
cd ../..
PSdir=`find TrigMenuRulebook/scripts -name "prescales_*" -type d`
echo "PSdir: "${PSdir}
rm $PSdir/Set_*.xml
rm $PSdir/*Emittance*
rm $PSdir/*Standby*
ls $PSdir

# upload PS keys

cmd="/afs/cern.ch/user/a/attrgcnf/public/TriggerTool/cmake/run_TriggerTool_MenuExperts.sh --dbConn $DBConn -psup $PSdir -smk $smk -w_n 50 -w_t 60"
echo $cmd " &> uploadPSK_prescaled.log"
eval $cmd &> uploadPSK_prescaled.log
hltpsk2=`grep 'INFO: HLT Prescale set saved with id' uploadPSK_prescaled.log | sed 's#.*: \([0-9]*\)\.#\1#'`
l1psk2=`grep 'INFO: Prescale set saved with id' uploadPSK_prescaled.log | sed 's#.*: \([0-9]*\)\.#\1#'`
if [ -z "$hltpsk2" ] || [ -z "$l1psk2" ]; then
    echo "ERROR Upload of prescale key failed"
    echo 'In ./uploadPSK_prescaled.log:'
    grep "Can't obtain write lock" uploadPSK_prescaled.log
    grep "SEVERE" uploadPSK_prescaled.log
    rm -rf TrigMenuRulebook
    exit 1
fi

echo "smk=${smk}" > prescaleKeys_9000.txt
echo "l1psk=${l1psk2}" >> prescaleKeys_9000.txt
echo "hltpsk=${hltpsk2}" >> prescaleKeys_9000.txt
echo "echo 'setting these keys: SMK ' \$smk ', L1 PSK ' \$l1psk ', HLT PSK ' \$hltpsk  " >> prescaleKeys_9000.txt
echo "Successfully uploaded prescales: l1psk=${l1psk2} and hltpsk=${hltpsk2}"

rm -rf TrigMenuRulebook

exit 0


