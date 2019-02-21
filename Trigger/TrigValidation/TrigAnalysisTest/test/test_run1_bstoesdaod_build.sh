#!/bin/bash

# art-description: Trigger test on Run 1 Bytestream data
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.1/AthenaP1
# art-include: 21.1-dev/AthenaP1
# art-output: *check*
# art-output: HLTChain.txt
# art-output: HLTTE.txt
# art-output: L1AV.txt
# art-output: HLTconfig*.xml
# art-output: L1Topoconfig*.xml
# art-output: LVL1config*.xml
# art-output: *.log
# art-output: *.root
# art-output: ntuple.pmon.gz
# art-output: *perfmon*
# art-output: TotalEventsProcessed.txt
# art-output: AOD.pool.root.checkFile0
# art-output: AOD.pool.root.checkFiletrigSize.txt
# art-output: *.regtest.new

export NAME="run1_bstoesdaod_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export EVENTS="5"
export JOB_LOG="athena.log"

Reco_tf.py --maxEvents $EVENTS \
--AMITag 'q222' \
--autoConfiguration='everything' \
--conditionsTag 'all:COMCOND-BLKPA-RUN1-07' \
--preExec \
'all:DQMonFlags.doCTPMon=False;DQMonFlags.doLVL1CaloMon=False;DQMonFlags.doHLTMon=False;' \
'RAWtoESD:TriggerFlags.doMergedHLTResult=False' \
--inputBSFile='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigAnalysisTest/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1' \
--outputESDFile 'ESD.pool.root' \
--outputAODFile 'AOD.pool.root' \
--outputHISTFile 'HIST.root' &> ${JOB_LOG}

N_CONTAINERS=$(grep -o HLT_xAOD__ ${JOB_LOG} | wc -l)
if [ $N_CONTAINERS -gt 0 ]; then 
  echo "xAOD Container Check: ${N_CONTAINERS} xAOD HLT containers found. OK."; 
  echo "art-result: xAODContainers 0"
else 
  echo "ERROR no converted HLT xAOD containers found in the output root file. Please check whether the conversion was scheduled"; 
  echo "art-result: xAODContainers 1"
fi

source exec_art_triggertest_post.sh
