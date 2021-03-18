#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Pythia event generation -- Zprime to tt
# art-type: grid
# art-output: EVNT_Pythia

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT_Pythia --jobConfig=950072

echo "art-result:$? Gen_tf"


