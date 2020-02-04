#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - NLO gridpack
# art-type: grid
# art-output: test_lhe_events.events
# art-output: output_hists.root

set -e
 
mkdir run_makeGridpack
cd run_makeGridpack

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=950104 --outputFileValidation=False

echo "art-result: $? gridpack_creation"

cd ../
mkdir run_generateFromGridpack
cd run_generateFromGridpack

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputTXTFile=test_lhe_events --jobConfig=950104 --inputGenConfFile=../run_makeGridpack/run_01_gridpack.tar.gz

echo "art-result: $?"

simple_lhe_plotter.py test_lhe_events.events
cp output_hists.root test_lhe_events.events ../
