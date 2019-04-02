include.block ( "EventOverlayJobTransforms/MuonOverlay_jobOptions.py" )

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags
from AthenaCommon import CfgGetter

from RecExConfig.RecFlags import rec as recFlags

if DetFlags.overlay.MDT_on() or DetFlags.overlay.CSC_on() or DetFlags.overlay.RPC_on() or DetFlags.overlay.TGC_on():
   
    include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )
 
    import MuonRecExample.MuonReadCalib
    if readBS and isRealData:
       theApp.Dlls += [ "MuonByteStream"]
       if DetFlags.overlay.CSC_on():
          from MuonRecExample.MuonRecFlags import muonRecFlags
          muonRecFlags.doCSCs.set_Value_and_Lock = True

       from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
       muonByteStreamFlags.TgcDataType = "atlas"#FIXME should not be setting jobproperties at this point in the configuration.
       muonByteStreamFlags.RpcDataType = "atlas"#FIXME should not be setting jobproperties at this point in the configuration.
       muonByteStreamFlags.MdtDataType = "atlas"#FIXME should not be setting jobproperties at this point in the configuration.

    if overlayFlags.doBkg==True:
       from OverlayCommonAlgs.OverlayCommonAlgsConf import DeepCopyObjects
       job += DeepCopyObjects("BkgRdo4")
       job.BkgRdo4.MuonObjects = True
       
    import MuonCnvExample.MuonCablingConfig

    digitizationFlags.doMuonNoise=False #FIXME should not be setting jobproperties at this point in the configuration.

    if readBS:
       include("MuonCnvExample/MuonReadBS_jobOptions.py")

    if DetFlags.overlay.CSC_on():
        if readBS:
           ToolSvc.CscRawDataProviderTool.EvtStore = "OriginalEvent_SG"
        include ( "CscOverlay/CscOverlay_jobOptions.py" )
        job.CscOverlay.IsByteStream = readBS
        job.CscOverlay.DataStore = "OriginalEvent_SG"
        
        job += CfgGetter.getAlgorithm("CscTruthOverlay")

        #print "ACH123: Setting DEBUG v99"
        #job.CscOverlay.MakeRDOTool.OutputLevel=DEBUG
        #job.CscOverlay.MakeRDOTool.cscCalibTool.OutputLevel=DEBUG
        #job.CscOverlay.OutputLevel=DEBUG
        #MessageSvc.debugLimit = 100000

        #print "ACH123: NumSamples = 2 for MakeRDOTool"
        #job.CscOverlay.MakeRDOTool.NumSamples=2
        
    if DetFlags.overlay.MDT_on():
       # include ( "MdtOverlay/MdtOverlay_jobOptions.py" )
        job += CfgGetter.getAlgorithm("MdtOverlay")
        from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MdtDigitToMdtRDO
        job += MdtDigitToMdtRDO()
        job.MdtDigitToMdtRDO.EvtStore = job.MdtOverlay.OutputStore

        if readBS:
           ToolSvc.MdtRawDataProviderTool.EvtStore = "OriginalEvent_SG"
           job.MdtOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = False

        job += CfgGetter.getAlgorithm("MdtTruthOverlay")

        #job.MdtOverlay.OutputLevel = VERBOSE
        #job.MdtDigitToMdtRDO.OutputLevel = VERBOSE

    if DetFlags.overlay.RPC_on():
        job += CfgGetter.getAlgorithm("RpcOverlay")
        from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import RpcDigitToRpcRDO
        job += RpcDigitToRpcRDO()
        job.RpcDigitToRpcRDO.EvtStore = job.RpcOverlay.OutputStore
        #include ( "RpcOverlay/RpcOverlay_jobOptions.py" )
        #job.RpcOverlay.DataStore = "BkgEvent_2_SG"
        if readBS:
           ToolSvc.RpcRawDataProviderTool.EvtStore = "OriginalEvent_SG"
           job.RpcOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = False 
        #job.RpcOverlay.OutputLevel = VERBOSE
        #job.RpcDigitToRpcRDO.OutputLevel = VERBOSE
        job += CfgGetter.getAlgorithm("RpcTruthOverlay")

    if DetFlags.overlay.TGC_on():
       # include ( "TgcOverlay/TgcOverlay_jobOptions.py" )
        job += CfgGetter.getAlgorithm("TgcOverlay")
        from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import TgcDigitToTgcRDO
        job += TgcDigitToTgcRDO()
        job.TgcDigitToTgcRDO.EvtStore = job.TgcOverlay.OutputStore
        #job.TgcOverlay.DataStore = "BkgEvent_2_SG"
        if readBS:
           ToolSvc.TgcRawDataProviderTool.EvtStore = "OriginalEvent_SG"
           job.TgcOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = False

           # storegate dump
           # StoreGateSvc = Service( "StoreGateSvc" )
           # StoreGateSvc.Dump = True  #true will dump data store contents

           # StoreGateSvc.OutputLevel=DEBUG
        job += CfgGetter.getAlgorithm("TgcTruthOverlay")

            
