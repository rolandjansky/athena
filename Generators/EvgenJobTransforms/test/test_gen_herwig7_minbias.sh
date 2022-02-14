#!/bin/bash
# art-description: Generation test H7 min_bias 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --maxEvents=100 --jobConfig=950297 \
    --outputEVNTFile=test_herwig7_minbias_inelastic.EVNT.pool.root \

echo "art-result: $? generate"

   



