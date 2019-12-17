#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test
# art-type: grid

set -e
 
mkdir -p tests/test_07_mc.MG_ttbar_MadSpinTest
cd tests/test_07_mc.MG_ttbar_MadSpinTest
Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=../../testJOs/test_07_mc.MG_ttbar_MadSpinTest

echo "art-result: $?"
