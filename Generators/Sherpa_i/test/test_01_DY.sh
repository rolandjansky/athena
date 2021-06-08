#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Sherpa DY test
# art-type: grid
# art-output: EVNT.root
# art-output: MyOutput.root
# art-output: MyOutput.yoda.gz
# art-output: dcube
# art-html: dcube

Gen_tf.py --ecmEnergy=13000. --maxEvents=10000 --firstEvent=-1 --randomSeed=123456 --outputEVNTFile=EVNT.root --jobConfig=421001 --outputYODAFile=MyOutput.yoda.gz --postInclude=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Herwig7_i/MC_JO/MC_test_05_Zmumu.py
echo "art-result: $? Gen_tf"

source setupRivet.sh

python /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/rootconvert.py MyOutput.yoda.gz

echo "art-result: $? convert"

dcubeName="Sherpa"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Sherpa_i/test_01_DY/test_01_config.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Sherpa_i/test_01_DY/test_01_hists.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName MyOutput.root $dcubeXml $dcubeRef

echo  "art-result: $? Dcube"