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

if DetFlags.overlay.MDT_on():
        from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MdtDigitToMdtRDO
        job += MdtDigitToMdtRDO( "SigMdtDigitToMdtRDO" )
        job.SigMdtDigitToMdtRDO.EvtStore = job.MdtOverlay.MCStore
        #ACH if readBS==False:
           #ACH job += MdtDigitToMdtRDO( "BkgMdtDigitToMdtRDO" )
           #ACH job.BkgMdtDigitToMdtRDO.Store = job.MdtOverlay.TempBkgStore
if DetFlags.overlay.RPC_on():
        from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import RpcDigitToRpcRDO
        job += RpcDigitToRpcRDO( "SigRpcDigitToRpcRDO" )
        job.SigRpcDigitToRpcRDO.EvtStore = job.RpcOverlay.MCStore
        #ACH if readBS==False:
           #ACH job += RpcDigitToRpcRDO( "BkgRpcDigitToRpcRDO" )
           #ACH job.BkgRpcDigitToRpcRDO.Store = job.RpcOverlay.TempBkgStore
if DetFlags.overlay.TGC_on():
        from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import TgcDigitToTgcRDO
        job += TgcDigitToTgcRDO( "SigTgcDigitToTgcRDO" )
        job.SigTgcDigitToTgcRDO.EvtStore = job.TgcOverlay.MCStore
        #ACH if readBS==False:
           #ACH job += TgcDigitToTgcRDO( "BkgTgcDigitToTgcRDO" )
           #ACH job.BkgTgcDigitToTgcRDO.Store = job.TgcOverlay.TempBkgStore


