#!/bin/bash
# art-description: Generation test PowhegPythia8Tauolapp Z 
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=429696 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID429xxx/MC15.429696.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Ztautau.py \
    --outputEVNTFile=test_JZ0_evtgen.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15




