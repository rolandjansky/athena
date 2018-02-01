#!/bin/bash

# art-description: athenaHLT on DATA all slices leakCheck 
# art-type: grid
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-include: 21.0/AthenaP1
# art-include: 21.0-TrigMC/AthenaP1
# art-include: master/AthenaP1
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

export NAME="athenaHLT_on_data_leakCheck_grid"
export TESTOPTION="setMenu='Physics_pp_v7_tight_physics_prescale';setLVL1XML='TriggerMenuXML/LVL1config_Physics_pp_v7.xml';setL1TopoXML=setLVL1XML.replace('/LVL1','/L1Topo');HLTOutputLevel=WARNING;doValidation=True"
export FILE="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00339070.physics_EnhancedBias.merge.RAW._lb0101._SFO-1._0001.1"
export JOBOPTION="TrigP1Test/runHLT_standaloneRTT_leakCheck.py"
export EVENTS="1000"
export ATHENAHLTOPT="--stdcmalloc --leak-check-execute  -o HLT_physicsV7"

export LD_PRELOAD=/cvmfs/sft.cern.ch/lcg/releases/libunwind/5c2cade-76996/$CMTCONFIG/lib/libunwind.so

source exec_athenaHLT_art_trigger_validation.sh
source exec_art_trigp1test_post.sh



