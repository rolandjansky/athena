#!/bin/bash
# art-description: Generation test PowhegPythia8Tauolapp Z 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --jobConfig=421112 --maxEvents=10 \
    --outputEVNTFile=test_TauolaPP_evtgen.EVNT.pool.root \

echo "art-result: $? generate"




