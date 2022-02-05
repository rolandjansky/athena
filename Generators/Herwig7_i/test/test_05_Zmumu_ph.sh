#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Herwig ART test Zmumu
# art-type: grid
# art-output: MyOutput.yoda.gz
# art-output: MyOutput.root
# art-output: log.generate
# art-output: dcube
# art-html: dcube

Gen_tf.py --ecmEnergy=13000. --maxEvents=1000 --firstEvent=-1 --randomSeed=123456 --jobConfig=600433 --outputYODAFile=MyOutput.yoda.gz --outputEVNTFile=EVNT.root --postInclude=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Herwig7_i/MC_JO/MC_test_00_dijet.py  

echo "art-result:$? Gen_tf"

asetup 22.6.1,AthGeneration
source setupRivet

python /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/rootconvert.py MyOutput.yoda.gz

echo "art-result: $? Plot"

dcubeName="Herwig7"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Herwig7_i/config_file/test_05_config.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Herwig7_i/master_branch/reference/test_05_output.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName MyOutput.root $dcubeXml $dcubeRef

echo  "art-result: $? DCube"
