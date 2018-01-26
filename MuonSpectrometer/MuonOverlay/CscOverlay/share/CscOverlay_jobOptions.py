from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.CfgGetter import getPublicTool
from OverlayCommonAlgs.OverlayFlags import overlayFlags
getPublicTool("CscDigitToCscRDOTool")

from CscOverlay.CscOverlayConf import CscOverlay
job += CscOverlay()
job.CscOverlay.mainInputCSC_Name         = "CSCRDO"
job.CscOverlay.overlayInputCSC_Name      = "CSCRDO"
job.CscOverlay.DigitizationTool          = getPublicTool("CscDigitizationTool") #CscDigitizationTool
job.CscOverlay.DigitizationTool.EvtStore = job.CscOverlay.MCStore
job.CscOverlay.DigitizationTool.OutputObjectName = overlayFlags.evtStore() + "+CSC_DIGITS"
job.CscOverlay.MakeRDOTool2              = ToolSvc.CscDigitToCscRDOTool.clone("CscDigitToCscRDOTool2") # consider a separate random number stream
job.CscOverlay.MakeRDOTool2.EvtStore     = job.CscOverlay.MCStore
job.CscOverlay.MakeRDOTool2.addNoise     = False
job.CscOverlay.MakeRDOTool2.NumSamples   = 2
job.CscOverlay.MakeRDOTool4              = ToolSvc.CscDigitToCscRDOTool.clone("CscDigitToCscRDOTool4") # consider a separate random number stream
job.CscOverlay.MakeRDOTool4.EvtStore     = job.CscOverlay.MCStore
job.CscOverlay.MakeRDOTool4.addNoise     = False
job.CscOverlay.MakeRDOTool4.NumSamples   = 4
job.CscOverlay.RndmSvc                   = digitizationFlags.rndmSvc.get_Value()

#job.CscOverlay.OutputLevel=VERBOSE
#svcMgr.MessageSvc.defaultLimit=100000

digitizationFlags.rndmSeedList.addSeed("CscOverlay", 12261551, 245536301 )

print job.CscOverlay

