#!/bin/bash
# art-description: Generation test H7 Zjets
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=13000 --runNumber=429721 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID429xxx/MC15.429721.Herwig7_H7UE_MMHT2014lo68cl_Zjets.py  \
    --outputEVNTFile=test_herwig7_zjets_inelastic.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15



