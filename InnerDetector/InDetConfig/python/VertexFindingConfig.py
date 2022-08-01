# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def primaryVertexFindingCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    if flags.Detector.GeometryITk:
        vtxFlags = flags.ITk.PriVertex
    else:
        vtxFlags = flags.InDet.PriVertex

    if vtxFlags.sortingSetup == 'SumPt2Sorting':
        vertexWeightTool = CompFactory.Trk.SumPtVertexWeightCalculator(
            DoSumPt2Selection=True
        )
        decorName = "sumPt2"
    elif vtxFlags.sortingSetup == "SumPtSorting":
        vertexWeightTool = CompFactory.Trk.SumPtVertexWeightCalculator(
            DoSumPt2Selection=False
        )
        decorName = "sumPt"

    vertexSorter = CompFactory.Trk.VertexCollectionSortingTool(
        VertexWeightCalculator=vertexWeightTool,
        decorationName=decorName,
    )
    # finder tool
    finderTool = None
    if vtxFlags.setup == 'GaussAdaptiveMultiFinding':
        finderTool = acc.popToolsAndMerge(
            AdaptiveMultiFindingBaseCfg(flags, doGauss=True)
        )
    elif vtxFlags.setup == "AdaptiveMultiFinding":
        finderTool = acc.popToolsAndMerge(
            AdaptiveMultiFindingBaseCfg(flags, doGauss=False)
        )
    elif vtxFlags.setup == "GaussIterativeFinding":
        finderTool = acc.popToolsAndMerge(
            IterativeFindingBaseCfg(flags, doGauss=True)
        )
    elif vtxFlags.setup == "IterativeFinding":
        finderTool = acc.popToolsAndMerge(
            IterativeFindingBaseCfg(flags, doGauss=False)
        )
    elif vtxFlags.setup == "ActsGaussAdaptiveMultiFinding":
        finderTool = acc.popToolsAndMerge(
            ActsGaussAdaptiveMultiFindingBaseCfg(flags)
        )

    # setup the finder alg
    InDetPriVxFinder = CompFactory.InDet.InDetPriVxFinder(
        name="InDetPriVxFinder",
        doVertexSorting=True,
        VertexCollectionSortingTool=vertexSorter,
        VertexFinderTool=finderTool,
    )
    acc.addEventAlgo(InDetPriVxFinder)

    from OutputStreamAthenaPool.OutputStreamConfig import addToESD, addToAOD

    excludedVtxAuxData = "-vxTrackAtVertex.-MvfFitInfo.-isInitialized.-VTAV"
    verticesContainer = [
        "xAOD::VertexContainer#PrimaryVertices",
        "xAOD::VertexAuxContainer#PrimaryVerticesAux." + excludedVtxAuxData,
    ]

    acc.merge(addToAOD(flags, verticesContainer))
    acc.merge(addToESD(flags, verticesContainer))

    return acc


def AdaptiveMultiFindingBaseCfg(flags, doGauss, **kwargs):
    acc = ComponentAccumulator()
    if "SeedFinder" not in kwargs:
        if doGauss:
            kwargs["SeedFinder"] = CompFactory.Trk.TrackDensitySeedFinder()
        else:
            from TrkConfig.TrkVertexFitterUtilsConfig import (
                TrackToVertexIPEstimatorCfg,
            )

            kwargs["SeedFinder"] = CompFactory.Trk.ZScanSeedFinder(
                IPEstimator=acc.popToolsAndMerge(
                    TrackToVertexIPEstimatorCfg(flags)
                ),
            )

    if "TrackSelector" not in kwargs:
        from InDetConfig.InDetTrackSelectionToolConfig import VtxInDetTrackSelectionCfg
        kwargs["TrackSelector"] = acc.popToolsAndMerge(
            VtxInDetTrackSelectionCfg(flags)
        )

    if "VertexFitterTool" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import DetAnnealingMakerCfg
        InDetAnnealingMaker = acc.popToolsAndMerge(DetAnnealingMakerCfg(flags))
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        InDetExtrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
        InDetImpactPoint3dEstimator = CompFactory.Trk.ImpactPoint3dEstimator(
            Extrapolator=InDetExtrapolator
        )
        from TrkConfig.TrkVertexFitterUtilsConfig import (
            FullLinearizedTrackFactoryCfg,
        )

        linearizedFactory = acc.popToolsAndMerge(
            FullLinearizedTrackFactoryCfg(flags)
        )
        kwargs["VertexFitterTool"] = CompFactory.Trk.AdaptiveMultiVertexFitter(
            LinearizedTrackFactory=linearizedFactory,
            ImpactPoint3dEstimator=InDetImpactPoint3dEstimator,
            AnnealingMaker=InDetAnnealingMaker,
            DoSmoothing=True,
        )

    if flags.Detector.GeometryITk:
        vtxFlags = flags.ITk.PriVertex
    else:
        vtxFlags = flags.InDet.PriVertex
    kwargs.setdefault("useBeamConstraint", vtxFlags.useBeamConstraint)
    kwargs.setdefault("selectiontype", 0)
    kwargs.setdefault("TracksMaxZinterval", vtxFlags.maxZinterval)
    kwargs.setdefault("do3dSplitting", not vtxFlags.useBeamConstraint)
    kwargs.setdefault("useSeedConstraint", False)
    finderTool = CompFactory.InDet.InDetAdaptiveMultiPriVxFinderTool(
        name="InDetAdaptiveMultiPriVxFinderTool",
        **kwargs,
    )
    acc.setPrivateTools(finderTool)
    return acc


