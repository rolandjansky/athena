#!/bin/bash
# art-description: Generation test PowhegPythia Wt (inclusive top) 
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm -f *.dat;
Generate_tf.py --ecmEnergy=13000 --runNumber=410013 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID410xxx/MC15.410013.PowhegPythiaEvtGen_P2012_Wt_inclusive_top.py  --steering=afterburn \
    --outputEVNTFile=test_powheg_wt_inelastic.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15




