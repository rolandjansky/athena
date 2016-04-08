#########################################################
#
# SimulationJobOptions/preInclude.FastMBKiller.py
# Zach Marshall
#
# Requested for the Atlfast-II and MinBias groups
#########################################################

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
from AthenaCommon.CfgGetter import getPublicTool

FastMBKiller=getPublicTool('FastIDKiller/FastMBKiller',tryDefaultConfigurable=True)
FastMBKiller.Z=3600
FastMBKiller.R=14

UAStore.addAction(FastMBKiller, ['BeginOfRun','Step'])
