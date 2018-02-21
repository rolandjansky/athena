#########################################################
#
# SimulationJobOptions/postInitOptions.SCTCuts50.py
# Pat Ward - adapted from postInitOptions.PixelCuts50.py by Zach Marshall
#
# For a special production to evaluate the effect of low
# energy secondaries on SCT Clustering.
#
#########################################################

atlasG4log.info("G4 SCT Config: Setting SCT cut")
from AthenaCommon.CfgGetter import getService
sctRegionTool = getService('DetectorGeometrySvc').RegionCreators['SCTPhysicsRegionTool']
sctRegionTool.ElectronCut = 0.05
sctRegionTool.PositronCut = 0.05
sctRegionTool.GammaCut = 0.05
