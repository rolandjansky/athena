include.block ( "EventOverlayJobTransforms/MuonOverlay_jobOptions.py" )

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags
from AthenaCommon import CfgGetter
from AthenaCommon.CfgGetter import getAlgorithm

from RecExConfig.RecFlags import rec as recFlags

if DetFlags.overlay.MDT_on() or DetFlags.overlay.CSC_on() or DetFlags.overlay.RPC_on() or DetFlags.overlay.TGC_on():

    include( "MuonEventAthenaPool/MuonEventAthenaPool_joboptions.py" )

    import MuonRecExample.MuonReadCalib
    if overlayFlags.isDataOverlay():
        theApp.Dlls += [ "MuonByteStream"]
        if DetFlags.overlay.CSC_on():
            from MuonRecExample.MuonRecFlags import muonRecFlags
            muonRecFlags.doCSCs.set_Value_and_Lock = True #FIXME should not be setting jobproperties at this point in the configuration.

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

    if overlayFlags.isDataOverlay():
        include("MuonCnvExample/MuonReadBS_jobOptions.py")

    if DetFlags.overlay.CSC_on():
        if overlayFlags.isDataOverlay():
            ToolSvc.CscRawDataProviderTool.RdoLocation = overlayFlags.dataStore()+"+CSCRDO"
        job += getAlgorithm("CscOverlay")

        #job.CscOverlay.OutputLevel=VERBOSE
        #svcMgr.MessageSvc.defaultLimit=100000
        #print job.CscOverlay

        #print "ACH123: Setting DEBUG v99"
        #job.CscOverlay.MakeRDOTool.OutputLevel=DEBUG
        #job.CscOverlay.MakeRDOTool.cscCalibTool.OutputLevel=DEBUG
        #job.CscOverlay.OutputLevel=DEBUG
        #MessageSvc.debugLimit = 100000

        #print "ACH123: NumSamples = 2 for MakeRDOTool"
        #job.CscOverlay.MakeRDOTool.NumSamples=2

    if DetFlags.overlay.MDT_on():
        job += CfgGetter.getAlgorithm("MdtOverlay")
        job += CfgGetter.getAlgorithm("OverlayMdtDigitToMdtRDO")
        if overlayFlags.isDataOverlay():
            ToolSvc.MdtRawDataProviderTool.RdoLocation = overlayFlags.dataStore()+"+MDTCSM"
            job.MdtOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = False
        #job.MdtOverlay.OutputLevel = VERBOSE
        #job.OverlayMdtDigitToMdtRDO.OutputLevel = VERBOSE

    if DetFlags.overlay.RPC_on():
        job += CfgGetter.getAlgorithm("RpcOverlay")
        job += CfgGetter.getAlgorithm("OverlayRpcDigitToRpcRDO")
        if overlayFlags.isDataOverlay():
            ToolSvc.RpcRawDataProviderTool.RdoLocation = overlayFlags.dataStore()+"+RPCPAD"
            job.RpcOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = False
        #job.RpcOverlay.OutputLevel = VERBOSE
        #job.OverlayRpcDigitToRpcRDO.OutputLevel = VERBOSE

    if DetFlags.overlay.TGC_on():
        job += CfgGetter.getAlgorithm("TgcOverlay")
        job += CfgGetter.getAlgorithm("OverlayTgcDigitToTgcRDO")
        if overlayFlags.isDataOverlay():
            ToolSvc.TgcRawDataProviderTool.RdoLocation = overlayFlags.dataStore()+"+TGCRDO"
            job.TgcOverlay.ConvertRDOToDigitTool.RetrievePrivateCopy = False
        #job.TgcOverlay.OutputLevel = VERBOSE
        #job.OverlayTgcDigitToRpcRDO.OutputLevel = VERBOSE

        # storegate dump
        # StoreGateSvc = Service( "StoreGateSvc" )
        # StoreGateSvc.Dump = True  #true will dump data store contents

        # StoreGateSvc.OutputLevel=DEBUG
