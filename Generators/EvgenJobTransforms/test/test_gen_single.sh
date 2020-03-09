#!/bin/bash
# art-description: MC Generators test Particel Gun single particle production
# art-type: build
# art-include: mster/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=8000 --jobConfig=421119 --maxEvents=10 \
    --outputEVNTFile=test_single_nu.EVNT.pool.root \

echo "art-result: $? generate"





