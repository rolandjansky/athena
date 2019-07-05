#!/bin/bash
# art-description: Generation test Epos min_bias 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=361224 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/tests/MC15.361224.Epos_minbias_inelastic.py  \
    --outputEVNTFile=test_epos_minbias_inelastic.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15


