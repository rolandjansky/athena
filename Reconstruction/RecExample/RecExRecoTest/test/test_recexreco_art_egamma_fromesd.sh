#!/bin/sh
#
# art-description: Athena runs topoclustering from an ESD file
# art-type: grid
athena  RecExRecoTest/RecExRecoTest_ART_egamma_fromESD.py
echo "art-result: $?"
