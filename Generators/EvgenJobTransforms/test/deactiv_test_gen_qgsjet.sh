#!/bin/bash
# art-description: Generation test QGSjet  
# art-type: build
# art-include: master/AthGeneration
# art-include: master--HepMC3/Athena
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421117 --maxEvents=10 \
    --outputEVNTFile=test_qgsjet_minbias_inelastic.EVNT.pool.root \

echo "art-result: $? generate"




