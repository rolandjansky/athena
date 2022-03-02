#!/bin/bash
# art-description: Generation test StarlightPy8Tauola, gamma-gamma-> tau tau
# art-type: build
# art-include: master/AthGeneration
# art-include: master--HepMC3/Athena
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=5020 --jobConfig=860100 --maxEvents=10 \
    --outputEVNTFile=test_starlight_gammagamma2tautau.EVNT.pool.root \

echo "art-result: $? generate"




