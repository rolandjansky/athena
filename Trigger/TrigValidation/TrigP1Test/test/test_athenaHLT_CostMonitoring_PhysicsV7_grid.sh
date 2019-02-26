#!/bin/bash

# art-description: athenaHLT on recent EnhancedBias with cost monitoring enabled
# art-type: grid
# art-include: 21.3/Athena
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/AthenaP1
# art-include: 21.0-TrigMC/AthenaP1
# art-include: master/AthenaP1
# art-include: master/Athena
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
# art-output: *.regtest.new
# art-output: *stdout*

export NAME="athenaHLT_CostMonitoring_PhysicsV7_grid"
export TESTOPTION="setMenu='Physics_pp_v7_unprescaled_only_physics_prescale';setLVL1XML='TriggerMenuXML/LVL1config_Physics_pp_v7.xml';setL1TopoXML=setLVL1XML.replace('/LVL1','/L1Topo');rerunLVL1=True;enableCostD3PD=True;enableCostForCAF=True;HLTOutputLevel=INFO;doValidation=True;"
export FILE="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00339070.physics_EnhancedBias.merge.RAW._lb0101._SFO-1._0001.1"
export JOBOPTION="TriggerRelease/runHLT_standalone.py"
export EVENTS="10000"

source exec_athenaHLT_art_trigger_validation.sh
source exec_art_trigp1test_post.sh



