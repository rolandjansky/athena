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
