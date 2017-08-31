# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getInDetOverlay(name="InDetOverlay", **kwargs):
    from AthenaCommon.DetFlags import DetFlags
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    kwargs.setdefault("do_TRT", DetFlags.overlay.TRT_on());
    kwargs.setdefault("do_TRT_background", DetFlags.overlay.TRT_on());
    kwargs.setdefault("mainInputTRTKey", overlayFlags.dataStore() + "+TRT_RDOs");
    kwargs.setdefault("overlayInputTRTKey", overlayFlags.evtStore() + "+TRT_RDOs");
    kwargs.setdefault("mainOutputTRTKey", overlayFlags.outputStore() + "+TRT_RDOs");
    kwargs.setdefault("do_SCT", DetFlags.overlay.SCT_on());
    kwargs.setdefault("do_SCT_background", DetFlags.overlay.SCT_on());
    kwargs.setdefault("mainInputSCTKey", overlayFlags.dataStore() + "+SCT_RDOs");
    kwargs.setdefault("overlayInputSCTKey", overlayFlags.evtStore() + "+SCT_RDOs");
    kwargs.setdefault("mainOutputSCTKey", overlayFlags.outputStore() + "+SCT_RDOs");
    kwargs.setdefault("do_Pixel", DetFlags.overlay.pixel_on());
    kwargs.setdefault("do_Pixel_background", DetFlags.overlay.pixel_on());
    kwargs.setdefault("mainInputPixelKey", overlayFlags.dataStore() + "+PixelRDOs");
    kwargs.setdefault("overlayInputPixelKey", overlayFlags.evtStore() + "+PixelRDOs");
    kwargs.setdefault("mainOutputPixelKey", overlayFlags.outputStore() + "+PixelRDOs");

    return CfgMgr.InDetOverlay(name, **kwargs)


def getInDetSDOOverlay(name="InDetSDOOverlay", **kwargs):
    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.DetFlags import DetFlags
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    kwargs.setdefault("do_TRT", DetFlags.overlay.TRT_on());
    kwargs.setdefault("do_TRT_background", globalflags.DataSource != "data");
    kwargs.setdefault("mainInputTRTKey", overlayFlags.dataStore() + "+TRT_SDO_Map");
    kwargs.setdefault("overlayInputTRTKey", overlayFlags.evtStore() + "+TRT_SDO_Map");
    kwargs.setdefault("mainOutputTRTKey", overlayFlags.outputStore() + "+TRT_SDO_Map");
    kwargs.setdefault("do_SCT", DetFlags.overlay.SCT_on());
    kwargs.setdefault("do_SCT_background", globalflags.DataSource != "data");
    kwargs.setdefault("mainInputSCTKey", overlayFlags.dataStore() + "+SCT_SDO_Map");
    kwargs.setdefault("overlayInputSCTKey", overlayFlags.evtStore() + "+SCT_SDO_Map");
    kwargs.setdefault("mainOutputSCTKey", overlayFlags.outputStore() + "+SCT_SDO_Map");
    kwargs.setdefault("do_Pixel", DetFlags.overlay.pixel_on());
    kwargs.setdefault("do_Pixel_background", globalflags.DataSource != "data");
    kwargs.setdefault("mainInputPixelKey", overlayFlags.dataStore() + "+PixelSDO_Map");
    kwargs.setdefault("overlayInputPixelKey", overlayFlags.evtStore() + "+PixelSDO_Map");
    kwargs.setdefault("mainOutputPixelKey", overlayFlags.outputStore() + "+PixelSDO_Map");

    return CfgMgr.InDetSDOOverlay(name, **kwargs)
