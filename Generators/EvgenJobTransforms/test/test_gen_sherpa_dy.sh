#!/bin/bash
# art-description: Generation test Sherpa DY without inputs 
# art-type: build
# art-include: master/AthGeneration
# art-include: master--HepMC3/Athena
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421001 --maxEvents=10 \
    --outputEVNTFile=test_sherpa_dy_inelastic.EVNT.pool.root \

echo "art-result: $? generate"



