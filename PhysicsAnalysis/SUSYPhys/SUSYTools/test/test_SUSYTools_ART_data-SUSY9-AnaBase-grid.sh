#!/bin/sh

# art-description: SUSYTools ART test - TestSUSYToolsAlg.py
# art-type: grid
# art-include: 21.2/AnalysisBase
# art-output: hist-DATA_DAOD_SUSY9.root
# art-output: dcube

# Create empty pool file
art.py createpoolfile

echo "Running SUSYTools test: \'TestSUSYToolsAlg.py -t data18 -m 5000 -d 9\'"
TestSUSYToolsAlg.py -t data18 -m 5000 -d 9
echo  "art-result: $? TEST"

mv submitDir/hist-ARTInput.root ./hist-DATA_DAOD_SUSY9.root

echo "Running DCube post-processing"

tName="DATA-SUSY9"
dcubeRef=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/References/hist-DATA_DAOD_SUSY9.root
dcubeXml=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/xml/dcube_config_master.xml

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube ${tName} hist-DATA_DAOD_SUSY9.root ${dcubeXml} ${dcubeRef}

echo "art-result: $? DCUBE"

echo "Done."
