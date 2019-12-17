#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test
# art-type: grid

set -e

mkdir -p tests/test_17_mc.MGPy8EG_A14NNPDF23_ttbar_Merge_Np01
cd tests/test_17_mc.MGPy8EG_A14NNPDF23_ttbar_Merge_Np01
Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=../../testJOs/test_17_mc.MGPy8EG_A14NNPDF23_ttbar_Merge_Np01

echo "art-result: $?"
