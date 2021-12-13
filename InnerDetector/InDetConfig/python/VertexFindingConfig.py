# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def primaryVertexFindingCfg(flags, **kwargs):
    return primaryVertexFindingImplCfg(
        flags,
        SortingSetup=flags.InDet.primaryVertexSortingSetup,
        VertexSetup=flags.InDet.primaryVertexSetup,
        **kwargs)


def primaryVertexFindingImplCfg(
        flags,
        SortingSetup,
        VertexSetup,
        ** kwargs):
    acc = ComponentAccumulator()

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

    tracksMaxZinterval = kwargs.pop("maxZinterval")

    # Track Selector
    # TODO find out which of the settings below need to be picked from flags
    InDetTrackSelectorTool = CompFactory.InDet.InDetTrackSelectionTool(
        "InDetDetailedTrackSelectionTool",
        CutLevel="TightPrimary",
        UseTrkTrackTools=False,
        **kwargs
    )

    vertexSorter = None
    if SortingSetup == 'SumPt2Sorting':
        vertexWeightTool = CompFactory.Trk.SumPtVertexWeightCalculator(
            "InDetSumPtVertexWeightCalculator"
        )
        vertexSorter = CompFactory.Trk.VertexCollectionSortingTool(
            "InDetVertexCollectionSortingTool",
            VertexWeightCalculator=vertexWeightTool
        )

    # finder tool
    finderTool = None
    if VertexSetup == 'GaussAdaptiveMultiFinding':

        InDetVtxSeedFinder = CompFactory.Trk.TrackDensitySeedFinder(
            name="GaussianDensitySeedFinder")

        InDetAnnealingMaker = CompFactory.Trk.DetAnnealingMaker(
            name="InDetAnnealingMaker",
            SetOfTemperatures=[1.])

        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))
        InDetImpactPoint3dEstimator = CompFactory.Trk.ImpactPoint3dEstimator(
            name="InDetImpactPoint3dEstimator",
            Extrapolator=InDetExtrapolator)

        from InDetConfig.TrackingCommonConfig import (
            FullLinearizedTrackFactoryCfg)
        linearizedFactory = acc.popToolsAndMerge(
            FullLinearizedTrackFactoryCfg(flags))

        InDetVxFitterTool = CompFactory.Trk.AdaptiveMultiVertexFitter(
            name="InDetAdaptiveMultiVertexFitter",
            LinearizedTrackFactory=linearizedFactory,
            ImpactPoint3dEstimator=InDetImpactPoint3dEstimator,
            AnnealingMaker=InDetAnnealingMaker,
            DoSmoothing=True)

        finderTool = CompFactory.InDet.InDetAdaptiveMultiPriVxFinderTool(
            name="InDetAdaptiveMultiPriVxFinderTool",
            SeedFinder=InDetVtxSeedFinder,
            VertexFitterTool=InDetVxFitterTool,
            TrackSelector=InDetTrackSelectorTool,
            useBeamConstraint=flags.InDet.useBeamConstraint,
            selectiontype=0,
            TracksMaxZinterval=tracksMaxZinterval,
            m_useSeedConstraint=False,  # switching off seed constraint
            do3dSplitting=flags.InDet.doPrimaryVertex3DFinding)

    elif VertexSetup == 'ActsGaussAdaptiveMultiFinding':

        from ActsGeometry.ActsGeometryConfig import (
            ActsTrackingGeometryToolCfg, ActsExtrapolationToolCfg)
        actsGeoAcc, geometry = ActsTrackingGeometryToolCfg(flags)
        acc.merge(actsGeoAcc)

        trackExtrapolator = acc.getPrimaryAndMerge(
            ActsExtrapolationToolCfg(flags))

        finderTool = CompFactory.ActsAdaptiveMultiPriVtxFinderTool(
            "ActsAdaptiveMultiPriVtxFinderTool",
            ExtrapolationTool=trackExtrapolator,
            TrackSelector=InDetTrackSelectorTool,
            TrackingGeometryTool=geometry,
            tracksMaxZinterval=tracksMaxZinterval,
        )

    # setup the actual finder we want to use
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


if __name__ == "__main__":
    from AthenaCommon.Logging import logging
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior = 1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    from AthenaConfiguration.ComponentAccumulator import printProperties
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg

    flags.Input.Files = defaultTestFiles.RDO
    import sys
    if 'ActsGaussAdaptiveMultiFinding' in sys.argv:
        flags.InDet.primaryVertexSetup = 'ActsGaussAdaptiveMultiFinding'
    else:
        flags.InDet.primaryVertexSetup = 'GaussAdaptiveMultiFinding'
    flags.lock()

    acc = MainServicesCfg(flags)
    acc.merge(primaryVertexFindingCfg(flags))

    mlog = logging.getLogger("primaryVertexFindingConfigTest")
    mlog.info("Configuring  primaryVertexFinding: ")
    printProperties(mlog,
                    acc.getEventAlgo(
                        f"InDet{flags.InDet.Tracking.extension}PriVxFinder"),
                    nestLevel=1,
                    printDefaults=True)

    with open("primaryVertexFinding.pkl", "wb") as f:
        acc.store(f)
