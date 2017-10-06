from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import FastCaloSimParamAlg
topSeq += FastCaloSimParamAlg()
ISF_HITSStream.stream1.ItemList += ["ISF_FCS_Parametrization::FCS_StepInfoCollection#MergedEventSteps"]
from AthenaCommon.CfgGetter import getPublicTool
stepInfoSDTool = getPublicTool("SensitiveDetectorMasterTool").SensitiveDetectors['FCS_StepInfoSensitiveDetector']
stepInfoSDTool.shift_lar_subhit=True
stepInfoSDTool.shorten_lar_step=True
stepInfoSDTool.maxRadiusLAr=1
