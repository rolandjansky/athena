#!/bin/sh

# art-description: SUSYTools ART test - TestSUSYToolsAlg.py
# art-type: grid
# art-include: 21.2/AnalysisBase
# art-output: hist-MC_DAOD_PHYSVAL.root
# art-output: dcube

# Create empty pool file
art.py createpoolfile

echo "Running SUSYTools test: \'TestSUSYToolsAlg.py -t MC -m -1\'"
TestSUSYToolsAlg.py -t MC -m -1
echo  "art-result: $? TEST"

mv submitDir/hist-DAOD_PHYSVAL.root ./hist-MC_DAOD_PHYSVAL.root

echo "Running DCube post-processing"

tName="MC"
dcubeRef=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/References/hist-MC_DAOD_PHYSVAL.root
dcubeXml=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/xml/dcube_config_master.xml

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube ${tName} hist-MC_DAOD_PHYSVAL.root ${dcubeXml} ${dcubeRef}

echo "art-result: $? DCUBE"

echo "Done."
