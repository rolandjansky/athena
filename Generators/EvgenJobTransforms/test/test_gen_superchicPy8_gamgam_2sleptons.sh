#!/bin/bash
# art-description: Generation test SuperChic4+Py8 gamma+gamma to 2 sleptons
# art-include: master/AthGeneration
# art-include: master--HepMC3/Athena
# art-include: 22.0/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421453 --maxEvents=10 \
    --outputEVNTFile=test_superchic.EVNT.pool.root \

echo "art-result: $? generate"

