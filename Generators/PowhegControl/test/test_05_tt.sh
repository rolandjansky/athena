#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Powheg Control ART test tt
# art-type: grid
# art-output: test_powheg_W.TXT.tar.gz
# art-output: dcube
# art-html: dcube

Gen_tf.py --ecmEnergy=13000. --maxEvents=10000 --firstEvent=-1 --randomSeed=123456 --jobConfig=421358 --outputTXTFile=test_powheg_tt.TXT.tar.gz 

echo "art-result:$? Gen_tf"

simple_lhe_plotter.py test_powheg_tt.TXT.events

echo "art-result: $? Plot