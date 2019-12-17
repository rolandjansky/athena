#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test
# art-type: grid

set -e
 
mkdir -p tests/test_04_mc.aMC_ttbar_BaseFragmentGridpackTest
cd tests/test_04_mc.aMC_ttbar_BaseFragmentGridpackTest
mkdir run_makeGridpack
cd run_makeGridpack
Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=../../../testJOs/test_04_mc.aMC_ttbar_BaseFragmentGridpackTest/
          
cd ../
mkdir run_generateFromGridpack
cd run_generateFromGridpack
Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=../../../testJOs/test_04_mc.aMC_ttbar_BaseFragmentGridpackTest/ --inputGenConfFile=../run_makeGridpack/run_01_gridpack.tar.gz 

echo "art-result: $?"
