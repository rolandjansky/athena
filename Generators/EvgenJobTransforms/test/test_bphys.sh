#!/bin/bash
# art-description: Generation test Pythia8B Jpsimumu Zmumu 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=429705 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID429xxx/MC15.429705.Pythia8B_A14NNPDF23LO_pp_JpsimumuZmumu.py \
    --outputEVNTFile=test_bb_Jpsimu4mu4X.EVNT.pool.root \

echo "art-result: $? generate"

rm -fr _joproxy15
