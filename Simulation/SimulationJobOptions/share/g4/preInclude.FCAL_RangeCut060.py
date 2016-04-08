#########################################################
#
# Sets FCAL range cut to 0.06 mm (default 0.03 mm)
#
#########################################################

from AthenaCommon.CfgGetter import getPublicTool
FCALRegionTool = getPublicTool('FCALPhysicsRegionTool')
FCALRegionTool.ElectronCut = 0.06
FCALRegionTool.PositronCut = 0.06
FCALRegionTool.GammaCut = 0.06
