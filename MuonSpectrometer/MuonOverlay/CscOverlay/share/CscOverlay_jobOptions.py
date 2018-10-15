from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.CfgGetter import getAlgorithm

job += getAlgorithm("CscOverlay")
from OverlayCommonAlgs.OverlayFlags import overlayFlags
job.CscOverlay.DigitizationTool.OutputObjectName = overlayFlags.evtStore() + "+CSC_DIGITS" #private tool, so this should work
#job.CscOverlay.OutputLevel=VERBOSE
#svcMgr.MessageSvc.defaultLimit=100000
print job.CscOverlay

