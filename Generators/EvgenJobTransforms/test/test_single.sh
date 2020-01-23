#!/bin/bash
# art-description: MC Generators test single particle production
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=8000 --runNumber=999999 --maxEvents=10 \
    --jobConfig=/afs/cern.ch/atlas/groups/Generators/MC14JobOptions/latest/share/tests/MC14.999999.ParticleGun_single_nu.py \
    --outputEVNTFile=test_single_nu.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy14





