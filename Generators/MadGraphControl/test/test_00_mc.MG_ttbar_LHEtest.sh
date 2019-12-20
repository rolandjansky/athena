#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - LO LHE generation
# art-type: grid
# art-output: test_lhe_events.events

set -e

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=421400 --firstEvent=1 --randomSeed=123456 --outputTXTFile=test_lhe_events --jobConfig=421400

echo "art-result: $?"
