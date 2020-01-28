#!/bin/bash
# art-description: Generation test Sherpa DY 
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=191919 --maxEvents=10 \
    --jobConfig=/afs/cern.ch/atlas/groups/Generators/MC14JobOptions/latest/share/tests/MC14.191919.Sherpa_NNPDF30NNLO_DrellYan.py  \
    --outputEVNTFile=test_sherpa_dy_inelastic.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy14



