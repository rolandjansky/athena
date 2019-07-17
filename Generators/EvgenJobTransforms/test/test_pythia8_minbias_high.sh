#!/bin/bash
# art-description: Generation test Pythia8 min_bias inelastic high
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=369035 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID369xxx/MC15.369035.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic_high.py \
    --outputEVNTFile=test_minbias_high.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15



