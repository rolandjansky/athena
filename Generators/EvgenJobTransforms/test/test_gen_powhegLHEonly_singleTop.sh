#!/bin/bash
# art-description: Generation test Powheg LHE-only single top s-channel
# art-type: build
# art-include: master/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421358 \
    --outputTXTFile=test_powheg_t.TXT.tar.gz \

echo "art-result: $? generate"


