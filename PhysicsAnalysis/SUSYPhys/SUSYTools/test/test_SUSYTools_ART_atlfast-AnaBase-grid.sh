#!/bin/sh

# art-description: SUSYTools ART test - TestSUSYToolsAlg.py
# art-type: grid
# art-include: 21.2/AnalysisBase
# art-output: hist-AFII_DAOD_PHYSVAL.root
# art-output: dcube

# Create empty pool file
art.py createpoolfile

echo "Running SUSYTools test: \'TestSUSYToolsAlg.py -t mc16e --AFII -m 1500\'"
TestSUSYToolsAlg.py -t mc16e --AFII -m 1500
echo  "art-result: $? TEST"

mv submitDir/hist-SUSYTools.root ./hist-AFII_DAOD_PHYSVAL.root

echo "Running DCube post-processing"

tName="AF2"
dcubeRef=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/References/hist-AFII_DAOD_PHYSVAL.root
dcubeXml=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/xml/dcube_config_master.xml

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube ${tName} hist-AFII_DAOD_PHYSVAL.root ${dcubeXml} ${dcubeRef}

echo "art-result: $? DCUBE"

echo "Done."
