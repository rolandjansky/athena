#!/bin/sh
#
# art-description: Athena runs topoclustering from an ESD file
# art-type: grid
# art-include: master/Athena

art.py createpoolfile

athena --threads=1 RecExRecoTest/RecExRecoTest_ART_egamma_fromESD.py
echo "art-result: $?"
