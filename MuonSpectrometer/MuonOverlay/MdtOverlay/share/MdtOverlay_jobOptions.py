include.block ( "MdtOverlay/MdtOverlay_jobOptions.py" )

from Digitization.DigitizationFlags import jobproperties

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigitTool
MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool (
                         "MdtRdoToMdtDigit",
                         DecodeMdtRDO = True,
                         DecodeRpcRDO = False,
                         DecodeTgcRDO = False,
                         DecodeCscRDO = False ) 
ToolSvc += MuonRdoToMuonDigitTool

#include ( "MDT_Digitization/MdtDigitizationTool_jobOptions.py" )

#if readBS and isRealData:
#   ToolSvc.MdtDigitizationTool.GetT0FromBD = True
if readBS and isRealData:
   if not hasattr(ToolSvc, 'MdtDigitizationTool'):
       from AthenaCommon import CfgGetter
       ToolSvc += CfgGetter.getPrivateTool("MdtDigitizationTool", checkType=True)
   ToolSvc.MdtDigitizationTool.GetT0FromBD = True

#jobproperties.Digitization.rndmSeedList.addSeed("MDTResponse", 49261510,105132394 )
#jobproperties.Digitization.rndmSeedList.addSeed("MDT_Digitization", 393242561, 857132381 )

from AthenaCommon import CfgGetter
from MdtOverlay.MdtOverlayConf import MdtOverlay
job += MdtOverlay()
job.MdtOverlay.mainInputMDT_Name                         = "MDT_DIGITS"
job.MdtOverlay.overlayInputMDT_Name                      = "MDT_DIGITS"
job.MdtOverlay.IntegrationWindow                         = 20
job.MdtOverlay.DigitizationTool                          = ToolSvc.MdtDigitizationTool
job.MdtOverlay.ConvertRDOToDigitTool                     = MuonRdoToMuonDigitTool
job.MdtOverlay.DigitizationTool.EvtStore                 = job.MdtOverlay.MCStore
job.MdtOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = True
job.MdtOverlay.ConvertRDOToDigitTool.DataStore           = job.MdtOverlay.DataStore

from OverlayCommonAlgs.OverlayFlags import OverlayFlags
if OverlayFlags.doSignal():
   job.MdtOverlay.CopyObject = True

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MdtDigitToMdtRDO
job += MdtDigitToMdtRDO()
job.MdtDigitToMdtRDO.Store = job.MdtOverlay.OutputStore


