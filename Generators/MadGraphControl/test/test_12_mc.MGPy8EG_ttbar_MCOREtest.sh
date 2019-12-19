#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - Multicore LO
# art-type: grid
# art-athena-mt: 8

set -e

export ATHENA_PROC_NUMBER=8
unset ATHENA_CORE_NUMBER # This is not an MT job!

mkdir 999999
get_files -jo mc.MGPy8EG_ttbar_MCOREtest.py
mv mc.*py 999999/

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --runNumber=999999 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=./999999

echo "art-result: $?"
