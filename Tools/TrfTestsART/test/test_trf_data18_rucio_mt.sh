#!/bin/bash
#
# art-description: Reco_tf.py q431 RAWtoALL in MT mode with RUCIO input dataset processing 50 files with 3 files/job
# art-type: grid
# art-input: data18_13TeV:data18_13TeV.00357750.physics_Main.daq.RAW
# art-input-nfiles: 50
# art-input-nfilesperjob: 3
# art-include: master/Athena
# art-athena-mt: 8

timeout 43200 Reco_tf.py \
  --inputBSFile=${ArtInFile} \
  --outputAODFile=myAOD.pool.root \
  --outputHISTFile=myHIST.root \
  --outputDESDM_MCPFile=myDESDM_MCP.pool.root \
  --outputDRAW_ZMUMUFile=myDRAW_ZMUMU.data \
  --multithreaded='True' \
  --preExec 'all:from AthenaMonitoring.DQMonFlags import DQMonFlags; DQMonFlags.doHLTMon=False' \
  --postExec 'FPEAuditor.NStacktracesOnFPE=10' \
  --autoConfiguration='everything' \
  --conditionsTag 'all:CONDBR2-BLKPA-RUN2-03' --geometryVersion='default:ATLAS-R2-2016-01-00-01' \
  --runNumber='357750' --steering='doRAWtoALL' --maxEvents='-1'

echo "art-result: $? Reco_tf_data18_rucio_mt"
