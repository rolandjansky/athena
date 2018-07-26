#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions_data.py
# art-type: grid
# art-include: 21.2/AthAnalysis
# art-output: monitoring.data.root
# art-output: dcube

# Create empty pool file
art.py createpoolfile

echo "Running SUSYTools test: \'share/minimalExampleJobOptions_data.py\'"
athena SUSYTools/minimalExampleJobOptions_data.py --evtMax=-1
echo  "art-result: $? TEST"

echo "Running DCube post-processing"

tName="DATA"
dcubeRef=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/References/monitoring.data.root
dcubeXml=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/xml/dcube_config_master.xml

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube ${tName} monitoring.data.root ${dcubeXml} ${dcubeRef}

echo "art-result: $? DCUBE"

echo "Done."
