# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.DetFlags import DetFlags

from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon import CfgMgr

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
    import MuonCablingConfig
    return CfgMgr.MdtROD_Decoder(name,**kwargs)


def MdtRawDataProviderTool(name="MdtRawDataProviderTool",**kwargs):
    ## TODO: set this value as default in C++
    kwargs.setdefault("Decoder", "MdtROD_Decoder")
    return CfgMgr.Muon__MDT_RawDataProviderTool(name,**kwargs)


#================================================================================
# RPC Bytestream reading setup
#================================================================================
    
def RpcROD_Decoder(name="RpcROD_Decoder",**kwargs):
    # setup cabling service needed by this tool
    import MuonCablingConfig
    return CfgMgr.Muon__RpcROD_Decoder(name,**kwargs)


def RpcRawDataProviderTool(name = "RpcRawDataProviderTool",**kwargs):
    ## TODO: set this value as default in C++
    kwargs.setdefault("Decoder", "RpcROD_Decoder")
    return CfgMgr.Muon__RPC_RawDataProviderTool(name,**kwargs)


#================================================================================
# TGC Bytestream reading setup
#================================================================================

def TgcROD_Decoder(name = "TgcROD_Decoder",**kwargs):
    # setup cabling service needed by this tool
    import MuonCablingConfig
    return CfgMgr.Muon__TGC_RodDecoderReadout(name,**kwargs)


def TgcRawDataProviderTool(name = "TgcRawDataProviderTool",**kwargs):
    ## TODO: set this value as default in C++
    kwargs.setdefault("Decoder", "TgcROD_Decoder")
    return CfgMgr.Muon__TGC_RawDataProviderTool(name,**kwargs)



#================================================================================
# CSC Bytestream reading setup
#================================================================================

def CscROD_Decoder(name = "CscROD_Decoder",**kwargs):
    return CfgMgr.Muon__CscROD_Decoder(name,**kwargs)


def CscRawDataProviderTool(name = "CscRawDataProviderTool",**kwargs):
    ## TODO: set this value as default in C++
    kwargs.setdefault("Decoder", "CscROD_Decoder")
    return CfgMgr.Muon__CSC_RawDataProviderTool(name,**kwargs)
    


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
