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

source /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/prescaleKeys.txt
#for testing:
#source /eos/atlas/atlascerngroupdisk/data-art/grid-input/TrigP1Test/exportMenuKeys.sh
#smk=5218
#l1psk=120
#hltpsk=420
echo 'smk' ${smk}
echo 'l1psk' ${l1psk}
echo 'hltpsk' ${hltpsk}

echo "Reading release from SMK"
python releaseFromSMK.py TRIGGERDBART ${smk} &> releaseFromSMK.log
eval "$( grep 'export release=' releaseFromSMK.log)" 
if [ -z ${release} ]; then
   echo "Release not found"
else
   asetup AthenaP1,21.1,${release}
fi

#athenaHLT.py -M -f /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_eb_zmm_egz.merged.RAW.selected._0001.data -o HLT_physicsV7_prescaled -J TrigConf::HLTJobOptionsSvc --use-database --db-type "Coral" --db-server "TRIGGERDBATN" --db-smkey ${smk} --db-hltpskey ${hltpsk} --db-extra "{'lvl1key': ${l1psk}}" -c 'rerunLVL1=True;enableCostD3PD=True;enableCostForCAF=True'

athenaHLT.py -n 10000 -f /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00339070.physics_EnhancedBias.merge.RAW._lb0101._SFO-1._0001.1 -o HLT_physicsV7_prescaled -J TrigConf::HLTJobOptionsSvc --use-database --db-type "Coral" --db-server "TRIGGERDBART" --db-smkey ${smk} --db-hltpskey ${hltpsk} --db-extra "{'lvl1key': ${l1psk}}" | tee ${JOB_LOG} 

Trig_reco_tf.py --inputBSFile=HLT_physicsV7_prescaled._0001.data --outputNTUP_TRIGCOST=trig_cost.root

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} ${NAME}"

exec_art_trigp1test_post.sh


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
