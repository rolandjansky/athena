# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TRT_SeededTrackFinder package

from AthenaConfiguration.ComponentFactory import CompFactory
import AthenaCommon.SystemOfUnits as Units

def TRT_SeededTrackFinderCfg(flags, name='InDetTRT_SeededTrackFinder', InputCollections=[], **kwargs):

    # TRT seeded back tracking algorithm

    #
    # --- decide if use the association tool
    #
    usePrdAssociationTool = (len(InputCollections) > 0)

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc = BeamSpotCondAlgCfg(flags)

    if flags.InDet.Tracking.ActivePass.usePixel:
        from InDetConfig.SiCombinatorialTrackFinderToolConfig import SiDetElementBoundaryLinksCondAlg_xk_Pixel_Cfg
        acc.merge(SiDetElementBoundaryLinksCondAlg_xk_Pixel_Cfg(flags))

    if flags.InDet.Tracking.ActivePass.useSCT:
        from InDetConfig.SiCombinatorialTrackFinderToolConfig import SiDetElementBoundaryLinksCondAlg_xk_SCT_Cfg
        acc.merge(SiDetElementBoundaryLinksCondAlg_xk_SCT_Cfg(flags))

    from InDetConfig.TrackingCommonConfig import InDetTrackFitterBTCfg, InDetTrackSummaryToolNoHoleSearchCfg, InDetTRT_ExtensionToolCfg
    InDetTrackFitterBT = acc.popToolsAndMerge(InDetTrackFitterBTCfg(flags))
    InDetTrackSummaryToolNoHoleSearch = acc.getPrimaryAndMerge(InDetTrackSummaryToolNoHoleSearchCfg(flags))
    InDetTRTExtensionTool = acc.popToolsAndMerge(InDetTRT_ExtensionToolCfg(flags))
    acc.addPublicTool(InDetTRTExtensionTool)

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))

    from InDetConfig.TRT_SeededTrackFinderToolConfig import TRT_SeededTrackFinder_ATLCfg
    InDetTRT_SeededTrackTool = acc.popToolsAndMerge(TRT_SeededTrackFinder_ATLCfg(flags, InputCollections=InputCollections))
    acc.addPublicTool(InDetTRT_SeededTrackTool)

    kwargs.setdefault("RefitterTool", InDetTrackFitterBT)
    kwargs.setdefault("TrackTool", InDetTRT_SeededTrackTool)
    kwargs.setdefault("PRDtoTrackMap", 'InDetSegmentPRDtoTrackMap' if usePrdAssociationTool else "")
    kwargs.setdefault("TrackSummaryTool", InDetTrackSummaryToolNoHoleSearch)
    kwargs.setdefault("TrackExtensionTool", InDetTRTExtensionTool)
    kwargs.setdefault("MinTRTonSegment", flags.InDet.Tracking.ActivePass.minSecondaryTRTonTrk)
    kwargs.setdefault("MinTRTonly", flags.InDet.Tracking.ActivePass.minTRTonly)
    kwargs.setdefault("TrtExtension", True)
    kwargs.setdefault("SiExtensionCuts", flags.InDet.Tracking.ActivePass.SiExtensionCuts)
    kwargs.setdefault("minPt", flags.InDet.Tracking.ActivePass.minSecondaryPt)
    kwargs.setdefault("maxRPhiImp", flags.InDet.Tracking.ActivePass.maxSecondaryImpact)
    kwargs.setdefault("maxZImp", flags.InDet.Tracking.ActivePass.maxZImpact)
    kwargs.setdefault("maxEta", flags.InDet.Tracking.ActivePass.maxEta)
    kwargs.setdefault("Extrapolator", InDetExtrapolator)
    kwargs.setdefault("RejectShortExtension", flags.InDet.Tracking.ActivePass.rejectShortExtensions)
    kwargs.setdefault("FinalRefit", False)
    kwargs.setdefault("FinalStatistics", False)
    kwargs.setdefault("OutputSegments", False)
    kwargs.setdefault("InputSegmentsLocation", 'TRTSegments')
    kwargs.setdefault("OutputTracksLocation", 'TRTSeededTracks')

    if flags.InDet.Tracking.ActivePass.RoISeededBackTracking:
        from RegionSelector.RegSelToolConfig import regSelTool_SCT_Cfg
        RegSelTool_SCT   = acc.popToolsAndMerge(regSelTool_SCT_Cfg(flags))
        acc.addPublicTool(RegSelTool_SCT)

        kwargs.setdefault("RegSelTool", RegSelTool_SCT)
        kwargs.setdefault("CaloSeededRoI", True)
        kwargs.setdefault("EMROIPhiRZContainer", "InDetCaloClusterROIPhiRZ%.0fGeVUnordered" % (flags.InDet.Tracking.ActivePass.minRoIClusterEt/Units.GeV))

    InDetTRT_SeededTrackFinder = CompFactory.InDet.TRT_SeededTrackFinder(name = name, **kwargs)
    acc.addEventAlgo(InDetTRT_SeededTrackFinder)
    return acc
