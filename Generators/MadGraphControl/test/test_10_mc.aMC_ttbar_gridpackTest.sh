#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - NLO Gridpack
# art-type: grid
# art-output: test_lhe_events.events
# art-output: output_hists.root

mkdir run_makeGridpack
cd run_makeGridpack

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=950110 --outputFileValidation=False

echo "art-result: $? gridpack_creation"

cd ../
mkdir run_generateFromGridpack
cd run_generateFromGridpack

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputTXTFile=test_lhe_events --jobConfig=950110 --inputGenConfFile=../run_makeGridpack/run_01_gridpack.tar.gz

echo "art-result: $? Gen_tf"

simple_lhe_plotter.py test_lhe_events.events

echo "art-result: $? Plot"

cp output_hists.root test_lhe_events.events ../
