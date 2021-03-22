#!/bin/bash
# art-description: Generation test Starlight
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=5020 --jobConfig=421120 --maxEvents=100 \
    --outputEVNTFile=test_starlight_gammagamma2ee.EVNT.pool.root \

echo "art-result: $? generate"




