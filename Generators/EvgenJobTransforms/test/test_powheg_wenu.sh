#!/bin/bash
# art-description: Generation test Powheg+Pythia8 W->enu 
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Generate_tf.py --ecmEnergy=13000 --runNumber=361100 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID361xxx/MC15.361100.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Wplusenu.py  \
    --outputEVNTFile=test_powheg_wenu_inelastic.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15




