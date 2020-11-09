#!/bin/sh

# art-description: SUSYTools ART test - share/minimalExampleJobOptions_mc.py
# art-type: grid
# art-include: 21.2/AthAnalysis
# art-output: monitoring.MCd.root
# art-output: dcube

# Create empty pool file
art.py createpoolfile

echo "Running SUSYTools test: 'share/minimalExampleJobOptions_mc.py -c \'MCCampaign=\"MCd\"\' '"
athena SUSYTools/minimalExampleJobOptions_mc.py --evtMax=2000 -c 'MCCampaign="MCd"'
echo  "art-result: $? TEST"

echo "Running DCube post-processing"

tName="MCd"
dcubeRef=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/References/monitoring.MCd.root
dcubeXml=/cvmfs/atlas.cern.ch/repo/sw/database/GroupData/dev/SUSYTools/ART/xml/dcube_config_master.xml

/cvmfs/atlas.cern.ch/repo/sw/art/dcube/bin/art-dcube ${tName} monitoring.MCd.root ${dcubeXml} ${dcubeRef}

echo "art-result: $? DCUBE"

echo "Done."
