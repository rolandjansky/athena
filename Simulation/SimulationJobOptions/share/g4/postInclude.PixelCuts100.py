#########################################################
#
# SimulationJobOptions/postInitOptions.PixelCuts100.py
# Zach Marshall
#
# For a special production to evaluate the effect of low
# energy secondaries on Pixel Clustering.
#
#########################################################

atlasG4log.info("G4 PIX Config: Setting PIX cut")
from AthenaCommon.CfgGetter import getService
pixelRegionTool = getService('DetectorGeometrySvc').RegionCreators['PixelPhysicsRegionTool']
pixelRegionTool.ElectronCut = 0.1
pixelRegionTool.PositronCut = 0.1
pixelRegionTool.GammaCut = 0.1
