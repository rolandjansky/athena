include.block ( "EventOverlayJobTransforms/MuonMcSignal_jobOptions.py" )

# run the muon Digit -> RDO for the signal in the MC Store
# and, in the case where the background is MC generated, 
# also for the background in the temporary background store
# if the background is real data, we do not do Digit -> RDO
# in the temporary background store because of MDT Cabling problem
# for CSC, Digit -> RDO was already done during overlay and 
# should not be repeated here

from AthenaCommon.DetFlags import DetFlags

theApp.Dlls += [ "MuonByteStreamCnvTest" ]
theApp.Dlls += [ "TrigT1RPChardware" ]
theApp.Dlls += [ "TrigT1RPCsteering" ]
theApp.Dlls += [ "TrigT1RPCmonitoring" ]
from AthenaCommon.CfgGetter import getAlgorithm
if DetFlags.overlay.MDT_on():
        job += getAlgorithm( "SigMdtDigitToMdtRDO" )
        #ACH if readBS==False:
           #ACH job += MdtDigitToMdtRDO( "BkgMdtDigitToMdtRDO" )
           #ACH job.BkgMdtDigitToMdtRDO.Store = job.MdtOverlay.TempBkgStore
if DetFlags.overlay.RPC_on():
        job += getAlgorithm( "SigRpcDigitToRpcRDO" )
        #ACH if readBS==False:
           #ACH job += RpcDigitToRpcRDO( "BkgRpcDigitToRpcRDO" )
           #ACH job.BkgRpcDigitToRpcRDO.Store = job.RpcOverlay.TempBkgStore
if DetFlags.overlay.TGC_on():
        job += getAlgorithm( "SigTgcDigitToTgcRDO" )
        #ACH if readBS==False:
           #ACH job += TgcDigitToTgcRDO( "BkgTgcDigitToTgcRDO" )
           #ACH job.BkgTgcDigitToTgcRDO.Store = job.TgcOverlay.TempBkgStore


