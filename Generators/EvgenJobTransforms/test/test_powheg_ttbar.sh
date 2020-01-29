#!/bin/bash
# art-description: Generation test Powheg+Pythia8 ttbar 
# art-include: 21.6/AthGeneration
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=410006 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID410xxx/MC15.410006.PowhegPythia8EvtGen_A14_ttbar_hdamp172p5_nonallhad.py  \
    --outputEVNTFile=test_powheg_ttbar.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15
    


