#!/bin/bash
# art-description: Generation test Pythia8 with two jet filters QCDTruthJetJZ2 with R=0.6 and MultiBJetFilter with R=0.4 
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421126 --maxEvents=100 \
    --outputEVNTFile=test_Py8JZ2MultiBJetFilt.EVNT.pool.root \

echo "art-result: $? generate"


