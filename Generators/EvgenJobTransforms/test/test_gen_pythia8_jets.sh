#!/bin/bash
# art-description: Generation test Pythia8 dijets 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421110 --maxEvents=100 \
    --outputEVNTFile=test_jets.EVNT.pool.root \

echo "art-result: $? generate"

