#!/bin/bash
# art-description: Generation test Starlight
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=5020 --runNumber=420252 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID420xxx/MC15.420252.MC15.420252.Starlight_gammagamma2ee_3p6M8.py  \
    --outputEVNTFile=test_starlight_gammagamma2ee.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15




