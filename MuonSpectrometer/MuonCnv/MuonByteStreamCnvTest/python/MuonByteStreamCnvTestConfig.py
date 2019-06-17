# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getMdtRdoToMdtDigitAlg(name="MdtRdoToMdtDigitOverlayAlg", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("MdtRdoContainer", overlayFlags.bkgPrefix() + "MDTCSM")
        kwargs.setdefault("MdtDigitContainer", overlayFlags.bkgPrefix() + "MDT_DIGITS")
    else:
        kwargs.setdefault("EvtStore", overlayFlags.dataStore())
        kwargs.setdefault("MdtRdoContainer", overlayFlags.dataStore()+"+MDTCSM")
        kwargs.setdefault("MdtDigitContainer", overlayFlags.dataStore()+"+MDT_DIGITS")
    return CfgMgr.MdtRdoToMdtDigit(name, **kwargs)


def getRpcRdoToRpcDigitAlg(name="RpcRdoToRpcDigitOverlayAlg", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("RpcRdoContainer", overlayFlags.bkgPrefix() + "RPCPAD")
        kwargs.setdefault("RpcDigitContainer", overlayFlags.bkgPrefix() + "RPC_DIGITS")
    else:
        kwargs.setdefault("EvtStore", overlayFlags.dataStore())
        kwargs.setdefault("RpcRdoContainer", overlayFlags.dataStore()+"+RPCPAD")
        kwargs.setdefault("RpcDigitContainer", overlayFlags.dataStore()+"+RPC_DIGITS")
    return CfgMgr.RpcRdoToRpcDigit(name, **kwargs)


def getTgcRdoToTgcDigitAlg(name="TgcRdoToTgcDigitOverlayAlg", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("TgcRdoContainer", overlayFlags.bkgPrefix() + "TGCRDO")
        kwargs.setdefault("TgcDigitContainer", overlayFlags.bkgPrefix() + "TGC_DIGITS")
    else:
        kwargs.setdefault("EvtStore", overlayFlags.dataStore())
        kwargs.setdefault("TgcRdoContainer", overlayFlags.dataStore()+"+TGCRDO")
        kwargs.setdefault("TgcDigitContainer", overlayFlags.dataStore()+"+TGC_DIGITS")
    return CfgMgr.TgcRdoToTgcDigit(name, **kwargs)


def getMdtDigitToMdtRDO(name="MdtDigitToMdtRDO", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.PileUpPremixing and 'OverlayMT' in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputObjectName", overlayFlags.bkgPrefix() + "MDTCSM")
    else:
        kwargs.setdefault("OutputObjectName", "MDTCSM")
    return CfgMgr.MdtDigitToMdtRDO(name, **kwargs)

def getRpcDigitToRpcRDO(name="RpcDigitToRpcRDO", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.PileUpPremixing and 'OverlayMT' in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputObjectName", overlayFlags.bkgPrefix() + "RPCPAD")
    else:
        kwargs.setdefault("OutputObjectName", "RPCPAD")
    return CfgMgr.RpcDigitToRpcRDO(name, **kwargs)

def getTgcDigitToTgcRDO(name="TgcDigitToTgcRDO", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.PileUpPremixing and 'OverlayMT' in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputObjectName", overlayFlags.bkgPrefix() + "TGCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "TGCRDO")
    return CfgMgr.TgcDigitToTgcRDO(name, **kwargs)


def getSigMdtDigitToMdtRDO(name="SigMdtDigitToMdtRDO", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputObjectName",overlayFlags.evtStore()+"+MDT_DIGITS")
    kwargs.setdefault("OutputObjectName",overlayFlags.evtStore()+"+MDTCSM")
    return CfgMgr.MdtDigitToMdtRDO(name, **kwargs)


def getSigRpcDigitToRpcRDO(name="SigRpcDigitToRpcRDO", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputObjectName",overlayFlags.evtStore()+"+RPC_DIGITS")
    kwargs.setdefault("OutputObjectName",overlayFlags.evtStore()+"+RPCPAD")
    return CfgMgr.RpcDigitToRpcRDO(name, **kwargs)


def getSigTgcDigitToTgcRDO(name="SigTgcDigitToTgcRDO", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputObjectName",overlayFlags.evtStore()+"+TGC_DIGITS")
    kwargs.setdefault("OutputObjectName",overlayFlags.evtStore()+"+TGCRDO")
    return CfgMgr.TgcDigitToTgcRDO(name, **kwargs)


def getOverlayCscDigitToCscRDO(name="OverlayCscDigitToCscRDO", **kwargs):
    kwargs.setdefault("CscDigitToRDOTool","CscDigitToCscRDOTool4")
    return CfgMgr.CscDigitToCscRDO(name, **kwargs)


def getOverlayMdtDigitToMdtRDO(name="OverlayMdtDigitToMdtRDO", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputObjectName",overlayFlags.outputStore()+"+MDT_DIGITS")
    kwargs.setdefault("OutputObjectName",overlayFlags.outputStore()+"+MDTCSM")
    return CfgMgr.MdtDigitToMdtRDO(name, **kwargs)


def getOverlayRpcDigitToRpcRDO(name="OverlayRpcDigitToRpcRDO", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputObjectName",overlayFlags.outputStore()+"+RPC_DIGITS")
    kwargs.setdefault("OutputObjectName",overlayFlags.outputStore()+"+RPCPAD")
    return CfgMgr.RpcDigitToRpcRDO(name, **kwargs)


def getOverlayTgcDigitToTgcRDO(name="OverlayTgcDigitToTgcRDO", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("InputObjectName",overlayFlags.outputStore()+"+TGC_DIGITS")
    kwargs.setdefault("OutputObjectName",overlayFlags.outputStore()+"+TGCRDO")
    return CfgMgr.TgcDigitToTgcRDO(name, **kwargs)
