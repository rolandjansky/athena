include.block ( "EventOverlayJobTransforms/MuonOverlay_jobOptions.py" )

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import ToolSvc
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.DetFlags import DetFlags
from OverlayCommonAlgs.OverlayFlags import overlayFlags
from AthenaCommon import CfgGetter
from RecExConfig.RecFlags import rec as recFlags

if DetFlags.overlay.MDT_on() or DetFlags.overlay.CSC_on() or DetFlags.overlay.RPC_on() or DetFlags.overlay.TGC_on() or DetFlags.overlay.sTGC_on() or DetFlags.overlay.MM_on():

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

    import MuonCnvExample.MuonCablingConfig

    digitizationFlags.doMuonNoise=False #FIXME should not be setting jobproperties at this point in the configuration.

    if overlayFlags.isDataOverlay():
        include("MuonCnvExample/MuonReadBS_jobOptions.py")

    if DetFlags.overlay.CSC_on():
        job += CfgGetter.getAlgorithm("CscOverlayDigitBuilder")
        job += CfgGetter.getAlgorithm("CscDigitToCscRDO4")
        job += CfgGetter.getAlgorithm("CscOverlay")
        if not overlayFlags.isDataOverlay():
            job += CfgGetter.getAlgorithm("CscTruthOverlay")

    if DetFlags.overlay.MDT_on():
        job += CfgGetter.getAlgorithm("MdtRdoToMdtDigitAlg")
        job += CfgGetter.getAlgorithm("MDT_OverlayDigitizer")
        job += CfgGetter.getAlgorithm("MdtOverlay")
        if not overlayFlags.isDataOverlay():
            job += CfgGetter.getAlgorithm("MdtTruthOverlay")
        job += CfgGetter.getAlgorithm("MdtDigitToMdtRDO")

    if DetFlags.overlay.sTGC_on():
        job += CfgGetter.getAlgorithm("STGC_RdoToDigitAlg")
        job += CfgGetter.getAlgorithm("STGC_OverlayDigitizer")
        job += CfgGetter.getAlgorithm("STGC_Overlay")
        if not overlayFlags.isDataOverlay():
            job += CfgGetter.getAlgorithm("STGC_TruthOverlay")
        job += CfgGetter.getAlgorithm("STGC_DigitToRDO")

    if DetFlags.overlay.MM_on():
        job += CfgGetter.getAlgorithm("MM_RdoToDigitAlg")
        job += CfgGetter.getAlgorithm("MM_OverlayDigitizer")
        job += CfgGetter.getAlgorithm("MM_Overlay")
        if not overlayFlags.isDataOverlay():
            job += CfgGetter.getAlgorithm("MM_TruthOverlay")
        job += CfgGetter.getAlgorithm("MM_DigitToRDO")

    if DetFlags.overlay.RPC_on():
        job += CfgGetter.getAlgorithm("RpcRdoToRpcDigitAlg")
        job += CfgGetter.getAlgorithm("RPC_OverlayDigitizer")
        job += CfgGetter.getAlgorithm("RpcOverlay")
        if not overlayFlags.isDataOverlay():
            job += CfgGetter.getAlgorithm("RpcTruthOverlay")
        job += CfgGetter.getAlgorithm("RpcDigitToRpcRDO")

    if DetFlags.overlay.TGC_on():
        job += CfgGetter.getAlgorithm("TgcRdoToTgcDigitAlg")
        job += CfgGetter.getAlgorithm("TGC_OverlayDigitizer")
        job += CfgGetter.getAlgorithm("TgcOverlay")
        if not overlayFlags.isDataOverlay():
            job += CfgGetter.getAlgorithm("TgcTruthOverlay")
        job += CfgGetter.getAlgorithm("TgcDigitToTgcRDO")

    # storegate dump
    #StoreGateSvc = Service( "StoreGateSvc" )
    #StoreGateSvc.Dump = True  #true will dump data store contents
    #StoreGateSvc.OutputLevel=DEBUG
