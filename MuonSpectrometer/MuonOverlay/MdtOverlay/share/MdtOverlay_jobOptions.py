include.block ( "MdtOverlay/MdtOverlay_jobOptions.py" )

#from Digitization.DigitizationFlags import jobproperties
from AthenaCommon import CfgGetter
#from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigitTool
#MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool (
#                         "MdtRdoToMdtDigit",
#                         DecodeMdtRDO = True,
#                         DecodeRpcRDO = False,
#                         DecodeTgcRDO = False,
#                         DecodeCscRDO = False ) 
#ToolSvc += CfgGetter.getPrivateTool("MdtRdoToMdtDigit")
ToolSvc += CfgGetter.getPrivateTool("MdtRdoToMdtDigit")
#job += MdtRdoToMdtDigit()

#include ( "MDT_Digitization/MdtDigitizationTool_jobOptions.py" )

#if readBS and isRealData:
#   ToolSvc.MdtDigitizationTool.GetT0FromBD = True

#t if readBS and isRealData:
#t   if not hasattr(ToolSvc, 'Mdt_OverlayDigitizationTool'):
# t      from AthenaCommon import CfgGetter
#  t     ToolSvc += CfgGetter.getPrivateTool("Mdt_OverlayDigitizationTool")

#, checkType=True)
 #  ToolSvc.Mdt_OverlayDigitizationTool.GetT0FromBD = True

#jobproperties.Digitization.rndmSeedList.addSeed("MDTResponse", 49261510,105132394 )
#jobproperties.Digitization.rndmSeedList.addSeed("MDT_Digitization", 393242561, 857132381 )

#from AthenaCommon import CfgGetter
#from MdtOverlay.MdtOverlayConf import MdtOverlay
job += CfgGetter.getAlgorithm("MdtOverlay")
#job.MdtOverlay.mainInputMDT_Name                         = "MDT_DIGITS"
#job.MdtOverlay.overlayInputMDT_Name                      = "MDT_DIGITS"
#job.MdtOverlay.IntegrationWindow                         = 20
#job.MdtOverlay.DigitizationTool                          = ToolSvc.Mdt_OverlayDigitizationTool
#job.MdtOverlay.ConvertRDOToDigitTool                     = MuonRdoToMuonDigitTool
#job.MdtOverlay.DigitizationTool.EvtStore                 = job.MdtOverlay.MCStore
#job.MdtOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = True
#job.MdtOverlay.ConvertRDOToDigitTool.DataStore           = job.MdtOverlay.DataStore

#from OverlayCommonAlgs.OverlayFlags import overlayFlags
#if overlayFlags.doSignal==True:
#   job.MdtOverlay.CopyObject = True
   
#from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MdtDigitToMdtRDO
#job += MdtDigitToMdtRDO()
#job.MdtDigitToMdtRDO.Store = job.MdtOverlay.OutputStore


#from MuonByteStreamCnvTest.MuonByteStreamCnvTestConfig import MdtDigitToMdtRDO
#job += MdtDigitToMdtRDO()
#job.MdtDigitToMdtRDO.Store = job.MdtOverlay.OutputStore
#job +=  CfgGetter.getPrivateTool("MdtRdoToMdtDigit")
from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MdtDigitToMdtRDO
job += MdtDigitToMdtRDO()
job.MdtDigitToMdtRDO.Store = job.MdtOverlay.OutputStore
