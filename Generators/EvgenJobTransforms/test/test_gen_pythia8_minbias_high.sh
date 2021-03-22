#!/bin/bash
# art-description: Generation test Pythia8 min_bias inelastic high
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421114 --maxEvents=100 \
    --outputEVNTFile=test_minbias_high.EVNT.pool.root \

echo "art-result: $? generate"



