#!/bin/bash
# art-description: Generation test Powheg+Pythia8 Wt_DR 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=600293 --maxEvents=100 \
    --outputEVNTFile=test_powheg_Wt_DR.EVNT.pool.root \

echo "art-result: $? generate"
    


