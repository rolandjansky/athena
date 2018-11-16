#!/bin/bash

# art-description: Physics pp v7 menu with athenaHLT over EnhancedBias data with prescales created in ART build job and read from DB. FROM NIGHTLY OF THE DAY BEFORE!
# art-type: grid
# art-include: 21.1/AthenaP1
# art-output: HLTChain.txt
# art-output: HLTTE.txt
# art-output: L1AV.txt
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: *.log
# art-output: costMonitoring_*
# art-output: *.root
# art-output: ntuple.pmon.gz
# art-output: *perfmon*
# art-output: *.regtest


export NAME="athenaHLT_prescaled_PhysicsV7"
export JOB_LOG="${NAME}.log"

#source /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/prescaleKeys_17000.txt | tee ${JOB_LOG} 
#for testing:
#source /eos/atlas/atlascerngroupdisk/data-art/grid-input/TrigP1Test/prescaleKeys_17000.txt | tee ${JOB_LOG}
eval "export $( grep 'smk=' /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/prescaleKeys_17000.txt)"
eval "export $( grep 'l1psk=' /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/prescaleKeys_17000.txt)"
eval "export $( grep 'hltpsk=' /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/prescaleKeys_17000.txt)"

#smk=5218
#l1psk=120
#hltpsk=420
echo 'smk' ${smk} | tee ${JOB_LOG} 
echo 'l1psk' ${l1psk} | tee ${JOB_LOG} 
echo 'hltpsk' ${hltpsk} | tee ${JOB_LOG} 

echo "Reading release from SMK" | tee ${JOB_LOG} 
get_files releaseFromSMK.py
python releaseFromSMK.py TRIGGERDBART ${smk} > releaseFromSMK.log
cat releaseFromSMK.log  | tee ${JOB_LOG}
eval "$( grep 'export release=' releaseFromSMK.log)" 
if [ -z ${release} ]; then
   echo "Release not found" | tee ${JOB_LOG} 
fi

l1psk="'lvl1key': ${l1psk}"
subshellcmd='source $AtlasSetup/scripts/asetup.sh AthenaP1,21.1,'${release}'; athenaHLT.py -n 5000 -f /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00339070.physics_EnhancedBias.merge.RAW._lb0101._SFO-1._0001.1 -o HLT_physicsV7_prescaled -J TrigConf::HLTJobOptionsSvc --use-database --db-type "Coral" --db-server "TRIGGERDBART" --db-smkey '${smk}' --db-hltpskey '${hltpsk}' --db-extra "{'${l1psk}'}"; Trig_reco_tf.py --inputBSFile=HLT_physicsV7_prescaled._0001.data --outputNTUP_TRIGCOST=trig_cost.root; RunTrigCostD3PD -f trig_cost.root --outputTagFromAthena --costMode --ratesMode --isCPUPrediction; chainDump.py -n -S  2>&1 | tee ${JOB_LOG}'
echo "running in subshell: $subshellcmd"
(eval $subshellcmd)

grep -r "RATE_" costMonitoring_*/csv/Table_Rate_Group_HLT_All.csv  | awk '{split($0,a,","); print a[1]"\t"a[3]"\t"a[4] }' >> HLTChain.txt

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

#exec_art_trigp1test_post.sh


#echo 'ART Grid test with Physics_pp_v7 PSed menu'
#
#if ! [ -x "$(command -v athenaHLT.py)" ]; then
#  echo 'Invalid - not a P1 environment.' >&2
#  exit 0
#fi
#
#ART_dir=${PWD}
#echo 'ART_dir: '${ART_dir}
#
#MENU='Physics_pp_v7'
#echo 'Menu:' ${MENU}
#
#export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
#source $ATLAS_LOCAL_ROOT_BASE/packageSetups/localSetup.sh git
##TODO: at the moment working on RB master
#git clone https://:@gitlab.cern.ch:8443/atlas-trigger-menu/TrigMenuRulebook.git
##git clone /afs/cern.ch/user/t/tamartin/public/TrigMenuRulebook/
#RB_dir=${PWD}/TrigMenuRulebook
#echo 'RB_dir: '${RB_dir}
#
#
#echo 'getting L1 and L1Topo xml files'
##get the L1 file 
#get_files -xmls -copy LVL1config_${MENU}.xml
#l1menu=`find .  -name LVL1config_${MENU}.xml` 
##get the L1 Topo configuration
#get_files -xmls -copy L1Topoconfig_${MENU}.xml 
#l1topo=`find . -name L1Topoconfig_${MENU}.xml` 
#
## generate only hlt xml for RB without DB access
#echo "Generating menu"
#athena.py  -c "TriggerMenuSetup='${MENU}'" TriggerMenuXML/runHLT_forXMLgeneration.py
#hltmenu=`find . -name outputHLTconfig.xml`
#
#echo 'l1menu: '${l1menu}
#echo 'l1topo: '${l1topo}
#echo 'hltmenu: '${hltmenu}
#
#cd ${RB_dir}/scripts
#
#rm -f l1.xml hlt.xml
#ln -s ${ART_dir}/${l1menu}   l1.xml
#ln -s ${ART_dir}/${hltmenu}   hlt.xml
#
##TODO: configure RB properly, which lumi point?
#sed -i -e 's/ignoreErrors = False/ignoreErrors = True/g' runOptions.py
#./runRuleBook.py 20000
#
#cd ${ART_dir}
#
#hltPS=`find  TrigMenuRulebook/scripts -name "RuleBook_HLTPS_${MENU}_*.xml"`
#l1PS=`find  TrigMenuRulebook/scripts -name "RuleBook_LV1PS_${MENU}_*.xml"`
#
#echo "hltPS: "${hltPS}
#echo "l1PS: "${l1PS}
#
##TODO: what to do with missing chains?
#python ${RB_dir}/scripts/apply_PS.py ${l1menu} ${l1PS} PSed_LVL1_${MENU}.xml
#python ${RB_dir}/scripts/apply_PS.py ${hltmenu} ${hltPS} PSed_HLT_${MENU}.xml
#
#athenaHLT.py -f '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1' -c 'setMenu="'${MENU}'";setLVL1XML="PSed_LVL1_'${MENU}'.xml"; setHLTXML="PSed_HLT_'${MENU}'.xml";rerunLVL1=True;enableCostD3PD=True;enableCostForCAF=True' TriggerRelease/runHLT_standalone.py
#export ATH_EXIT=$?
#
#chainDump.py -S --rootFile=expert-monitoring.root
#
#return $ATH_ExXIT
