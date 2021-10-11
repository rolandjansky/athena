# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getMdtRdoToMdtDigitAlg(name="MdtRdoToMdtDigitAlg", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("MdtRdoContainer", overlayFlags.bkgPrefix() + "MDTCSM")
        kwargs.setdefault("MdtDigitContainer", overlayFlags.bkgPrefix() + "MDT_DIGITS")
    else:
        kwargs.setdefault("EvtStore", overlayFlags.dataStore())
        kwargs.setdefault("MdtRdoContainer", overlayFlags.dataStore()+"+MDTCSM")
        kwargs.setdefault("MdtDigitContainer", overlayFlags.dataStore()+"+MDT_DIGITS")
    return CfgMgr.MdtRdoToMdtDigit(name, **kwargs)


def getRpcRdoToRpcDigitAlg(name="RpcRdoToRpcDigitAlg", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("RpcRdoContainer", overlayFlags.bkgPrefix() + "RPCPAD")
        kwargs.setdefault("RpcDigitContainer", overlayFlags.bkgPrefix() + "RPC_DIGITS")
    else:
        kwargs.setdefault("EvtStore", overlayFlags.dataStore())
        kwargs.setdefault("RpcRdoContainer", overlayFlags.dataStore()+"+RPCPAD")
        kwargs.setdefault("RpcDigitContainer", overlayFlags.dataStore()+"+RPC_DIGITS")
    return CfgMgr.RpcRdoToRpcDigit(name, **kwargs)


def getTgcRdoToTgcDigitAlg(name="TgcRdoToTgcDigitAlg", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("TgcRdoContainer", overlayFlags.bkgPrefix() + "TGCRDO")
        kwargs.setdefault("TgcDigitContainer", overlayFlags.bkgPrefix() + "TGC_DIGITS")
    else:
        kwargs.setdefault("EvtStore", overlayFlags.dataStore())
        kwargs.setdefault("TgcRdoContainer", overlayFlags.dataStore()+"+TGCRDO")
        kwargs.setdefault("TgcDigitContainer", overlayFlags.dataStore()+"+TGC_DIGITS")
    return CfgMgr.TgcRdoToTgcDigit(name, **kwargs)


def getSTGC_RdoToDigitAlg(name="STGC_RdoToDigitAlg", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("sTgcRdoContainer", overlayFlags.bkgPrefix() + "sTGCRDO")
        kwargs.setdefault("sTgcDigitContainer", overlayFlags.bkgPrefix() + "sTGC_DIGITS")
    else:
        kwargs.setdefault("EvtStore", overlayFlags.dataStore())
        kwargs.setdefault("sTgcRdoContainer", overlayFlags.dataStore()+"+sTGCRDO")
        kwargs.setdefault("sTgcDigitContainer", overlayFlags.dataStore()+"+sTGC_DIGITS")
    return CfgMgr.STGC_RdoToDigit(name, **kwargs)


def getMM_RdoToDigitAlg(name="MM_RdoToDigitAlg", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("MmRdoContainer", overlayFlags.bkgPrefix() + "MMRDO")
        kwargs.setdefault("MmDigitContainer", overlayFlags.bkgPrefix() + "MM_DIGITS")
    else:
        kwargs.setdefault("EvtStore", overlayFlags.dataStore())
        kwargs.setdefault("MmRdoContainer", overlayFlags.dataStore()+"+MMRDO")
        kwargs.setdefault("MmDigitContainer", overlayFlags.dataStore()+"+MM_DIGITS")
    return CfgMgr.MM_RdoToDigit(name, **kwargs)


def getMdtDigitToMdtRDO(name="MdtDigitToMdtRDO", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.PileUpPresampling and 'LegacyOverlay' not in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputObjectName", overlayFlags.bkgPrefix() + "MDTCSM")
    else:
        kwargs.setdefault("OutputObjectName", "MDTCSM")
    return CfgMgr.MdtDigitToMdtRDO(name, **kwargs)

def getRpcDigitToRpcRDO(name="RpcDigitToRpcRDO", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.PileUpPresampling and 'LegacyOverlay' not in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputObjectName", overlayFlags.bkgPrefix() + "RPCPAD")
    else:
        kwargs.setdefault("OutputObjectName", "RPCPAD")
    return CfgMgr.RpcDigitToRpcRDO(name, **kwargs)

def getTgcDigitToTgcRDO(name="TgcDigitToTgcRDO", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.PileUpPresampling and 'LegacyOverlay' not in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputObjectName", overlayFlags.bkgPrefix() + "TGCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "TGCRDO")
    return CfgMgr.TgcDigitToTgcRDO(name, **kwargs)


def getSTGC_DigitToRDO(name="STGC_DigitToRDO", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.PileUpPresampling and 'LegacyOverlay' not in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputObjectName", overlayFlags.bkgPrefix() + "sTGCRDO")
    else:
        kwargs.setdefault("OutputObjectName", "sTGCRDO")
    return CfgMgr.STGC_DigitToRDO(name, **kwargs)


def getMM_DigitToRDO(name="MM_DigitToRDO", **kwargs):
    from Digitization.DigitizationFlags import digitizationFlags
    if digitizationFlags.PileUpPresampling and 'LegacyOverlay' not in digitizationFlags.experimentalDigi():
        from OverlayCommonAlgs.OverlayFlags import overlayFlags
        kwargs.setdefault("OutputObjectName", overlayFlags.bkgPrefix() + "MMRDO")
    else:
        kwargs.setdefault("OutputObjectName", "MMRDO")
    return CfgMgr.MM_DigitToRDO(name, **kwargs)


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
