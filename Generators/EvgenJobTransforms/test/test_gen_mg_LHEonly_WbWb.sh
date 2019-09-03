#!/bin/bash
# art-description: Generation test MadGraph LHE-only for W+bW-b
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Gen_tf_txt.py --ecmEnergy=13000 --jobConfig=421124 \
    --outputTXTFile=test_mg_WbWb.TXT \

echo "art-result: $? generate"


