#!/bin/bash
# art-description: Generation test H7 min_bias 
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=429700 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID429xxx/MC15.429700.Herwig7_MinBias.py  \
    --outputEVNTFile=test_herwig7_minbias_inelastic.EVNT.pool.root \

echo "art-result: $? generate"

rm -rf _joproxy15
   



