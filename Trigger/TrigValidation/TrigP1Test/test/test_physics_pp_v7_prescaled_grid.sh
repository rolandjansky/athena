#!/bin/bash

# art-description: Physics pp v7 menu with athenaHLT over EnhancedBias data with prescales read in from the rulebook
# art-type: grid

echo 'ART Grid test with Physics_pp_v7 PSed menu'

if ! [ -x "$(command -v athenaHLT.py)" ]; then
  echo 'Invalid - not a P1 environment.' >&2
  exit 0
fi

ART_dir=${PWD}
echo 'ART_dir: '${ART_dir}

MENU='Physics_pp_v7'
echo 'Menu:' ${MENU}

export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source $ATLAS_LOCAL_ROOT_BASE/packageSetups/localSetup.sh git
#TODO: at the moment working on RB master
git clone https://:@gitlab.cern.ch:8443/atlas-trigger-menu/TrigMenuRulebook.git
#git clone /afs/cern.ch/user/t/tamartin/public/TrigMenuRulebook/
RB_dir=${PWD}/TrigMenuRulebook
echo 'RB_dir: '${RB_dir}


echo 'getting L1 and L1Topo xml files'
#get the L1 file 
get_files -xmls -copy LVL1config_${MENU}.xml
l1menu=`find .  -name LVL1config_${MENU}.xml` 
#get the L1 Topo configuration
get_files -xmls -copy L1Topoconfig_${MENU}.xml 
l1topo=`find . -name L1Topoconfig_${MENU}.xml` 

# generate only hlt xml for RB without DB access
echo "Generating menu"
athena.py  -c "TriggerMenuSetup='${MENU}'" TriggerMenuXML/runHLT_forXMLgeneration.py
hltmenu=`find . -name outputHLTconfig.xml`

echo 'l1menu: '${l1menu}
echo 'l1topo: '${l1topo}
echo 'hltmenu: '${hltmenu}

cd ${RB_dir}/scripts

rm -f l1.xml hlt.xml
ln -s ${ART_dir}/${l1menu}   l1.xml
ln -s ${ART_dir}/${hltmenu}   hlt.xml

#TODO: configure RB properly, which lumi point?
sed -i -e 's/ignoreErrors = False/ignoreErrors = True/g' runOptions.py
./runRuleBook.py 20000

cd ${ART_dir}

hltPS=`find  TrigMenuRulebook/scripts -name "RuleBook_HLTPS_${MENU}_*.xml"`
l1PS=`find  TrigMenuRulebook/scripts -name "RuleBook_LV1PS_${MENU}_*.xml"`

echo "hltPS: "${hltPS}
echo "l1PS: "${l1PS}

#TODO: what to do with missing chains?
python ${RB_dir}/scripts/apply_PS.py ${l1menu} ${l1PS} PSed_LVL1_${MENU}.xml
python ${RB_dir}/scripts/apply_PS.py ${hltmenu} ${hltPS} PSed_HLT_${MENU}.xml

athenaHLT.py -f '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1' -c 'setMenu="'${MENU}'";setLVL1XML="PSed_LVL1_'${MENU}'.xml"; setHLTXML="PSed_HLT_'${MENU}'.xml";rerunLVL1=True;enableCostD3PD=True;enableCostForCAF=True' TriggerRelease/runHLT_standalone.py
export ATH_EXIT=$?

chainDump.py -S --rootFile=expert-monitoring.root

return $ATH_EXIT