#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: MadGraph Event Generation Test - Cluster grid pack
# art-type: grid

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputTXTFile=fake_lhe_events --jobConfig=950111 --outputFileValidation=False
# Note 11.4.2020: This crashes on the grid (duh, it's designed to use a cluster)
echo "art-result: $? Gen_tf"
