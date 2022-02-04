#!/bin/bash
# art-description: Generation test Hijing pp at 5020 GeV, nch>40
# art-include: master/AthGeneration
# art-include: master--HepMC3/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=5020 --jobConfig=421446 --maxEvents=10 \
    --outputEVNTFile=test_hijing_nch40.EVNT.pool.root \

echo "art-result: $? generate"


