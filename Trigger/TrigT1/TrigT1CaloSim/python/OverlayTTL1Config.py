# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getTTL1Overlay(name="OverlayTTL1", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    kwargs.setdefault("EmTTL1MainKey", overlayFlags.dataStore() + "/LArTTL1EM");
    kwargs.setdefault("EmTTL1OverlayKey", overlayFlags.evtStore() + "/LArTTL1EM");
    kwargs.setdefault("EmTTL1OutputKey", overlayFlags.outputStore() + "/LArTTL1EM");

    kwargs.setdefault("HadTTL1MainKey", overlayFlags.dataStore() + "/LArTTL1HAD");
    kwargs.setdefault("HadTTL1OverlayKey", overlayFlags.evtStore() + "/LArTTL1HAD");
    kwargs.setdefault("HadTTL1OutputKey", overlayFlags.outputStore() + "/LArTTL1HAD");

    kwargs.setdefault("TileTTL1MainKey", overlayFlags.dataStore() + "/TileTTL1Cnt");
    kwargs.setdefault("TileTTL1OverlayKey", overlayFlags.evtStore() + "/TileTTL1Cnt");
    kwargs.setdefault("TileTTL1OutputKey", overlayFlags.outputStore() + "/TileTTL1Cnt");

    kwargs.setdefault("TileMBTSTTL1MainKey", overlayFlags.dataStore() + "/TileTTL1MBTS");
    kwargs.setdefault("TileMBTSTTL1OverlayKey", overlayFlags.evtStore() + "/TileTTL1MBTS");
    kwargs.setdefault("TileMBTSTTL1OutputKey", overlayFlags.outputStore() + "/TileTTL1MBTS");

    return CfgMgr.LVL1__OverlayTTL1(name, **kwargs)
