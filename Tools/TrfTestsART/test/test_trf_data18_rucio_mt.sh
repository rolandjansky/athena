#!/bin/bash
#
# art-description: Reco_tf.py data18 RAWtoALL in MT mode with RUCIO input dataset processing 50 files with 3 files/job
# art-type: grid
# art-input: data18_13TeV:data18_13TeV.00357750.physics_Main.daq.RAW
# art-input-nfiles: 50
# art-input-nfilesperjob: 3
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8
# art-runon: weekdays
# art-runon: Sunday

timeout 64800 Reco_tf.py \
  --inputBSFile=${ArtInFile} \
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

echo "art-result: $? Reco_tf_data18_rucio_mt"

# Check for FPEs in the logiles
test_trf_check_fpe.sh
fpeStat=$?

echo "art-result: ${fpeStat} FPEs in logfiles"
