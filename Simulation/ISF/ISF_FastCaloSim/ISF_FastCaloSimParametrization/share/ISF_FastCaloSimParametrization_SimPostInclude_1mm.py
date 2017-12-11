from AthenaCommon.CfgGetter import getPublicTool
stepInfoSDTool = getPublicTool("SensitiveDetectorMasterTool").SensitiveDetectors['FCS_StepInfoSensitiveDetector']
stepInfoSDTool.shift_lar_subhit=True
stepInfoSDTool.shorten_lar_step=True
stepInfoSDTool.maxRadiusLAr=1
