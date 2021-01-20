#!/bin/sh

# art-include: 21.6/AthGeneration
# art-description: MadGraph Event Generation Test - SUSY OTF EVNT+TXT
# art-type: grid
# art-output: test_lhe_events.events
# art-output: output_hists.root
# art-output: EVNT.root
# art-output: dcube
# art-html: dcube

Gen_tf.py --ecmEnergy=13000. --maxEvents=-1 --firstEvent=1 --randomSeed=123456 --outputEVNTFile=EVNT.root --outputTXTFile=test_lhe_events --jobConfig=950120

echo "art-result: $? Gen_tf"

simple_lhe_plotter.py test_lhe_events.events

echo "art-result: $? Plot"

# Run tests on the log file
env -u PYTHONPATH -u PYTHONHOME python3 /cvmfs/atlas.cern.ch/repo/sw/Generators/MCJobOptions/scripts/logParser.py -s -i log.generate

echo "art-result: $? log-check"

dcubeName="LHE"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MadGraphControl/LHE_DCubeConfig.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MadGraphControl/test_20_output_hists.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName output_hists.root $dcubeXml $dcubeRef
echo  "art-result: $? DCube"
