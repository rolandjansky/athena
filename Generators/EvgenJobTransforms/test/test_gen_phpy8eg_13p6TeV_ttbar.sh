#!/bin/bash
# art-description: Generation test Ph+Py8 ttbar incl. with ECM=13.6TeV
# art-include: master/AthGeneration
# art-include: master--HepMC3/Athena
# art-include: 22.0/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --jobConfig=950070 --maxEvents=10 \
    --outputEVNTFile=test_powheg_ttbar.EVNT.pool.root --ecmEnergy=13600 \

echo "art-result: $? generate"
    