def IterativeFindingBaseCfg(flags, doGauss, **kwargs):
    acc = ComponentAccumulator()
    if "SeedFinder" not in kwargs:
        if doGauss:
            kwargs["SeedFinder"] = CompFactory.Trk.TrackDensitySeedFinder()
        else:
            from TrkConfig.TrkVertexFitterUtilsConfig import (
                TrackToVertexIPEstimatorCfg,
            )

            kwargs["SeedFinder"] = CompFactory.Trk.ZScanSeedFinder(
                IPEstimator=acc.popToolsAndMerge(
                    TrackToVertexIPEstimatorCfg(flags)
                ),
            )

    if "TrackSelector" not in kwargs:
        from InDetConfig.InDetTrackSelectionToolConfig import VtxInDetTrackSelectionCfg
        kwargs["TrackSelector"] = acc.popToolsAndMerge(
            VtxInDetTrackSelectionCfg(flags)
        )
    if "LinearizedTrackFactory" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import (
            FullLinearizedTrackFactoryCfg,
        )

        kwargs["LinearizedTrackFactory"] = acc.popToolsAndMerge(
            FullLinearizedTrackFactoryCfg(flags)
        )
    if "ImpactPoint3dEstimator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg

        InDetExtrapolator = acc.popToolsAndMerge(InDetExtrapolatorCfg(flags))
        kwargs[
            "ImpactPoint3dEstimator"
        ] = CompFactory.Trk.ImpactPoint3dEstimator(
            Extrapolator=InDetExtrapolator
        )
    if "VertexFitterTool" not in kwargs:
        from TrkConfig.TrkVertexFitterUtilsConfig import DetAnnealingMakerCfg
        InDetAnnealingMaker = acc.popToolsAndMerge(DetAnnealingMakerCfg(flags))
        InDetVertexSmoother = CompFactory.Trk.SequentialVertexSmoother()
        kwargs["VertexFitterTool"] = CompFactory.Trk.AdaptiveVertexFitter(
            SeedFinder=kwargs["SeedFinder"],
            LinearizedTrackFactory=kwargs["LinearizedTrackFactory"],
            ImpactPoint3dEstimator=kwargs["ImpactPoint3dEstimator"],
            AnnealingMaker=InDetAnnealingMaker,
            VertexSmoother=InDetVertexSmoother,
        )

    if flags.Detector.GeometryITk:
        vtxFlags = flags.ITk.PriVertex
    else:
        vtxFlags = flags.InDet.PriVertex
    kwargs.setdefault("useBeamConstraint", vtxFlags.useBeamConstraint)
    kwargs.setdefault("significanceCutSeeding", 12)
    kwargs.setdefault("maximumChi2cutForSeeding", 49)
    kwargs.setdefault("maxVertices", 200)
    kwargs.setdefault("doMaxTracksCut", vtxFlags.doMaxTracksCut)
    kwargs.setdefault("MaxTracks", vtxFlags.MaxTracks)
    finderTool = CompFactory.InDet.InDetIterativePriVxFinderTool(
        name="InDetIterativePriVxFinderTool", **kwargs
    )

    acc.setPrivateTools(finderTool)
    return acc


def ActsGaussAdaptiveMultiFindingBaseCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    from ActsGeometry.ActsGeometryConfig import (
        ActsTrackingGeometryToolCfg,
        ActsExtrapolationToolCfg,
    )

    if "TrackSelector" not in kwargs:
        from InDetConfig.InDetTrackSelectionToolConfig import VtxInDetTrackSelectionCfg
        kwargs["TrackSelector"] = acc.popToolsAndMerge(
            VtxInDetTrackSelectionCfg(flags)
        )
    geometry = acc.getPrimaryAndMerge(ActsTrackingGeometryToolCfg(flags))
    trackExtrapolator = acc.getPrimaryAndMerge(ActsExtrapolationToolCfg(flags))
    if flags.Detector.GeometryITk:
        vtxFlags = flags.ITk.PriVertex
    else:
        vtxFlags = flags.InDet.PriVertex

    kwargs.setdefault("useBeamConstraint", vtxFlags.useBeamConstraint)
    kwargs.setdefault("tracksMaxZinterval", vtxFlags.maxZinterval)
    kwargs.setdefault("do3dSplitting", not vtxFlags.useBeamConstraint)
    finderTool = CompFactory.ActsTrk.AdaptiveMultiPriVtxFinderTool(
        "ActsAdaptiveMultiPriVtxFinderTool",
        **kwargs,
        ExtrapolationTool=trackExtrapolator,
        TrackingGeometryTool=geometry,
    )
    acc.setPrivateTools(finderTool)
    return acc


if __name__ == "__main__":
    from AthenaCommon.Logging import logging
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    flags.Input.Files = defaultTestFiles.RDO_RUN2
    import sys
    if 'ActsGaussAdaptiveMultiFinding' in sys.argv:
        flags.InDet.PriVertex.setup = "ActsGaussAdaptiveMultiFinding"
    elif "IterativeFinding" in sys.argv:
        flags.InDet.PriVertex.setup = "IterativeFinding"
    elif "GaussIterativeFinding" in sys.argv:
        flags.InDet.PriVertex.setup = "GaussIterativeFinding"
    elif "AdaptiveMultiFinding" in sys.argv:
        flags.InDet.PriVertex.setup = "AdaptiveMultiFinding"
    elif "GaussAdaptiveMultiFinding" in sys.argv:
        flags.InDet.PriVertex.setup = "GaussAdaptiveMultiFinding"
    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge(primaryVertexFindingCfg(flags))

    mlog = logging.getLogger("primaryVertexFindingConfigTest")
    mlog.info("Configuring  primaryVertexFinding: ")

    printProperties(
        mlog,
        acc.getEventAlgo("InDetPriVxFinder"),
        nestLevel=2,
        printDefaults=True,
    )
