#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - LO MadSpin
# art-type: grid
# art-output: test_lhe_events.events
# art-output: output_hists.root

set -e
 
Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputTXTFile=test_lhe_events --jobConfig=950107

echo "art-result: $?"

simple_lhe_plotter.py test_lhe_events.events
