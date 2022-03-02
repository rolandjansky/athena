#!/bin/sh
#
# art-description: Run reco using mc20e (overlay+trigger+rco)
# art-output: log.*
# art-athena-mt: 8
# art-type: grid
# art-include: master/Athena

Reco_tf.py --AMI=r13145 --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3681/HITS.25836812._004813.pool.root.1 --inputRDO_BKGFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/large/mc20_13TeV.900149.PG_single_nu_Pt50.digit.RDO.e8307_s3482_s3136_d1715/RDO.26811908._031801.pool.root.1 --maxEvents=500 --imf False --asetup "RDOtoRDOTrigger:Athena,21.0,latest"

RES=$?
echo "art-result: $RES Reco"
