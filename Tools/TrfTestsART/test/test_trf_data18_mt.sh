#!/bin/bash
#
# art-description: Reco_tf.py q431 RAWtoALL in MT mode
# art-type: grid
# art-include: master/Athena
# art-athena-mt: 8

export ATHENA_CORE_NUMBER=8
Reco_tf.py --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data18_13TeV.00357750.physics_Main.daq.RAW/data18_13TeV.00357750.physics_Main.daq.RAW._lb0114._SFO-5._0003.data  --outputAODFile=aod.pool.root --outputHISTFile=hist.root --multithreaded='True' --preExec 'all:from AthenaMonitoring.DQMonFlags import DQMonFlags; DQMonFlags.doMonitoring=True; DQMonFlags.doNewMonitoring=True; DQMonFlags.doHLTMon=False' --postExec 'FPEAuditor.NStacktracesOnFPE=10' --autoConfiguration='everything' --conditionsTag 'all:CONDBR2-BLKPA-RUN2-03' --geometryVersion='default:ATLAS-R2-2016-01-00-01' --runNumber='357750' --steering='doRAWtoALL' --maxEvents='-1'

echo "art-result: $? Reco_tf_data18_mt"
