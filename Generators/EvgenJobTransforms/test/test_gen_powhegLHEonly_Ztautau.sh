#!/bin/bash
# art-description: Generation test Powheg LHE-only ZZ
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421354 \
    --outputTXTFile=test_powheg_ZZ.TXT.tar.gz \

echo "art-result: $? generate"


