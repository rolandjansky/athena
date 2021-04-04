#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Pythia event generation -- minbias
# art-type: grid
# art-output: EVNT.root
# art-output: MyOutput.root
# art-output: MyOutput.yoda.gz
# art-output: dcube
# art-html: dcube


Gen_tf.py --ecmEnergy=13000. --maxEvents=10000 --firstEvent=-1 --randomSeed=123456 --outputEVNTFile=EVNT.root --outputYODAFile=MyOutput.yoda.gz --postInclude=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/JO_MC/JO_MC_minbias.py --jobConfig=421113

echo "art-result:$? Gen_tf"

asetup 22.6.1,AthGeneration
source setupRivet

python /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/rootconvert.py MyOutput.yoda.gz

dcubeName = "Pythia8i"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/test_02_minbias/config_minbias.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/test_02_minbias/MyOutput.root"

bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName MyOutput.root $dcubeXml $dcubeRef

#yodadiff -o yodadiff MyOutput.yoda.gz ~/Qualification_task_ART/Pythia_8/result/Pythia_8/test_01_Z_tautau/MyOutput.yoda.gz     # use this for histogram comparison if one only care if they are exactly the same
echo  "art-result: $? Dcube" 






