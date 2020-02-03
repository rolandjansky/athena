#!/bin/bash
# art-description: Generation test H7 dijets
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=429699 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID429xxx/MC15.429699.Herwig7_Dijet.py  \
    --outputEVNTFile=test_herwig7_dijet.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15


