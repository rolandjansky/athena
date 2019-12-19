#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - LO Gridpack
# art-type: grid

set -e

mkdir run_makeGridpack
cd run_makeGridpack

mkdir 999999
get_files -jo mc.MG_ttbar_gridpackTest.py
mv mc.*py 999999/

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=./999999

cd ../
mkdir run_generateFromGridpack
cd run_generateFromGridpack

mkdir 999999
get_files -jo mc.MG_ttbar_gridpackTest.py
mv mc.*py 999999/

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=./999999 --inputGenConfFile=../run_makeGridpack/run_01_gridpack.tar.gz 

echo "art-result: $?"
