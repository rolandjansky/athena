#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - LHE generation with LHAPDF
# art-type: grid
# art-output: test_lhe_events.events

set -e

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=421402 --firstEvent=1 --randomSeed=123456 --outputTXTFile=test_lhe_events --jobConfig=421402

echo "art-result: $?"
