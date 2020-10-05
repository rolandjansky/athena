# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.DetFlags import DetFlags
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr
from OverlayCommonAlgs.OverlayFlags import overlayFlags

if DetFlags.readRDOBS.Muon_on():
    # the following 'if' is crucial to not break online, which user their own ByteStreamCnvSvc
    if not hasattr(ServiceMgr,"ByteStreamCnvSvc"):
        from ByteStreamCnvSvc import ReadByteStream

    if not hasattr( ServiceMgr, "ByteStreamAddressProviderSvc" ):
        from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
        ServiceMgr += ByteStreamAddressProviderSvc()

#================================================================================
# MDT Bytestream reading setup
#================================================================================

def MdtROD_Decoder(name="MdtROD_Decoder",**kwargs):
    # setup cabling service needed by this tool
    from . import MuonCablingConfig
    return CfgMgr.MdtROD_Decoder(name,**kwargs)


def MdtRawDataProviderTool(name="MdtRawDataProviderTool",**kwargs):
    kwargs.setdefault("Decoder", "MdtROD_Decoder")
    if DetFlags.overlay.MDT_on() and overlayFlags.isDataOverlay():
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("RdoLocation",overlayFlags.bkgPrefix()+"MDTCSM")
        else:
            kwargs.setdefault("RdoLocation",overlayFlags.dataStore()+"+MDTCSM")
    return CfgMgr.Muon__MDT_RawDataProviderToolMT(name,**kwargs)


#================================================================================
# RPC Bytestream reading setup
#================================================================================
    
def RpcROD_Decoder(name="RpcROD_Decoder",**kwargs):
    # setup cabling service needed by this tool
    from . import MuonCablingConfig
    return CfgMgr.Muon__RpcROD_Decoder(name,**kwargs)


def RpcRawDataProviderTool(name = "RpcRawDataProviderTool",**kwargs):
    kwargs.setdefault("Decoder", "RpcROD_Decoder")
    if DetFlags.overlay.RPC_on() and overlayFlags.isDataOverlay():
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("RdoLocation", overlayFlags.bkgPrefix()+"RPCPAD")
        else:
            kwargs.setdefault("RdoLocation", overlayFlags.dataStore()+"+RPCPAD")
    return CfgMgr.Muon__RPC_RawDataProviderToolMT(name,**kwargs)


#================================================================================
# TGC Bytestream reading setup
#================================================================================

def TgcROD_Decoder(name = "TgcROD_Decoder",**kwargs):
    # setup cabling service needed by this tool
    from . import MuonCablingConfig
    return CfgMgr.Muon__TGC_RodDecoderReadout(name,**kwargs)


def TgcRawDataProviderTool(name = "TgcRawDataProviderTool",**kwargs):
    kwargs.setdefault("Decoder", "TgcROD_Decoder")
    if DetFlags.overlay.TGC_on() and overlayFlags.isDataOverlay():
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("RdoLocation", overlayFlags.bkgPrefix()+"TGCRDO")
        else:
            kwargs.setdefault("RdoLocation", overlayFlags.dataStore()+"+TGCRDO")
    return CfgMgr.Muon__TGC_RawDataProviderToolMT(name,**kwargs)



#================================================================================
# CSC Bytestream reading setup
#================================================================================

def CscROD_Decoder(name = "CscROD_Decoder",**kwargs):
    return CfgMgr.Muon__CscROD_Decoder(name,**kwargs)


def CscRawDataProviderTool(name = "CscRawDataProviderTool",**kwargs):
    kwargs.setdefault("Decoder", "CscROD_Decoder")
    if DetFlags.overlay.CSC_on() and overlayFlags.isDataOverlay():
        if overlayFlags.isOverlayMT():
            kwargs.setdefault("RdoLocation", overlayFlags.bkgPrefix()+"CSCRDO")
        else:
            kwargs.setdefault("RdoLocation", overlayFlags.dataStore()+"+CSCRDO")
    return CfgMgr.Muon__CSC_RawDataProviderToolMT(name,**kwargs)
    

#
# For backwards compat - TO BE REMOVED as soon as all clients get these tools via AthenaCommon.CfgGetter
#
from AthenaCommon.CfgGetter import getPublicTool
if DetFlags.readRDOBS.MDT_on():
    MuonMdtRawDataProviderTool = getPublicTool("MdtRawDataProviderTool")
else:
    MuonMdtRawDataProviderTool = None

if DetFlags.readRDOBS.RPC_on():
    MuonRpcRawDataProviderTool = getPublicTool("RpcRawDataProviderTool")
else:
    MuonRpcRawDataProviderTool = None

if DetFlags.readRDOBS.TGC_on():
    MuonTgcRawDataProviderTool = getPublicTool("TgcRawDataProviderTool")
else:
    MuonTgcRawDataProviderTool = None

if DetFlags.readRDOBS.CSC_on():
    MuonCscRawDataProviderTool = getPublicTool("CscRawDataProviderTool")
else:
    MuonCscRawDataProviderTool = None
