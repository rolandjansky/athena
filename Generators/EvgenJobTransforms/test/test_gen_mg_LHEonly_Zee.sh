#!/bin/bash
# art-description: Generation test MadGraph LHE-only for Z->ee
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421005 \
    --outputTXTFile=test_mg_Zee.TXT.tar.gz \

echo "art-result: $? generate"


