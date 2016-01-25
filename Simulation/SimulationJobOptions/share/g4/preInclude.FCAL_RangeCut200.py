#########################################################
#
# Sets FCAL range cut to 0.2 mm (default 0.03 mm)
#
#########################################################

from AthenaCommon.CfgGetter import getPublicTool
FCALRegionTool = getPublicTool('FCALPhysicsRegionTool')
FCALRegionTool.ElectronCut = 0.2
FCALRegionTool.PositronCut = 0.2
FCALRegionTool.GammaCut = 0.2
