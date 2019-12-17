#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test
# art-type: grid

mkdir -p tests/test_09_mc.MG_ttbar_gridpackTest
cd tests/test_09_mc.MG_ttbar_gridpackTest
mkdir run_makeGridpack
cd run_makeGridpack
Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=../../../testJOs/test_09_mc.MG_ttbar_gridpackTest/

set -e
          
cd ../
mkdir run_generateFromGridpack
cd run_generateFromGridpack
Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=../../../testJOs/test_09_mc.MG_ttbar_gridpackTest/ --inputGenConfFile=../run_makeGridpack/run_01_gridpack.tar.gz 

echo "art-result: $?"
