#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - NLO LHE generation
# art-type: grid
# art-output: test_lhe_events.events

set -e

mkdir 999999
get_files -jo mc.aMC_ttbar_LHEtest.py
mv mc.*py 999999/

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputTXTFile=test_lhe_events --jobConfig=./999999

echo "art-result: $?"
