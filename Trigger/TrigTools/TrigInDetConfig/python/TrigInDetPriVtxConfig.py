#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

__author__ =   "Tomas Bold"


#  These routines create the vertex finder using the ComponentAccumulator 


def vertexFinderCfg(flags, signature, inputTracks, outputVertices, adaptiveVertexing):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    from InDetPriVxFinder.InDetPriVxFinderMonitoring import (
        InDetPriVxFinderMonitoringTool,
    )

    acc = ComponentAccumulator()
    if adaptiveVertexing:
        vtxFinderTool = acc.popToolsAndMerge(
            adaptiveMultiVertexFinderCfg(flags, signature)
        )
        alg_name = f"InDetTrigAMVFPriVxFinder{signature}"
    else:
        vtxFinderTool = acc.popToolsAndMerge(iterativeVertexFinderCfg(flags, signature))
        alg_name = f"InDetTrigIterativePriVxFinder{signature}"

    acc.addEventAlgo(
        CompFactory.InDet.InDetPriVxFinder(
            alg_name,
            VertexFinderTool=vtxFinderTool,
            TracksName=inputTracks,
            VxCandidatesOutputName=outputVertices,
            VertexCollectionSortingTool=CompFactory.Trk.VertexCollectionSortingTool(
                f"InDetVertexCollectionSortingTool{signature}",
                VertexWeightCalculator=CompFactory.Trk.SumPtVertexWeightCalculator(
                    f"InDetSumPtVertexWeightCalculator{signature}",
                    DoSumPt2Selection=True,
                ),
            ),
            doVertexSorting=True,
            PriVxMonTool=InDetPriVxFinderMonitoringTool(),
        ),
        primary=True,
    )
    return acc
    

def iterativeVertexFinderCfg(flags, signature):
    """ Configure the iterative vertex finder """
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    from InDetConfig.InDetTrackSelectionToolConfig import TrigVtxInDetTrackSelectionCfg
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    from InDetTrigRecExample.TrigInDetConfiguredVtxCuts import ConfiguredTrigVtxCuts 

    acc = ComponentAccumulator()
    vtx_cuts = ConfiguredTrigVtxCuts()
    extrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
    acc.addPublicTool(extrapolator)
    linear_track_factory = CompFactory.Trk.FullLinearizedTrackFactory(
        f"FullLinearizedTrackFactory{signature}",
        Extrapolator=extrapolator,
    )
    acc.setPrivateTools(
        CompFactory.InDet.InDetIterativePriVxFinderTool(
            f"InDetTrigPriVxFinderTool{signature}",
            VertexFitterTool=CompFactory.Trk.FastVertexFitter(
                f"InDetTrigFastVertexFitterTool{signature}",
                LinearizedTrackFactory=linear_track_factory,
                Extrapolator=extrapolator,
            ),
            TrackSelector=acc.popToolsAndMerge(
                TrigVtxInDetTrackSelectionCfg(flags, signature = signature)
            ),
            SeedFinder=CompFactory.Trk.TrackDensitySeedFinder(
                f"TrigGaussianDensitySeed{signature}",
                DensityEstimator=CompFactory.Trk.GaussianTrackDensity(
                    f"TrigGaussianDensity{signature}"
            ),
            ),
            ImpactPoint3dEstimator=CompFactory.Trk.ImpactPoint3dEstimator(
                f"InDetTrigImpactPoint3dEstimator{signature}",
                Extrapolator=extrapolator,
            ),
            LinearizedTrackFactory=linear_track_factory,
            useBeamConstraint=True,
            significanceCutSeeding=12,
            maximumChi2cutForSeeding=29,
            maxVertices=200,
            createSplitVertices=False,
            doMaxTracksCut=vtx_cuts.doMaxTracksCut(),
            MaxTracks=vtx_cuts.MaxTracks(),
        )
    )
    return acc


def adaptiveMultiVertexFinderCfg(flags, signature):
    """ Configure the adaptive multi-vertex finder """
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory

    from InDetConfig.InDetTrackSelectionToolConfig import TrigVtxInDetTrackSelectionCfg
    from TrkConfig.TrkVertexFitterUtilsConfig import TrackToVertexIPEstimatorCfg
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig

    acc = ComponentAccumulator()
    extrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
    config = getInDetTrigConfig(signature)

    # AdaptiveMultiVertexFitter (below) has all of its tools public
    linearized_track_factory = CompFactory.Trk.FullLinearizedTrackFactory(
                    f"FullLinearizedTrackFactory{signature}",
                    Extrapolator=extrapolator,
                )
    acc.addPublicTool(linearized_track_factory)

    impact_point3d_estimator = CompFactory.Trk.ImpactPoint3dEstimator(
                    f"InDetTrigImpactPoint3dEstimator{signature}",
                    Extrapolator=extrapolator,
                )
    acc.addPublicTool(impact_point3d_estimator)

    from TrkConfig.TrkVertexFitterUtilsConfig import DetAnnealingMakerCfg
    annealing_maker = acc.popToolsAndMerge(DetAnnealingMakerCfg(flags))

    acc.setPrivateTools(
        CompFactory.InDet.InDetAdaptiveMultiPriVxFinderTool(
            # All ToolHandles of InDetAdaptiveMultiPriVxFinderTool are private
            f"InDetTrigAdaptiveMultiPriVxFinderTool{signature}",
            SeedFinder=CompFactory.Trk.TrackDensitySeedFinder(
                f"TrigGaussianDensitySeed{signature}",
                DensityEstimator=CompFactory.Trk.GaussianTrackDensity(
                    f"TrigGaussianDensity{signature}"
                    ), # private tool
                ),
            VertexFitterTool=CompFactory.Trk.AdaptiveMultiVertexFitter( 
                #All tools are PUBLIC - not sure if defaults are okay for the rest? FIXME
                f"InDetTrigAdaptivemultiVertexFitterTool{signature}",
                LinearizedTrackFactory=linearized_track_factory,
                ImpactPoint3dEstimator=impact_point3d_estimator,
                AnnealingMaker=annealing_maker,
                DoSmoothing=True,
            ),
            TrackSelector=acc.popToolsAndMerge(
                TrigVtxInDetTrackSelectionCfg(flags, signature = signature)
            ),
            IPEstimator=acc.popToolsAndMerge(TrackToVertexIPEstimatorCfg(flags, Extrapolator = extrapolator)),
            useBeamConstraint=True,
            TracksMaxZinterval=config.TracksMaxZinterval,
            addSingleTrackVertices=config.addSingleTrackVertices,
            selectiontype=0,
            do3dSplitting=True,  # NB: comment from original function suggests that this should be flags.InDet.doPrimaryVertex3DFinding
        )
    )
    return acc
