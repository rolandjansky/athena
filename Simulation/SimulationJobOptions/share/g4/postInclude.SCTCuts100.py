#########################################################
#
# SimulationJobOptions/postInitOptions.SCTCuts100.py
# Pat Ward - adapted from postInitOptions.PixelCuts100.py by Zach Marshall
#
# For a special production to evaluate the effect of low
# energy secondaries on SCT Clustering.
#
# This job option should be added via the postInitInclude
# command line argument.
#
#########################################################

atlasG4log.info("G4 SCT Config: Setting SCT cut")
from AthenaCommon.CfgGetter import getService
sctRegionTool = getService('DetectorGeometrySvc').RegionCreators['SCTPhysicsRegionTool']
sctRegionTool.ElectronCut = 0.1
sctRegionTool.PositronCut = 0.1
sctRegionTool.GammaCut = 0.1
