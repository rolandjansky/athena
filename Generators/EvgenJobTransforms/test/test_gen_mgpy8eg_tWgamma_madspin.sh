#!/bin/bash
# art-description: Generation test MG+Py8+EG pp->ttbar+W+a with MadSpin
# art-type: build
# art-include: 21.6/master
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421006 --maxEvents=100 \
    --outputEVNTFile=test_mgpy8eg_tWgamma_madspin.EVNT.pool.root \

echo "art-result: $? generate"


