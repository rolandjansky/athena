#!/bin/sh
#
# art-description: Run mc20e reco with pileup and MT = 8
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8

#Unset these to get around ATLASJT-420
unset ATHENA_CORE_NUMBER
unset ATHENA_PROC_NUMBER

Reco_tf.py --AMI=r12806 --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc16_13TeV/valid1/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3227/HITS.12560240._000287.pool.root.1 --inputRDO_BKGFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/large/mc20e_presampling.2k.latest.RDO.pool.root  --maxEvents=500 --imf False --conditionsTag="OFLCOND-MC16-SDR-RUN2-09"


rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees
  rc2=$?
fi
echo  "art-result: ${rc2} Diff"


