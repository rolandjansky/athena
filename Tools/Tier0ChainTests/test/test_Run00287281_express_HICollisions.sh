#!/bin/sh
#
# art-description: RecoTrf on 2015 PbPb express stream (Run287281 LB124)
# art-type: grid

Reco_tf.py --outputHISTFile=myMergedMonitoring_0.root --outputESDFile=myESD_0.pool.root --outputAODFile=myAOD_0.AOD.pool.root --outputTAGFile=myTAG_0.root --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data15_hi.00287281.express_express.merge.RAW._lb0124._SFO-ALL._0001.1 --AMI=q314 --maxEvents=25 --imf False
echo "art-result: $? Reco"

ArtPackage=$1
ArtJobName=$2
art.py compare grid --entries 10 --file=*AOD*.pool.root ${ArtPackage} ${ArtJobName}
echo "art-result: $? Diff"

