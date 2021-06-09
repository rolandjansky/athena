# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def primaryVertexFindingCfg(flags):
    acc = ComponentAccumulator()
    vertexWeightTool = CompFactory.Trk.SumPtVertexWeightCalculator(
        "InDetSumPtVertexWeightCalculator"
    )
    vertexSorter = CompFactory.Trk.VertexCollectionSortingTool(
        "InDetVertexCollectionSortingTool", VertexWeightCalculator=vertexWeightTool
    )


    from InDetConfig.TrackingCommonConfig import InDetTrackSummaryToolCfg


    from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg, ActsExtrapolationToolCfg, ActsAlignmentCondAlgCfg
    #TODO the alignment conditions alg should be loaded with extrapolator (same for ActsGeoSvc)
    acc.merge(ActsAlignmentCondAlgCfg(flags))
    actsGeoAcc, geometry = ActsTrackingGeometryToolCfg(flags)
    acc.merge(actsGeoAcc)

    trackExtrapolatorAcc = ActsExtrapolationToolCfg(flags)
    trackExtrapolator = trackExtrapolatorAcc.getPrimary()
    acc.merge(trackExtrapolatorAcc)

    trackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags))

    # TODO find out which of the settings below need to be picked from flags
    trackSelector = CompFactory.InDet.InDetTrackSelectionTool(
        "InDetDetailedTrackSelectionTool",
        CutLevel="TightPrimary",
        Extrapolator=trackExtrapolator,
        TrackSummaryTool=trackSummaryTool,
        maxAbsEta=9999.0,
        maxD0=4.0,
        maxNPixelHoles=1,
        maxSigmaD0=5.0,
        maxSigmaZ0SinTheta=10.0,
        maxZ0=1000.0,
        maxZ0SinTheta=1000.0,
        minNInnermostLayerHits=0,
        minNPixelHits=1,
        minNSctHits=4,
        minNSiHits=6,
        minNTrtHits=0,
        minPt=500.0,
    )


    finderTool = CompFactory.ActsAdaptiveMultiPriVtxFinderTool(
        "ActsAdaptiveMultiPriVtxFinderTool",
        ExtrapolationTool=trackExtrapolator,
        TrackSelector=trackSelector,
        TrackingGeometryTool=geometry,
        tracksMaxZinterval=3,
    )

    InDetPriVxFinder = CompFactory.InDet.InDetPriVxFinder(
        name=f"InDet{flags.InDet.Tracking.extension}PriVxFinder",
        doVertexSorting=True,
        VertexCollectionSortingTool=vertexSorter,
        VertexFinderTool=finderTool
    )
    acc.addEventAlgo(InDetPriVxFinder)

    #from OutputStreamAthenaPool.OutputStreamConfig import addToESD,addToAOD
    #TODO debug why vertex container is crashing AOD writing
    #verticesContainer = ["xAOD::VertexContainer#PrimaryVertices", "xAOD::VertexAuxContainer#PrimaryVerticesAux."]
    #acc.merge(addToAOD(flags, verticesContainer))
    #acc.merge(addToESD(flags, verticesContainer))

    return acc
