#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Pythia event generation -- Z to tautau
# art-type: grid
# art-output: EVNT.root

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=950073

echo "art-result:$? Gen_tf"


