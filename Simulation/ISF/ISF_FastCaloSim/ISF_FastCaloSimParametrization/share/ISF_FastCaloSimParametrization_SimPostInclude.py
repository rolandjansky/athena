from ISF_FastCaloSimParametrization.ISF_FastCaloSimParametrizationConf import FastCaloSimParamAlg
topSeq += FastCaloSimParamAlg()
ISF_HITSStream.stream1.ItemList += ["ISF_FCS_Parametrization::FCS_StepInfoCollection#MergedEventSteps"]
from AthenaCommon.CfgGetter import getPublicTool
stepInfoSDTool = getPublicTool("SensitiveDetectorMasterTool").SensitiveDetectors['FCS_StepInfoSensitiveDetector']
stepInfoSDTool.shift_lar_subhit=True
stepInfoSDTool.shorten_lar_step=True

stepInfoSDTool.maxEtaPS=1
stepInfoSDTool.maxPhiPS=5
stepInfoSDTool.maxrPS=0

stepInfoSDTool.maxEtaEM1=1
stepInfoSDTool.maxPhiEM1=5
stepInfoSDTool.maxrEM1=15

stepInfoSDTool.maxEtaEM2=1
stepInfoSDTool.maxPhiEM2=5
stepInfoSDTool.maxrEM2=60

stepInfoSDTool.maxEtaEM3=1
stepInfoSDTool.maxPhiEM3=5
stepInfoSDTool.maxrEM3=8

