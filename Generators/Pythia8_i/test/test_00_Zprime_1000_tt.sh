#!/bin/sh

# art-include: 21.6/AthGeneration
# art-include: master/AthGeneration
# art-description: Pythia event generation -- Zprime to tautau
# art-type: grid
# art-output: EVNT.root
# art-output: MyOutput.root
# art-output: MyOutput.yoda.gz
# art-output: dcube
# art-html: dcube


Gen_tf.py --ecmEnergy=13000. --maxEvents=10000 --firstEvent=-1 --randomSeed=123456 --outputEVNTFile=EVNT.root --outputYODAFile=MyOutput.yoda.gz --postInclude=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/JO_MC/JO_MC_Zprime.py --jobConfig=950072

source setupRivet.sh

echo "art-result: $? Gen_tf"

python /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/rootconvert.py MyOutput.yoda.gz

echo "art-result: $? convert"

dcubeName="Pythia8i"
dcubeXml="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/test_00_Zprime_1000_tt/config_Zprime.xml"
dcubeRef="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Pythia8i/test_00_Zprime_1000_tt/MyOutput.root"


bash /cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube $dcubeName MyOutput.root $dcubeXml $dcubeRef

#yodadiff -o yodadiff MyOutput.yoda.gz ~/Qualification_task_ART/Pythia_8/result/Pythia_8/test_00_Zprime_1000_tt/MyOutput.yoda.gz     # use this for histogram comparison if one only care if they are exactly the same
echo  "art-result: $? Dcube"






