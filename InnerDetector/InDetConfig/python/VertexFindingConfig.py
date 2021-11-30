# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def primaryVertexFindingCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    vertexWeightTool = CompFactory.Trk.SumPtVertexWeightCalculator(
        "InDetSumPtVertexWeightCalculator"
    )
    vertexSorter = CompFactory.Trk.VertexCollectionSortingTool(
        "InDetVertexCollectionSortingTool", VertexWeightCalculator=vertexWeightTool
    )

    from InDetConfig.TrackingCommonConfig import TrackSummaryToolCfg
    from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg, ActsExtrapolationToolCfg

    actsGeoAcc, geometry = ActsTrackingGeometryToolCfg(flags)
    acc.merge(actsGeoAcc)

    trackExtrapolator = acc.getPrimaryAndMerge(ActsExtrapolationToolCfg(flags))
    trackSummaryTool = acc.getPrimaryAndMerge(TrackSummaryToolCfg(flags))

    if flags.Detector.GeometryITk:
        vtxFlags = flags.ITk.PriVertex
    else:
        vtxFlags = flags.InDet.PriVertex

    for key in (
        "maxAbsEta",
        "maxD0",
        "maxNPixelHoles",
        "maxSigmaD0",
        "maxSigmaZ0SinTheta",
        "maxZ0",
        "maxZ0SinTheta",
        "minNInnermostLayerHits",
        "minNPixelHits",
        "minNSctHits",
        "minNSiHits",
        "minNTrtHits",
        "minPt",
        "maxZinterval"
    ):
        kwargs.setdefault(key, getattr(vtxFlags, key))


    tracksMaxZinterval=kwargs.pop("maxZinterval")

    # TODO find out which of the settings below need to be picked from flags
    trackSelector = CompFactory.InDet.InDetTrackSelectionTool(
        "InDetDetailedTrackSelectionTool",
        CutLevel="TightPrimary",
        Extrapolator=trackExtrapolator,
        TrackSummaryTool=trackSummaryTool,
        **kwargs
    )

    finderTool = CompFactory.ActsAdaptiveMultiPriVtxFinderTool(
        "ActsAdaptiveMultiPriVtxFinderTool",
        ExtrapolationTool=trackExtrapolator,
        TrackSelector=trackSelector,
        TrackingGeometryTool=geometry,
        tracksMaxZinterval=tracksMaxZinterval,
    )

    InDetPriVxFinder = CompFactory.InDet.InDetPriVxFinder(
        name=f"InDet{flags.InDet.Tracking.extension}PriVxFinder",
        doVertexSorting=True,
        VertexCollectionSortingTool=vertexSorter,
        VertexFinderTool=finderTool
    )
    acc.addEventAlgo(InDetPriVxFinder)

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD, addToAOD
    excludedVtxAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
    verticesContainer = [
         "xAOD::VertexContainer#PrimaryVertices",
         "xAOD::VertexAuxContainer#PrimaryVerticesAux."+excludedVtxAuxData
    ]

    acc.merge(addToAOD(flags, verticesContainer))
    acc.merge(addToESD(flags, verticesContainer))

    return acc
