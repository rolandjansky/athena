#########################################################
#
# SimulationJobOptions/postInitOptions.PixelCuts10.py
# Zach Marshall
#
# For a special production to evaluate the effect of low
# energy secondaries on Pixel Clustering.
#
#########################################################

atlasG4log.info("G4 PIX Config: Setting PIX cut")
from AthenaCommon.CfgGetter import getService
pixelRegionTool = getService('DetectorGeometrySvc').RegionCreators['PixelPhysicsRegionTool']
pixelRegionTool.ElectronCut = 0.01
pixelRegionTool.PositronCut = 0.01
pixelRegionTool.GammaCut = 0.01
