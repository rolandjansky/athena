#!/bin/bash
# art-description: Generation test Pythia8B Jpsimumu Zmumu 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --maxEvents=100 --jobConfig=421101 \
    --outputEVNTFile=test_bb_Jpsimu4mu4X.EVNT.pool.root \

echo "art-result: $? generate"

