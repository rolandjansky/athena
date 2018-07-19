# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getMdtRdoToMdtDigit(name="MdtRdoToMdtDigitOverlay", **kwargs):
    kwargs.setdefault("DecodeMdtRDO", True)
    kwargs.setdefault("DecodeRpcRDO", False)
    kwargs.setdefault("DecodeTgcRDO", False)
    kwargs.setdefault("DecodeCscRDO", False)
    kwargs.setdefault("RetrievePrivateCopy", True)
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.dataStore())
    return CfgMgr.MuonRdoToMuonDigitTool(name, **kwargs)


def getRpcRdoToRpcDigit(name="RpcRdoToRpcDigitOverlay", **kwargs):
    kwargs.setdefault("DecodeMdtRDO", False)
    kwargs.setdefault("DecodeRpcRDO", True)
    kwargs.setdefault("DecodeTgcRDO", False)
    kwargs.setdefault("DecodeCscRDO", False)
    kwargs.setdefault("RetrievePrivateCopy", True)
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.dataStore())
    return CfgMgr.MuonRdoToMuonDigitTool(name, **kwargs)


def getTgcRdoToTgcDigit(name="TgcRdoToTgcDigitOverlay", **kwargs):
    kwargs.setdefault("DecodeMdtRDO", False)
    kwargs.setdefault("DecodeRpcRDO", False)
    kwargs.setdefault("DecodeTgcRDO", True)
    kwargs.setdefault("DecodeCscRDO", False)
    kwargs.setdefault("RetrievePrivateCopy", True)
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    kwargs.setdefault("EvtStore", overlayFlags.dataStore())
    return CfgMgr.MuonRdoToMuonDigitTool(name, **kwargs)


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
