# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr


def getEventInfoOverlay(name="EventInfoOverlay", **kwargs):
    from OverlayCommonAlgs.OverlayFlags import overlayFlags

    kwargs.setdefault("BkgInputKey", overlayFlags.bkgPrefix() + "EventInfo")
    kwargs.setdefault("SignalInputKey", overlayFlags.sigPrefix() + "EventInfo")
    kwargs.setdefault("OutputKey", "EventInfo")

    kwargs.setdefault("DataOverlay", overlayFlags.isDataOverlay())

    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    kwargs.setdefault("ValidateBeamSpot", not overlayFlags.isDataOverlay() and commonGeoFlags.Run() == "RUN3")

    return CfgMgr.xAODMaker__EventInfoOverlay(name, **kwargs)


def getEventInfoUpdateFromContextAlg(name="EventInfoUpdateFromContextAlg", **kwargs):
    from SGComps import AddressRemappingSvc
    AddressRemappingSvc.addInputRename("xAOD::EventInfo","EventInfo","HITs_EventInfo")
    AddressRemappingSvc.addInputRename("xAOD::EventAuxInfo","EventInfoAux.","HITs_EventInfoAux.")

    kwargs.setdefault("SignalInputKey", "HITs_EventInfo")
    kwargs.setdefault("OutputKey", "EventInfo")

    return CfgMgr.xAODMaker__EventInfoUpdateFromContextAlg(name, **kwargs)
