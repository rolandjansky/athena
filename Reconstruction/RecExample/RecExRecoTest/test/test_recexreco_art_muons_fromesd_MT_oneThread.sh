#!/bin/sh
#
# art-description: Athena runs topoclustering from an ESD file
# art-type: grid
# art-include: master/Athena

athena --threads=1 RecExRecoTest/RecExRecoTest_ART_muons_fromESD.py
echo "art-result: $?"
