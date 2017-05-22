#!/bin/sh
set -e

Reco_tf.py  --outputHISTFile=myMergedMonitoring_CosmicCalo_0.root --maxEvents=500 --outputESDFile=myESD_CosmicCalo_0.pool.root --outputAODFile=myAOD_CosmicCalo_0.AOD.pool.root --outputTAGFile=myTAG_CosmicCalo_0.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_comm.00183904.physics_CosmicCalo.merge.RAW._lb0006._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_comm.00183904.physics_CosmicCalo.merge.RAW._lb0007._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data11_comm.00183904.physics_CosmicCalo.merge.RAW._lb0008._SFO-10._0001.1 --preExec='rec.doTrigger=False;'

