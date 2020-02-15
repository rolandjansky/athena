#!/bin/bash
# art-description: Generation test MG+Py8 tt NLO 
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Generate_tf.py --ecmEnergy=13000 --runNumber=412121 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID412xxx/MC15.412121.aMcAtNloHerwig7EvtGen_MEN30NLO_ttbar_incl_LHE.py \
    --outputEVNTFile=test_mgpythia8_tt_NLO.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15


