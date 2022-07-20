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

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.MCChannelNumber() > 0:
        kwargs.setdefault("MCChannelNumber", athenaCommonFlags.MCChannelNumber())

    return CfgMgr.xAODMaker__EventInfoOverlay(name, **kwargs)


def getEventInfoUpdateFromContextAlg(name="EventInfoUpdateFromContextAlg", **kwargs):
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "BeamSpotCondAlg"):
        from IOVDbSvc.CondDB import conddb
        conddb.addFolderSplitOnline("INDET","/Indet/Onl/Beampos","/Indet/Beampos", className='AthenaAttributeList')
        condSeq += CfgMgr.BeamSpotCondAlg( "BeamSpotCondAlg" )
    from SGComps import AddressRemappingSvc
    AddressRemappingSvc.addInputRename("xAOD::EventInfo","EventInfo","Input_EventInfo")
    AddressRemappingSvc.addInputRename("xAOD::EventAuxInfo","EventInfoAux.","Input_EventInfoAux.")

    kwargs.setdefault("SignalInputKey", "Input_EventInfo")
    kwargs.setdefault("OutputKey", "EventInfo")

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.MCChannelNumber() > 0:
        kwargs.setdefault("MCChannelNumber", athenaCommonFlags.MCChannelNumber())

    return CfgMgr.xAODMaker__EventInfoUpdateFromContextAlg(name, **kwargs)
