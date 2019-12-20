#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - NLO gridpack
# art-type: grid

set -e
 
mkdir run_makeGridpack
cd run_makeGridpack

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=421404 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=421404

cd ../
mkdir run_generateFromGridpack
cd run_generateFromGridpack

mkdir 999999
get_files -jo mc.aMC_ttbar_BaseFragmentGridpackTest.py
mv mc.*py 999999/

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=421404 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=421404 --inputGenConfFile=../run_makeGridpack/run_01_gridpack.tar.gz 

echo "art-result: $?"
