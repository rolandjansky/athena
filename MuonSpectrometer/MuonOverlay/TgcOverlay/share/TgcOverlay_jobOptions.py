from Digitization.DigitizationFlags import jobproperties

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigitTool
MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool (
                         "TgcRdoToTgcDigit",
                         DecodeMdtRDO = False,
                         DecodeRpcRDO = False,
                         DecodeTgcRDO = True,
                         DecodeCscRDO = False ) 
ToolSvc += MuonRdoToMuonDigitTool

from AthenaCommon import CfgGetter

from TgcOverlay.TgcOverlayConf import TgcOverlay
job += TgcOverlay()
job.TgcOverlay.mainInputTGC_Name                         = "TGC_DIGITS"
job.TgcOverlay.overlayInputTGC_Name                      = "TGC_DIGITS"
#job.TgcOverlay.DigitizationTool = CfgGetter.getPrivateTool("TgcDigitizationTool", checkType=True)
job.TgcOverlay.DigitizationTool = CfgGetter.getPrivateTool("Tgc_OverlayDigitizationTool")
job.TgcOverlay.ConvertRDOToDigitTool                     = MuonRdoToMuonDigitTool
#job.TgcOverlay.DigitizationTool.EvtStore                  = job.TgcOverlay.MCStore
job.TgcOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = True
job.TgcOverlay.ConvertRDOToDigitTool.DataStore           = job.TgcOverlay.DataStore

from OverlayCommonAlgs.OverlayFlags import overlayFlags
if overlayFlags.doSignal==True:
   job.TgcOverlay.CopyObject = True

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import TgcDigitToTgcRDO
job += TgcDigitToTgcRDO()
job.TgcDigitToTgcRDO.Store = job.TgcOverlay.OutputStore


