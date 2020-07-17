# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getTTL1Overlay(name="OverlayTTL1", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    if overlayFlags.isOverlayMT():
        kwargs.setdefault("BkgEmTTL1Key", overlayFlags.bkgPrefix() + "LArTTL1EM")
        kwargs.setdefault("SignalEmTTL1Key", overlayFlags.sigPrefix() + "LArTTL1EM")
        kwargs.setdefault("OutputEmTTL1Key", "LArTTL1EM")
        kwargs.setdefault("BkgHadTTL1Key", overlayFlags.bkgPrefix() + "LArTTL1HAD")
        kwargs.setdefault("SignalHadTTL1Key", overlayFlags.sigPrefix() + "LArTTL1HAD")
        kwargs.setdefault("OutputHadTTL1Key", "LArTTL1HAD")
        kwargs.setdefault("BkgTileTTL1Key", overlayFlags.bkgPrefix() + "TileTTL1Cnt")
        kwargs.setdefault("SignalTileTTL1Key", overlayFlags.sigPrefix() + "TileTTL1Cnt")
        kwargs.setdefault("OutputTileTTL1Key", "TileTTL1Cnt")
        kwargs.setdefault("BkgTileMBTSTTL1Key", overlayFlags.bkgPrefix() + "TileTTL1MBTS")
        kwargs.setdefault("SignalTileMBTSTTL1Key", overlayFlags.sigPrefix() + "TileTTL1MBTS")
        kwargs.setdefault("OutputTileMBTSTTL1Key", "TileTTL1MBTS")
    else:
        kwargs.setdefault("BkgEmTTL1Key", overlayFlags.dataStore() + "+LArTTL1EM")
        kwargs.setdefault("SignalEmTTL1Key", overlayFlags.evtStore() + "+LArTTL1EM")
        kwargs.setdefault("OutputEmTTL1Key", overlayFlags.outputStore() + "+LArTTL1EM")
        kwargs.setdefault("BkgHadTTL1Key", overlayFlags.dataStore() + "+LArTTL1HAD")
        kwargs.setdefault("SignalHadTTL1Key", overlayFlags.evtStore() + "+LArTTL1HAD")
        kwargs.setdefault("OutputHadTTL1Key", overlayFlags.outputStore() + "+LArTTL1HAD")
        kwargs.setdefault("BkgTileTTL1Key", overlayFlags.dataStore() + "+TileTTL1Cnt")
        kwargs.setdefault("SignalTileTTL1Key", overlayFlags.evtStore() + "+TileTTL1Cnt")
        kwargs.setdefault("OutputTileTTL1Key", overlayFlags.outputStore() + "+TileTTL1Cnt")
        kwargs.setdefault("BkgTileMBTSTTL1Key", overlayFlags.dataStore() + "+TileTTL1MBTS")
        kwargs.setdefault("SignalTileMBTSTTL1Key", overlayFlags.evtStore() + "+TileTTL1MBTS")
        kwargs.setdefault("OutputTileMBTSTTL1Key", overlayFlags.outputStore() + "+TileTTL1MBTS")

    return CfgMgr.LVL1__OverlayTTL1(name, **kwargs)
