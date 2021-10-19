#!/bin/bash
# art-description: Generation test H7 Zjets
# art-include: master/AthGeneration
# art-include: master--HepMC3/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421105 --maxEvents=10 \
    --outputEVNTFile=test_herwig7_zjets_inelastic.EVNT.pool.root \

echo "art-result: $? generate"



