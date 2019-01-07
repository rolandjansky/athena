#!/bin/bash
# art-description: Generation test Epos min_bias 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=8000 --runNumber=129080 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC14JobOptions/latest/share/tests/MC14.129080.Epos_minbias_inelastic.py  \
    --outputEVNTFile=test_epos_minbias_inelastic.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy14


