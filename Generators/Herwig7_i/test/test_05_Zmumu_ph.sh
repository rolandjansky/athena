#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Herwig ART test Zmumu
# art-type: grid
# art-output: test_05_Zmumu.TXT.tar.gz
# art-output: output_hists.root
# art-output: log.generate
# art-output: dcube
# art-html: dcube

Gen_tf.py --ecmEnergy=13000. --maxEvents=1000 --firstEvent=-1 --randomSeed=123456 --jobConfig=600433 --outputTXTFile=test_05_Zmumu.TXT.tar.gz 

echo "art-result:$? Gen_tf"

simple_lhe_plotter.py test_05_Zmumu.TXT.events

echo "art-result: $? Plot"

dcubeName="Herwig7"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Herwig7_i/config_file/test_05_config.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Herwig7_i/reference/test_05_output.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName output_hists.root $dcubeXml $dcubeRef

echo  "art-result: $? DCube"
