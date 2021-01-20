#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - Multicore LO
# art-type: grid
# art-athena-mt: 8
# art-output: EVNT.root

export ATHENA_PROC_NUMBER=8
unset ATHENA_CORE_NUMBER # This is not an MT job!

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=950112
# Note 11.4.2020: This is currently crashing because the output validator expects an MP-specific file (asked VT)
echo "art-result: $? generation"

# Run tests on the log file
env -u PYTHONPATH -u PYTHONHOME python3 /cvmfs/atlas.cern.ch/repo/sw/Generators/MCJobOptions/scripts/logParser.py -s -i log.generate

echo "art-result: $? log-check"
