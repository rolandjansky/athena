#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions_mc.py
# art-type: grid
# art-include: 21.2/AthAnalysis
# art-output: monitoring.MCa.root
# art-output: dcube

# Create empty pool file
art.py createpoolfile

echo "Running SUSYTools test: 'share/minimalExampleJobOptions_mc.py -c \'MCCampaign=\"MCa\"\' '"
athena SUSYTools/minimalExampleJobOptions_mc.py --evtMax=2000 -c 'MCCampaign="MCa"'
echo  "art-result: $? TEST"

echo "Running DCube post-processing"

tName="MCa"
dcubeRef=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/References/monitoring.MCa.root
dcubeXml=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/xml/dcube_config_master.xml

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube ${tName} monitoring.MCa.root ${dcubeXml} ${dcubeRef}

echo "art-result: $? DCUBE"

echo "Done."
