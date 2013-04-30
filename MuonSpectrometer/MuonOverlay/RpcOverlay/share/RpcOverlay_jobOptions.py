from Digitization.DigitizationFlags import jobproperties

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigitTool
MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool (
                         "RpcRdoToRpcDigit",
                         DecodeMdtRDO = False,
                         DecodeRpcRDO = True,
                         DecodeTgcRDO = False,
                         DecodeCscRDO = False ) 
ToolSvc += MuonRdoToMuonDigitTool

#old way
#include ( "RPC_Digitization/RpcDigitizationTool_jobOptions.py" )
#jobproperties.Digitization.rndmSeedList.addSeed("RPC_Digitization", 49261510, 105132394 )

#new way
if not hasattr(ToolSvc, 'RpcDigitizationTool'):
   from AthenaCommon import CfgGetter
   RpcDigitizationTool = CfgGetter.getPrivateTool("RpcDigitizationTool", checkType=True)

from RpcOverlay.RpcOverlayConf import RpcOverlay
job += RpcOverlay()
job.RpcOverlay.mainInputRPC_Name                         = "RPC_DIGITS"
job.RpcOverlay.overlayInputRPC_Name                      = "RPC_DIGITS"
job.RpcOverlay.DigitizationTool                          = RpcDigitizationTool
job.RpcOverlay.ConvertRDOToDigitTool                     = MuonRdoToMuonDigitTool
job.RpcOverlay.DigitizationTool.EvtStore                 = job.RpcOverlay.MCStore
job.RpcOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = True
job.RpcOverlay.ConvertRDOToDigitTool.DataStore           = job.RpcOverlay.DataStore

from OverlayCommonAlgs.OverlayFlags import OverlayFlags
if OverlayFlags.doSignal():
   job.RpcOverlay.CopyObject = True

from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import RpcDigitToRpcRDO
job += RpcDigitToRpcRDO()
job.RpcDigitToRpcRDO.Store = job.RpcOverlay.OutputStore

