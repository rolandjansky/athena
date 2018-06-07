#!/bin/sh
#
# art-description: Run test-beam simulation outside ISF, generating events on-the-fly, writing HITS
# art-type: grid

TestBeam_tf.py --outputHITSFile 'test.HITS.pool.root' --maxEvents '10' --Eta '0.35' --testBeamConfig 'tbtile' --postInclude 'PyJobTransforms/UseFrontier.py'

ArtPackage=$1
ArtJobName=$2
# TODO This is a regression test I think. We would also need to compare these files to fixed references
art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName}
