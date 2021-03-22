#!/bin/bash
# art-description: Generation test Powheg LHE-only bb 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421353 --maxEvents=10 \
    --outputEVNTFile=test_powheg_bb.EVNT.pool.root \

echo "art-result: $? generate"
    


