#########################################################
#
# Sets EMEC range cut to 0.2 mm (default 0.1 mm)
#
#########################################################

from AthenaCommon.CfgGetter import getPublicTool
EMECRegionTool = getPublicTool('EMECPhysicsRegionTool')
EMECRegionTool.ElectronCut = 0.2
EMECRegionTool.PositronCut = 0.2
EMECRegionTool.GammaCut = 0.2
