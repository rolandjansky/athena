# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getTTL1Overlay(name="OverlayTTL1", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    # Tile TTL1 overlay is currently not needed as Tile trigger towers are created from the overlaid container
    enableTileTTL1Overlay = False

    kwargs.setdefault("BkgEmTTL1Key", overlayFlags.dataStore() + "+LArTTL1EM");
    kwargs.setdefault("SignalEmTTL1Key", overlayFlags.evtStore() + "+LArTTL1EM");
    kwargs.setdefault("OutputEmTTL1Key", overlayFlags.outputStore() + "+LArTTL1EM");

    kwargs.setdefault("BkgHadTTL1Key", overlayFlags.dataStore() + "+LArTTL1HAD");
    kwargs.setdefault("SignalHadTTL1Key", overlayFlags.evtStore() + "+LArTTL1HAD");
    kwargs.setdefault("OutputHadTTL1Key", overlayFlags.outputStore() + "+LArTTL1HAD");

    if enableTileTTL1Overlay:
        kwargs.setdefault("BkgTileTTL1Key", overlayFlags.dataStore() + "+TileTTL1Cnt");
        kwargs.setdefault("SignalTileTTL1Key", overlayFlags.evtStore() + "+TileTTL1Cnt");
        kwargs.setdefault("OutputTileTTL1Key", overlayFlags.outputStore() + "+TileTTL1Cnt");
    else:
        kwargs.setdefault("BkgTileTTL1Key", "");
        kwargs.setdefault("SignalTileTTL1Key", "");
        kwargs.setdefault("OutputTileTTL1Key", "");

    return CfgMgr.LVL1__OverlayTTL1(name, **kwargs)
