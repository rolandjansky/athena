include.block ( "EventOverlayJobTransforms/MuonOverlay_jobOptions.py" )

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from Digitization.DigitizationFlags import digitizationFlags
from OverlayCommonAlgs.OverlayFlags import OverlayFlags
from MuonRecExample.MuonRecFlags import muonRecFlags,mooreFlags,muonboyFlags

from RecExConfig.RecFlags import rec as recFlags

if OverlayFlags.doMDT() or OverlayFlags.doCSC() or OverlayFlags.doRPC() or OverlayFlags.doTGC():
   
    include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )
 
    import MuonRecExample.MuonReadCalib
    if readBS and isRealData:
       theApp.Dlls += [ "MuonByteStream"]
       if OverlayFlags.doCSC():
          from MuonRecExample.MuonRecFlags import muonRecFlags
          muonRecFlags.doCSCs.set_Value_and_Lock = True

       from MuonByteStream.MuonByteStreamFlags import muonByteStreamFlags
       muonByteStreamFlags.TgcDataType = "atlas"#FIXME should not be setting jobproperties at this point in the configuration.
       muonByteStreamFlags.RpcDataType = "atlas"#FIXME should not be setting jobproperties at this point in the configuration.
       muonByteStreamFlags.MdtDataType = "atlas"#FIXME should not be setting jobproperties at this point in the configuration.

    if OverlayFlags.doBkg():
       from OverlayCommonAlgs.OverlayCommonAlgsConf import DeepCopyObjects
       job += DeepCopyObjects("BkgRdo4")
       job.BkgRdo4.MuonObjects = True
       
    import MuonCnvExample.MuonCablingConfig

    digitizationFlags.doMuonNoise=False #FIXME should not be setting jobproperties at this point in the configuration.

    if readBS:
       include("MuonRecExample/MuonReadBS_jobOptions.py")

    if OverlayFlags.doCSC():
        if readBS:
           ToolSvc.CscRawDataProviderTool.EvtStore = "OriginalEvent_SG"
        include ( "CscOverlay/CscOverlay_jobOptions.py" )
        job.CscOverlay.IsByteStream = readBS
        job.CscOverlay.DataStore = "OriginalEvent_SG"

        #print "ACH123: Setting DEBUG v99"
        #job.CscOverlay.MakeRDOTool.OutputLevel=DEBUG
        #job.CscOverlay.MakeRDOTool.cscCalibTool.OutputLevel=DEBUG
        #job.CscOverlay.OutputLevel=DEBUG
        #MessageSvc.debugLimit = 100000

        #print "ACH123: NumSamples = 2 for MakeRDOTool"
        #job.CscOverlay.MakeRDOTool.NumSamples=2
        
    if OverlayFlags.doMDT():
        if not hasattr(ToolSvc, 'MdtDigitizationTool'):
            from AthenaCommon import CfgGetter
            ToolSvc += CfgGetter.getPrivateTool("MdtDigitizationTool", checkType=True)
        ToolSvc.MdtDigitizationTool.GetT0FromBD = True
        
        include ( "MdtOverlay/MdtOverlay_jobOptions.py" )
        #job.MdtOverlay.DataStore = "OriginalEvent_SG"
        if readBS:
           ToolSvc.MdtRawDataProviderTool.EvtStore = "OriginalEvent_SG"
           job.MdtOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = False

        #job.MdtOverlay.OutputLevel = VERBOSE
        #job.MdtDigitToMdtRDO.OutputLevel = VERBOSE

    if OverlayFlags.doRPC():
        include ( "RpcOverlay/RpcOverlay_jobOptions.py" )
        #job.RpcOverlay.DataStore = "BkgEvent_2_SG"
        if readBS:
           ToolSvc.RpcRawDataProviderTool.EvtStore = "OriginalEvent_SG"
           job.RpcOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = False 
        #job.RpcOverlay.OutputLevel = VERBOSE
        #job.RpcDigitToRpcRDO.OutputLevel = VERBOSE

    if OverlayFlags.doTGC():
        include ( "TgcOverlay/TgcOverlay_jobOptions.py" )
        #job.TgcOverlay.DataStore = "BkgEvent_2_SG"
        if readBS:
           ToolSvc.TgcRawDataProviderTool.EvtStore = "OriginalEvent_SG"
           job.TgcOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = False

           # storegate dump
           # StoreGateSvc = Service( "StoreGateSvc" )
           # StoreGateSvc.Dump = True  #true will dump data store contents

           # StoreGateSvc.OutputLevel=DEBUG

            
