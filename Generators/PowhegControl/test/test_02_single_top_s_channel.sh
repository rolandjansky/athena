#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Powheg Control ART test single top s-channel
# art-type: grid
# art-output: test_powheg_ZZ.TXT.tar.gz
# art-output: output_hists.root
# art-output: dcube
# art-html: dcube

Gen_tf.py --ecmEnergy=13000. --maxEvents=10000 --firstEvent=-1 --randomSeed=123456 --jobConfig=421358 --outputTXTFile=test_powheg_single_top_s_channel.TXT.tar.gz 

echo "art-result: $? Gen_tf"

simple_lhe_plotter.py test_powheg_single_top_s_channel.TXT.events

echo "art-result: $? Plot"

dcubeName="Powheg_LHE"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PowhegControl/config_file/test_02_config.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/PowhegControl/master_branch/reference_file/test_02_output_hists.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName output_hists.root $dcubeXml $dcubeRef

echo  "art-result: $? DCube"
