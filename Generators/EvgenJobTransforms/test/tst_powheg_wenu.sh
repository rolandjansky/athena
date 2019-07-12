#!/bin/bash
# art-description: Generation test Powheg+Pythia8 W->enu 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Generate_tf.py --ecmEnergy=13000 --jobConfig=421116 --maxEvents=10 \
    --outputEVNTFile=test_powheg_wenu_inelastic.EVNT.pool.root \

echo "art-result: $? generate"




