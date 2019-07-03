#!/bin/bash

# art-description: Trigger test on Run 1 Bytestream data
# art-type: build
# art-include: 21.1/AthenaP1
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: master/Athena
# Skipping art-output which has no effect for build tests.
# If you create a grid version, check art-output in existing grid tests.

export NAME="run1_bstoesdaod_build"
export COST_MONITORING="False"
export TEST="TrigAnalysisTest"
export EVENTS="5"
export JOB_LOG="athena.log"
export RECO_LOG="log.RAWtoESD"

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

N_CONTAINERS=$(grep -o HLT_xAOD__ ${RECO_LOG} | wc -l)
if [ $N_CONTAINERS -gt 0 ]; then 
  echo "xAOD Container Check: ${N_CONTAINERS} xAOD HLT containers found. OK."; 
  echo "art-result: xAODContainers 0"
else 
  echo "ERROR no converted HLT xAOD containers found in the output root file. Please check whether the conversion was scheduled"; 
  echo "art-result: xAODContainers 1"
fi

source exec_art_triggertest_post.sh
