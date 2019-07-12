#!/bin/bash
# art-description: Generation test H7 Wjets
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --jobConfig=421104 --maxEvents=10 \
    --outputEVNTFile=test_herwig7_wjets_inelastic.EVNT.pool.root \

echo "art-result: $? generate"

