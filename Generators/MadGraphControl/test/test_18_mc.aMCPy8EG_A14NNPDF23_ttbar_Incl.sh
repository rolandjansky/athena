#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - NLO inclusive ttbar
# art-type: grid
# art-output: EVNT.root

set -e

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=421418

echo "art-result: $?"
