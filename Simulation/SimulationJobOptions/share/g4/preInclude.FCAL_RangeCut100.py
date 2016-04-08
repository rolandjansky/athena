#########################################################
#
# Sets FCAL range cut to 0.1 mm (default 0.03 mm)
#
#########################################################

from AthenaCommon.CfgGetter import getPublicTool
FCALRegionTool = getPublicTool('FCALPhysicsRegionTool')
FCALRegionTool.ElectronCut = 0.1
FCALRegionTool.PositronCut = 0.1
FCALRegionTool.GammaCut = 0.1
