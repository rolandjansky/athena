# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr


def getPixelOverlay(name="PixelOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+PixelRDOs");
    kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+PixelRDOs");
    kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+PixelRDOs");

    kwargs.setdefault("includeBkg", True);

    return CfgMgr.PixelOverlay(name, **kwargs)


def getPixelSDOOverlay(name="PixelSDOOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    # We do not need background pixel SDOs
    kwargs.setdefault("BkgInputKey", "");

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "PixelSDO_Map");
        kwargs.setdefault("OutputKey", "PixelSDO_Map");
    else:
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+PixelSDO_Map");
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+PixelSDO_Map");

    return CfgMgr.InDetSDOOverlay(name, **kwargs)


def getSCTOverlay(name="SCTOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("BkgInputKey", "StoreGateSvc+" + overlayFlags.bkgPrefix() + "SCT_RDOs");
        kwargs.setdefault("SignalInputKey", "StoreGateSvc+" + overlayFlags.sigPrefix() + "SCT_RDOs");
        kwargs.setdefault("OutputKey", "StoreGateSvc+SCT_RDOs");
    else:
        kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+SCT_RDOs");
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+SCT_RDOs");
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+SCT_RDOs");

    kwargs.setdefault("includeBkg", True);

    return CfgMgr.SCTOverlay(name, **kwargs)


def getSCTSDOOverlay(name="SCTSDOOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    # We do not need background SCT SDOs
    kwargs.setdefault("BkgInputKey", "");

    if overlayFlags.isOverlayMT():
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "SCT_SDO_Map");
        kwargs.setdefault("OutputKey", "SCT_SDO_Map");
    else:
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+SCT_SDO_Map");
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+SCT_SDO_Map");

    return CfgMgr.InDetSDOOverlay(name, **kwargs)


def getTRTOverlay(name="TRTOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags
    from Digitization.DigitizationFlags import digitizationFlags

    kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+TRT_RDOs");
    kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+TRT_RDOs");
    kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+TRT_RDOs");
    kwargs.setdefault("SignalInputSDOKey", overlayFlags.evtStore() + "+TRT_SDO_Map");

    kwargs.setdefault("includeBkg", True);

    kwargs.setdefault("TRT_LocalOccupancyTool", "TRT_LocalOccupancy")

    # HT hit correction fraction
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrel", 0.110)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcap", 0.090)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionBarrelNoE", 0.060)
    kwargs.setdefault("TRT_HT_OccupancyCorrectionEndcapNoE", 0.050)

    return CfgMgr.TRTOverlay(name, **kwargs)


def getTRTSDOOverlay(name="TRTSDOOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    if overlayFlags.isOverlayMT():
        if overlayFlags.isDataOverlay():
            kwargs.setdefault("BkgInputKey", "")
        else:
            kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "TRT_SDO_Map");
        kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "TRT_SDO_Map");
        kwargs.setdefault("OutputKey", "TRT_SDO_Map");
    else:
        if overlayFlags.isDataOverlay():
            kwargs.setdefault("BkgInputKey", "")
        else:
            kwargs.setdefault("BkgInputKey", overlayFlags.dataStore() + "+TRT_SDO_Map");
        kwargs.setdefault("SignalInputKey", overlayFlags.evtStore() + "+TRT_SDO_Map");
        kwargs.setdefault("OutputKey", overlayFlags.outputStore() + "+TRT_SDO_Map");

    return CfgMgr.InDetSDOOverlay(name, **kwargs)
