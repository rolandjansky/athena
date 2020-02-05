#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - LO merging test
# art-type: grid
# art-output: EVNT.root

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=950117

echo "art-result: $? generation"

# Run tests on the log file
env -u PYTHONPATH -u PYTHONHOME python3 /cvmfs/atlas.cern.ch/repo/sw/Generators/MCJobOptions/scripts/logParser.py -s -i log.generate

echo "art-result: $? log-check"
