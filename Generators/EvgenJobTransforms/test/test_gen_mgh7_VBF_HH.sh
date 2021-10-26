#!/bin/bash
# art-description: Generation test MG+Py8 VBF-only HH-> bb ZZ (ZZ-> 2l 2q)
# art-include: master/AthGeneration
# art-include: master--HepMC3/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421436 --maxEvents=10 \
    --outputEVNTFile=test_mgh7_VBFHH.EVNT.pool.root \

echo "art-result: $? generate"


