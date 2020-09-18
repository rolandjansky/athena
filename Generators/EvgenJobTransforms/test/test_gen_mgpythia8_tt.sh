#!/bin/bash
# art-description: Generation test MG+Py8 tt 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421007 --maxEvents=10 \
    --outputEVNTFile=test_mgpythia8_tt.EVNT.pool.root \

echo "art-result: $? generate"


