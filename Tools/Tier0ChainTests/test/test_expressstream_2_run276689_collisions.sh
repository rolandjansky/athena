#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py  --outputHISTFile=myMergedMonitoring_express_1.root --AMI=f628 --maxEvents=25 --outputESDFile=myESD_express_1.pool.root --outputAODFile=myAOD_express_1.AOD.pool.root --outputTAGFile=myTAG_express_1.root --ignoreErrors=False --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0222._SFO-ALL._0001.1,/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_13TeV.00276689.express_express.merge.RAW._lb0223._SFO-ALL._0001.1 --imf False
echo "art-result: $?"

Reco_tf.py --inputAODFile myAOD_express_1.AOD.pool.root --outputTAGFile myTAG_express_1.root
echo "art-result: $?"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $?"

art.py compare grid --days=3 --entries 10 ${ArtPackage} ${ArtJobName}
echo "art-result: $?"
