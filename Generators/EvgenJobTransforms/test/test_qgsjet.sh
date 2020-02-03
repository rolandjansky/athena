#!/bin/bash
# art-description: Generation test QGSjet  
# art-type: build
# art-include: master/Athena
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=361235 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID361xxx/MC15.361235.QGSJet_minbias_inelastic.py  \
    --outputEVNTFile=test_qgsjet_minbias_inelastic.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15




