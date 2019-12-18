#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test
# art-type: grid

mkdir -p tests/test_11_mc.aMC_ttbar_gridpackClusterTest
cd tests/test_11_mc.aMC_ttbar_gridpackClusterTest
mkdir run_makeGridpackCluster
cd run_makeGridpackCluster
Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=../../../testJOs/test_11_mc.aMC_ttbar_gridpackClusterTest/

echo "art-result: $?"
