#!/bin/bash
# art-description: Generation test MG+Py8 ttW 
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Generate_tf.py --ecmEnergy=13000 --runNumber=410066 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID410xxx/MC15.410066.MadGraphPythia8EvtGen_A14NNPDF23LO_ttW_Np0.py \
    --outputEVNTFile=test_mgpythia8_ttW.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15


