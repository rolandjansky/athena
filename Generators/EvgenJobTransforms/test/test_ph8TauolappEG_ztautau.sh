#!/bin/bash
# art-description: Generation test Pythia8Tauolapp Z->tautau 
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=429697 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID429xxx/MC15.429697.Pythia8EvtGen_A14NNPDF23LO_DYtautau_120M180.py  \
    --outputEVNTFile=test_DYZtautau.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15
    





