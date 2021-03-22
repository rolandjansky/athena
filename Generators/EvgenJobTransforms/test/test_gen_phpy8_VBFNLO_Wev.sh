#!/bin/bash
# art-description: Generation test Powheg+Py8 VBFNLO W+->e+v 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421356 --maxEvents=100 \
    --outputEVNTFile=test_phpy8_VBF_W.EVNT.pool.root \

echo "art-result: $? generate"
    


