#!/bin/bash
#
# art-description: Reco_tf.py data16 RAWtoALL in MT mode
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8

timeout 64800 Reco_tf.py \
  --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00304008.physics_Main.daq.RAW._lb0838._SFO-6._0001.data \
  --outputAODFile=myAOD.pool.root \
  --outputHISTFile=myHIST.root \
  --outputDESDM_MCPFile=myDESDM_MCP.pool.root \
  --outputDRAW_ZMUMUFile=myDRAW_ZMUMU.data \
  --outputDAOD_IDTIDEFile=myIDTIDE.pool.root \
  --multithreaded='True' \
  --preExec 'all:from AthenaMonitoring.DQMonFlags import DQMonFlags; DQMonFlags.doHLTMon=False' \
  --postExec 'FPEAuditor.NStacktracesOnFPE=10' \
  --autoConfiguration='everything' \
  --conditionsTag 'all:CONDBR2-BLKPA-RUN2-09' --geometryVersion='default:ATLAS-R2-2016-01-00-01' \
  --runNumber='357750' --steering='doRAWtoALL' --maxEvents='-1'

rc1=$?
echo "art-result: ${rc1} Reco_tf_data16_mt"

echo "==================Checking for FPEs"
grep --quiet "WARNING FPE" log.*
fpeStat=$?
# Let's flip this - finding a FPE is a failure and it'd be confusing to check for 0
if [[ "$fpeStat" == "0" ]]; then
  fpeStat="1"
else
  fpeStat="0"
fi

if [[ "$fpeStat" != "0" ]]; then
  echo "Found FPEs! FAILING the test. FPEs reported below." `date`
  for file in `ls log.*`;
    do
      echo "=====" $file;
      grep "WARNING FPE" $file | awk '{print $11}' | sed 's/\[//' | sed 's/\]//' | sed -r '/^\s*$/d' | sort  | uniq -c
    done
fi

echo "art-result: ${fpeStat} FPEs in logfiles"
