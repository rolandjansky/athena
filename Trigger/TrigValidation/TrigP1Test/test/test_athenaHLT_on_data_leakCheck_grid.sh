#!/bin/bash

# art-description: athenaHLT on DATA all slices leakCheck 
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
# art-output: LeakCheck.txt

export NAME="athenaHLT_on_data_leakCheck_grid"
export JOB_LOG="${NAME}.log"

export LD_PRELOAD=/cvmfs/sft.cern.ch/lcg/releases/libunwind/5c2cade-76996/$CMTCONFIG/lib/libunwind.so

athenaHLT.py --stdcmalloc --leak-check-execute  -o HLT_physicsV7 -n 100 -f "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00339070.physics_EnhancedBias.merge.RAW._lb0101._SFO-1._0001.1" -c "setMenu='Physics_pp_v7_tight_physics_prescale';setLVL1XML='TriggerMenuXML/LVL1config_Physics_pp_v7.xml';setL1TopoXML=setLVL1XML.replace('/LVL1','/L1Topo');HLTOutputLevel=WARNING;doValidation=True;rerunLVL1=True;" TrigP1Test/runHLT_standaloneRTT_leakCheck.py &> ${JOB_LOG}

ATH_RETURN=${PIPESTATUS[0]}
echo "art-result: ${ATH_RETURN} athena"

source exec_art_trigp1test_post.sh
