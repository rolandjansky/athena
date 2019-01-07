#!/bin/bash
# art-description: Generation test MG+Py8 Z->mumu
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Generate_tf.py --ecmEnergy=13000 --runNumber=361505 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID361xxx/MC15.361505.MadGraphPythia8EvtGen_A14NNPDF23LO_Zmumu_Np0.py \
    --outputEVNTFile=test_mgpythia8_Zmumu.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15


