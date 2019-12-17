#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test
# art-type: grid

set -e

mkdir -p tests/test_15_mc.aMCPy8EG_A14NNPDF23_ttbar_Incl
cd tests/test_15_mc.aMCPy8EG_A14NNPDF23_ttbar_Incl
Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=../../testJOs/test_15_mc.aMCPy8EG_A14NNPDF23_ttbar_Incl

echo "art-result: $?"
