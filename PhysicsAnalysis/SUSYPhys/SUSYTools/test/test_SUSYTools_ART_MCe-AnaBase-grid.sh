#!/bin/sh

# art-description: SUSYTools ART test - TestSUSYToolsAlg.py
# art-type: grid
# art-include: 22.2/AnalysisBase
# art-output: hist-MCe_DAOD_PHYS.root
# art-output: dcube

# Create empty pool file
art.py createpoolfile

echo "Running SUSYTools test: \'TestSUSYToolsAlg.py -t mc16e -m 1500\' -p 4441 -f PHYS --dosyst"
TestSUSYToolsAlg.py -t mc16e -m 1500 -p p4441 -f PHYS --dosyst
echo  "art-result: $? TEST"

mv submitDir/hist-ARTInput.root ./hist-MCe_DAOD_PHYS.root

echo "Running DCube post-processing"

tName="MCe"
dcubeRef=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/References/hist-MCe_DAOD_PHYS-rel22.root
dcubeXml=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/xml/dcube_config_master_rel22.xml

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube ${tName} hist-MCe_DAOD_PHYS.root ${dcubeXml} ${dcubeRef}

echo "art-result: $? DCUBE"

echo "Done."
