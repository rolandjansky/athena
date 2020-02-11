#!/bin/bash
# art-description: Generation test Pythia8 Z->tautau 
# art-type: build
# art-include: master/Athena
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=429708 --maxEvents=10 \
    --jobConfig=/afs/cern.ch/atlas/groups/Generators/MC15JobOptions/latest/share/DSID429xxx/MC15.429708.Pythia8EvtGen_A14NNPDF23LO_Ztautau.py  \
    --outputEVNTFile=test_Ztautau.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15


