#!/bin/sh
#
# art-description: Run reco using NSW setup without pileup or RDOTrigger steps.
# art-output: log.*
# art-athena-mt: 8
# art-type: grid
# art-include: master/Athena

Reco_tf.py '--AMI=r13134' '--inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/NSW/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.HITS.e4993_s3747.pool.root' '--outputRDOFile=myRDO.pool.root' '--outputAODFile=myAOD.pool.root' '--outputESDFile=myESD.pool.root'

RES=$?
echo "art-result: $RES Reco"
