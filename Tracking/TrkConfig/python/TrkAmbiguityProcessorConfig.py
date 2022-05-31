# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Configuration of TrkAmbiguityProcessor
# The ambiguity processor drives the ambiguity resolution step that
# rejects lower-quality candidates sharing a large number
# of associated hits with higher-quality ones.
# Special treatment for electrons (brem)
# photons (conversion) and charged particles
# in the cores of high-energy jets, can be applied.
# The track candidates resulting after the ambiguity resolution
# are then re-fitted  to obtain the final track parameter estimate.

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType


def SimpleAmbiguityProcessorToolCfg(flags,
                                    name="InDetAmbiguityProcessor",
                                    ClusterSplitProbContainer='',
                                    **kwargs):
    acc = ComponentAccumulator()

    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if (flags.Beam.Type is BeamType.Cosmics and
            flags.InDet.Tracking.ActivePass.extension != "DBM"):
        from InDetConfig.TrackingCommonConfig import InDetCosmicsScoringToolCfg
        InDetAmbiScoringTool = acc.popToolsAndMerge(
            InDetCosmicsScoringToolCfg(flags))
    elif (flags.InDet.Tracking.ActivePass.extension == "R3LargeD0" and
          flags.InDet.Tracking.nnCutLargeD0Threshold > 0):
        # Set up NN config
        from InDetConfig.TrackingCommonConfig import InDetNNScoringToolSiCfg
        InDetAmbiScoringTool = acc.popToolsAndMerge(
            InDetNNScoringToolSiCfg(flags))
    else:
        from InDetConfig.TrackingCommonConfig import InDetAmbiScoringToolSiCfg
        InDetAmbiScoringTool = acc.popToolsAndMerge(
            InDetAmbiScoringToolSiCfg(flags))

    if flags.InDet.Tracking.ActivePass.isLowPt:
        from InDetConfig.TrackingCommonConfig import InDetTrackFitterLowPt
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterLowPt(flags))
    else:
        from InDetConfig.TrackingCommonConfig import InDetTrackFitterCfg
        InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags))

    from InDetConfig.InDetAssociationToolsConfig import (
        InDetPRDtoTrackMapToolGangedPixelsCfg)
    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(
        InDetPRDtoTrackMapToolGangedPixelsCfg(flags))
    from TrkConfig.TrkTrackSummaryToolConfig import (
        InDetTrackSummaryToolAmbiCfg)
    ambi_track_summary_tool = acc.popToolsAndMerge(InDetTrackSummaryToolAmbiCfg(
        flags,
        name="InDetAmbiguityProcessorSplitProbTrackSummaryTool" + flags.InDet.Tracking.ActivePass.extension))

    from InDetConfig.InDetAmbiTrackSelectionToolConfig import (
        InDetAmbiTrackSelectionToolCfg)
    InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(
        InDetAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", InDetTrackFitter)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)
    kwargs.setdefault("SelectionTool", InDetAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName",
                      ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName",
                      'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("tryBremFit",
                      flags.InDet.Tracking.doBremRecovery and
                      (flags.InDet.Tracking.ActivePass.extension == "" or
                       flags.InDet.Tracking.ActivePass.extension == "BLS"))
    kwargs.setdefault(
        "caloSeededBrem", flags.InDet.Tracking.doCaloSeededBrem and
        flags.Detector.EnableCalo)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.ActivePass.minPTBrem)
    kwargs.setdefault("RefitPrds", True)
    kwargs.setdefault(
        "MatEffects", flags.InDet.Tracking.materialInteractionsType
        if flags.InDet.Tracking.materialInteractions else 0)

    if flags.InDet.Tracking.ActivePass.extension == (
        "Pixel" or
            flags.InDet.Tracking.ActivePass.extension == "DBM"):
        kwargs.setdefault("SuppressHoleSearch", True)

    InDetAmbiguityProcessor = CompFactory.Trk.SimpleAmbiguityProcessorTool(
        name=name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetAmbiguityProcessor)
    return acc


def SimpleAmbiguityProcessorTool_TRT_Cfg(
        flags,
        name='InDetTRT_SeededAmbiguityProcessor',
        ClusterSplitProbContainer="",
        **kwargs):
    acc = ComponentAccumulator()
    #
    # --- load Ambiguity Processor
    #
    from InDetConfig.TrackingCommonConfig import InDetTrackFitterBTCfg
    InDetTrackFitterBT = acc.popToolsAndMerge(InDetTrackFitterBTCfg(flags))
    from InDetConfig.InDetAssociationToolsConfig import (
        InDetPRDtoTrackMapToolGangedPixelsCfg)
    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(
        InDetPRDtoTrackMapToolGangedPixelsCfg(flags))

    #
    # --- set up special Scoring Tool for TRT seeded tracks
    #
    if flags.Beam.Type is BeamType.Cosmics:
        from InDetConfig.TrackingCommonConfig import (
            InDetCosmicScoringTool_TRTCfg)
        InDetTRT_SeededScoringTool = acc.popToolsAndMerge(
            InDetCosmicScoringTool_TRTCfg(flags))
        from TrkConfig.TrkTrackSummaryToolConfig import (
            InDetTrackSummaryToolSharedHitsCfg)
        InDetTRT_SeededSummaryTool = acc.popToolsAndMerge(
            InDetTrackSummaryToolSharedHitsCfg(flags))
    else:
        from InDetConfig.TrackingCommonConfig import (
            InDetTRT_SeededScoringToolCfg)
        InDetTRT_SeededScoringTool = acc.popToolsAndMerge(
            InDetTRT_SeededScoringToolCfg(flags))
        from TrkConfig.TrkTrackSummaryToolConfig import (
            InDetTrackSummaryToolCfg)
        InDetTRT_SeededSummaryTool = acc.popToolsAndMerge(
            InDetTrackSummaryToolCfg(flags))

    from InDetConfig.InDetAmbiTrackSelectionToolConfig import (
        InDetTRTAmbiTrackSelectionToolCfg)
    InDetTRT_SeededAmbiTrackSelectionTool = acc.popToolsAndMerge(
        InDetTRTAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", InDetTrackFitterBT)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("TrackSummaryTool", InDetTRT_SeededSummaryTool)
    kwargs.setdefault("SelectionTool", InDetTRT_SeededAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName",
                      ClusterSplitProbContainer)
    kwargs.setdefault(
        "OutputClusterSplitProbabilityName",
        'InDetTRT_SeededAmbiguityProcessorSplitProb'+flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("RefitPrds", False)
    kwargs.setdefault("SuppressTrackFit", False)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("ScoringTool", InDetTRT_SeededScoringTool)
    kwargs.setdefault(
        "MatEffects",
        flags.InDet.Tracking.materialInteractionsType if flags.InDet.Tracking.materialInteractions
        else 0)

    InDetTRT_SeededAmbiguityProcessor = CompFactory.Trk.SimpleAmbiguityProcessorTool(
        name=name, **kwargs)
    acc.setPrivateTools(InDetTRT_SeededAmbiguityProcessor)
    return acc


def SimpleAmbiguityProcessorTool_Trig_Cfg(
        flags,
        name='InDetTrig_SeededAmbiguityProcessor',
        **kwargs):
    import AthenaCommon.SystemOfUnits as Units

    acc = ComponentAccumulator()

    # TODO False if flags.InDet.Tracking.ActivePass.name == 'cosmics' else True
    kwargs.setdefault("SuppressHoleSearch", False)
    # kwargs.setdefault("RefitPrds", False)
    # #TODO clarify this setting False if flags.InDet.Tracking.ActivePass.name == 'cosmics' else True
    kwargs.setdefault("tryBremFit", True
                      if flags.InDet.Tracking.ActivePass.name == 'electron'
                      and flags.InDet.Tracking.doBremRecovery
                      else False)
    kwargs.setdefault("pTminBrem", 5*Units.GeV)
    kwargs.setdefault("MatEffects", 3)

    from TrigInDetConfig.TrigInDetConfig import (FitterToolCfg,
                                                 ambiguityScoringToolCfg)
    kwargs.setdefault("Fitter", acc.getPrimaryAndMerge(FitterToolCfg(flags)))
    kwargs.setdefault("ScoringTool", acc.getPrimaryAndMerge(
        ambiguityScoringToolCfg(flags)))
    from TrkConfig.TrkTrackSummaryToolConfig import (
        InDetTrigTrackSummaryToolCfg)
    kwargs.setdefault("TrackSummaryTool", acc.popToolsAndMerge(
        InDetTrigTrackSummaryToolCfg(flags)))
    from InDetConfig.InDetAssociationToolsConfig import (
        TrigPRDtoTrackMapToolGangedPixelsCfg)
    kwargs.setdefault("AssociationTool", acc.getPrimaryAndMerge(
        TrigPRDtoTrackMapToolGangedPixelsCfg(flags)))

    from InDetConfig.InDetAmbiTrackSelectionToolConfig import (
        InDetTrigTrackSelectionToolCfg)
    kwargs.setdefault("SelectionTool", acc.getPrimaryAndMerge(
        InDetTrigTrackSelectionToolCfg(flags)))

    acc.setPrivateTools(
        CompFactory.Trk.SimpleAmbiguityProcessorTool(name, **kwargs))
    return acc


def DenseEnvironmentsAmbiguityScoreProcessorToolCfg(
        flags,
        name="InDetAmbiguityScoreProcessor",
        ClusterSplitProbContainer="",
        **kwargs):

    acc = ComponentAccumulator()
    # --- set up different Scoring Tool for collisions and cosmics
    if (flags.Beam.Type is BeamType.Cosmics and
            flags.InDet.Tracking.ActivePass.extension != "DBM"):
        from InDetConfig.TrackingCommonConfig import InDetCosmicsScoringToolCfg
        InDetAmbiScoringTool = acc.popToolsAndMerge(
            InDetCosmicsScoringToolCfg(flags))
    elif (flags.InDet.Tracking.ActivePass.extension == "R3LargeD0"
          and flags.InDet.Tracking.nnCutLargeD0Threshold > 0):
        # Set up NN config
        from InDetConfig.TrackingCommonConfig import InDetNNScoringToolSiCfg
        InDetAmbiScoringTool = acc.popToolsAndMerge(
            InDetNNScoringToolSiCfg(flags))
    else:
        from InDetConfig.TrackingCommonConfig import InDetAmbiScoringToolSiCfg
        InDetAmbiScoringTool = acc.popToolsAndMerge(
            InDetAmbiScoringToolSiCfg(flags))
    kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)

    from InDetConfig.SiClusterizationToolConfig import (
        NnPixelClusterSplitProbToolCfg)
    kwargs.setdefault("SplitProbTool", acc.popToolsAndMerge(
        NnPixelClusterSplitProbToolCfg(flags))
        if flags.InDet.Tracking.doPixelClusterSplitting else "")

    from InDetConfig.InDetAssociationToolsConfig import (
        InDetPRDtoTrackMapToolGangedPixelsCfg)
    kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(
        InDetPRDtoTrackMapToolGangedPixelsCfg(flags)))
    from InDetConfig.TrackingCommonConfig import PRDtoTrackMapToolCfg
    kwargs.setdefault("AssociationToolNotGanged",
                      acc.popToolsAndMerge(PRDtoTrackMapToolCfg()))
    kwargs.setdefault("AssociationMapName",
                      f"PRDToTrackMap{flags.InDet.Tracking.ActivePass.extension}")

    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        kwargs.setdefault(
            "sharedProbCut", flags.InDet.Tracking.pixelClusterSplitProb1)
        kwargs.setdefault("sharedProbCut2",
                          flags.InDet.Tracking.pixelClusterSplitProb2)
        kwargs.setdefault(
            "SplitClusterMap_new",
            f"SplitClusterAmbiguityMap{flags.InDet.Tracking.ActivePass.extension}")

    kwargs.setdefault("InputClusterSplitProbabilityName",
                      ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName",
                      f"SplitProb{flags.InDet.Tracking.ActivePass.extension}")

    ScoreProcessorTool = CompFactory.Trk.DenseEnvironmentsAmbiguityScoreProcessorTool
    acc.setPrivateTools(ScoreProcessorTool(
        f"{name}{flags.InDet.Tracking.ActivePass.extension}", **kwargs))
    return acc


def ITkDenseEnvironmentsAmbiguityScoreProcessorToolCfg(
        flags,
        name="ITkAmbiguityScoreProcessor",
        ClusterSplitProbContainer='',
        **kwargs):

    acc = ComponentAccumulator()
    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type is BeamType.Cosmics:
        from InDetConfig.ITkTrackingCommonConfig import (
            ITkCosmicsScoringToolCfg)
        ITkAmbiScoringTool = acc.popToolsAndMerge(
            ITkCosmicsScoringToolCfg(flags))
    else:
        from InDetConfig.ITkTrackingCommonConfig import (
            ITkAmbiScoringToolCfg)
        ITkAmbiScoringTool = acc.popToolsAndMerge(ITkAmbiScoringToolCfg(flags))

    from InDetConfig.SiClusterizationToolConfig import (
        ITkTruthPixelClusterSplitProbToolCfg)
    ITkTruthPixelClusterSplitProbTool = acc.popToolsAndMerge(
        ITkTruthPixelClusterSplitProbToolCfg(flags))

    from InDetConfig.InDetAssociationToolsConfig import (
        ITkPRDtoTrackMapToolGangedPixelsCfg)
    ITkPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(
        ITkPRDtoTrackMapToolGangedPixelsCfg(flags))
    from InDetConfig.ITkTrackingCommonConfig import ITkPRDtoTrackMapToolCfg
    ITkPRDtoTrackMapTool = acc.popToolsAndMerge(ITkPRDtoTrackMapToolCfg(flags))

    kwargs.setdefault(
        "sharedProbCut",  flags.ITk.Tracking.pixelClusterSplitProb1)
    kwargs.setdefault("sharedProbCut2",
                      flags.ITk.Tracking.pixelClusterSplitProb2)
    kwargs.setdefault("SplitClusterMap_new", 'SplitClusterAmbiguityMap' +
                      flags.ITk.Tracking.ActivePass.extension)

    kwargs.setdefault("ScoringTool", ITkAmbiScoringTool)
    kwargs.setdefault("SplitProbTool", ITkTruthPixelClusterSplitProbTool
                      if flags.ITk.Tracking.doPixelClusterSplitting
                      else None,)
    kwargs.setdefault("AssociationTool", ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationToolNotGanged", ITkPRDtoTrackMapTool)
    kwargs.setdefault("AssociationMapName", 'ITkPRDToTrackMap' +
                      flags.ITk.Tracking.ActivePass.extension)
    kwargs.setdefault("InputClusterSplitProbabilityName",
                      ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName",
                      'SplitProb'+flags.ITk.Tracking.ActivePass.extension)

    # DenseEnvironmentsAmbiguityScoreProcessorTool
    ITkAmbiguityScoreProcessor = CompFactory.Trk.DenseEnvironmentsAmbiguityScoreProcessorTool(
        name=name+flags.ITk.Tracking.ActivePass.extension, **kwargs)

    acc.setPrivateTools(ITkAmbiguityScoreProcessor)
    return acc


def DenseEnvironmentsAmbiguityProcessorToolCfg(
        flags,
        name="InDetAmbiguityProcessor",
        **kwargs):

    acc = ComponentAccumulator()

    # --- set up different Scoring Tool for collisions and cosmics
    if (flags.Beam.Type is BeamType.Cosmics and
            flags.InDet.Tracking.ActivePass.extension != "DBM"):
        from InDetConfig.TrackingCommonConfig import InDetCosmicsScoringToolCfg
        InDetAmbiScoringTool = acc.popToolsAndMerge(
            InDetCosmicsScoringToolCfg(flags))
    elif (flags.InDet.Tracking.ActivePass.extension == "R3LargeD0" and
          flags.InDet.Tracking.nnCutLargeD0Threshold > 0):
        # Set up NN config
        from InDetConfig.TrackingCommonConfig import InDetNNScoringToolSiCfg
        InDetAmbiScoringTool = acc.popToolsAndMerge(
            InDetNNScoringToolSiCfg(flags))
    else:
        from InDetConfig.TrackingCommonConfig import InDetAmbiScoringToolSiCfg
        InDetAmbiScoringTool = acc.popToolsAndMerge(
            InDetAmbiScoringToolSiCfg(flags))
    kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)

    fitter_args = {}

    if flags.InDet.Tracking.holeSearchInGX2Fit:
        fitter_args.setdefault("DoHoleSearch", True)
        from InDetConfig.InDetBoundaryCheckToolConfig import (
            InDetBoundaryCheckToolCfg)
        InDetBoundaryCheckTool = acc.popToolsAndMerge(
            InDetBoundaryCheckToolCfg(flags))
        fitter_args.setdefault("BoundaryCheckTool", InDetBoundaryCheckTool)

    fitter_list = []

    if flags.InDet.Tracking.ActivePass.isLowPt:
        from InDetConfig.TrackingCommonConfig import (
            InDetTrackFitterLowPtAmbiCfg)
        InDetTrackFitterLowPt = acc.popToolsAndMerge(
            InDetTrackFitterLowPtAmbiCfg(
                flags,
                name='InDetTrackFitterLowPt'+flags.InDet.Tracking.ActivePass.extension,
                **fitter_args))
        fitter_list.append(InDetTrackFitterLowPt)
    else:
        from InDetConfig.TrackingCommonConfig import InDetTrackFitterAmbiCfg
        InDetTrackFitterAmbi = acc.popToolsAndMerge(
            InDetTrackFitterAmbiCfg(
                flags,
                name='InDetTrackFitterAmbi'+flags.InDet.Tracking.ActivePass.extension,
                **fitter_args))
        fitter_list.append(InDetTrackFitterAmbi)

    from InDetConfig.InDetAssociationToolsConfig import (
        InDetPRDtoTrackMapToolGangedPixelsCfg)
    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(
        InDetPRDtoTrackMapToolGangedPixelsCfg(flags))
    from TrkConfig.TrkTrackSummaryToolConfig import (
        InDetTrackSummaryToolAmbiCfg)
    ambi_track_summary_tool = acc.popToolsAndMerge(
        InDetTrackSummaryToolAmbiCfg(
            flags,
            name="InDetAmbiguityProcessorSplitProbTrackSummaryTool" + flags.InDet.Tracking.ActivePass.extension))

    from InDetConfig.InDetAmbiTrackSelectionToolConfig import (
        InDetAmbiTrackSelectionToolCfg)
    InDetAmbiTrackSelectionTool = acc.popToolsAndMerge(
        InDetAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", fitter_list)
    kwargs.setdefault("AssociationTool", InDetPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", 'PRDToTrackMap' +
                      flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("SelectionTool", InDetAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName",
                      'SplitProb'+flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("OutputClusterSplitProbabilityName",
                      'InDetAmbiguityProcessorSplitProb'+flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.InDet.Tracking.doBremRecovery and (
        flags.InDet.Tracking.ActivePass.extension == "" or
        flags.InDet.Tracking.ActivePass.extension == "BLS"))
    kwargs.setdefault(
        "caloSeededBrem", flags.InDet.Tracking.doCaloSeededBrem and
        flags.Detector.EnableCalo)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.ActivePass.minPTBrem)
    kwargs.setdefault("RefitPrds", True)
    kwargs.setdefault("KeepHolesFromBeforeRefit", False)

    InDetAmbiguityProcessor = CompFactory.Trk.DenseEnvironmentsAmbiguityProcessorTool(
        name=name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(InDetAmbiguityProcessor)
    return acc


def ITkDenseEnvironmentsAmbiguityProcessorToolCfg(
        flags,
        name="ITkAmbiguityProcessor",
        **kwargs):
    acc = ComponentAccumulator()

    #
    # --- set up different Scoring Tool for collisions and cosmics
    #
    if flags.Beam.Type is BeamType.Cosmics:
        from InDetConfig.ITkTrackingCommonConfig import (
            ITkCosmicsScoringToolCfg)
        ITkAmbiScoringTool = acc.popToolsAndMerge(
            ITkCosmicsScoringToolCfg(flags))
    else:
        from InDetConfig.ITkTrackingCommonConfig import (
            ITkAmbiScoringToolCfg)
        ITkAmbiScoringTool = acc.popToolsAndMerge(ITkAmbiScoringToolCfg(flags))

    fitter_args = {}
    fitter_args.setdefault("DoHoleSearch", True)

    from InDetConfig.InDetBoundaryCheckToolConfig import (
        ITkBoundaryCheckToolCfg)
    ITkBoundaryCheckTool = acc.popToolsAndMerge(ITkBoundaryCheckToolCfg(flags))
    fitter_args.setdefault("BoundaryCheckTool", ITkBoundaryCheckTool)

    from InDetConfig.ITkTrackingCommonConfig import ITkTrackFitterAmbiCfg
    fitter_list = []
    ITkTrackFitterAmbi = acc.popToolsAndMerge(
        ITkTrackFitterAmbiCfg(
            flags,
            name='ITkTrackFitterAmbi'+flags.ITk.Tracking.ActivePass.extension,
            **fitter_args))
    fitter_list.append(ITkTrackFitterAmbi)

    from InDetConfig.InDetAssociationToolsConfig import (
        ITkPRDtoTrackMapToolGangedPixelsCfg)
    ITkPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(
        ITkPRDtoTrackMapToolGangedPixelsCfg(flags))

    from TrkConfig.TrkTrackSummaryToolConfig import (
        ITkTrackSummaryToolAmbiCfg)
    ambi_track_summary_tool = acc.getPrimaryAndMerge(
        ITkTrackSummaryToolAmbiCfg(
            flags,
            name="ITkAmbiguityProcessorSplitProbTrackSummaryTool" + flags.ITk.Tracking.ActivePass.extension))

    from InDetConfig.InDetAmbiTrackSelectionToolConfig import (
        ITkAmbiTrackSelectionToolCfg)
    ITkAmbiTrackSelectionTool = acc.popToolsAndMerge(
        ITkAmbiTrackSelectionToolCfg(flags))

    kwargs.setdefault("Fitter", fitter_list)
    kwargs.setdefault("AssociationTool", ITkPRDtoTrackMapToolGangedPixels)
    kwargs.setdefault("AssociationMapName", 'ITkPRDToTrackMap' +
                      flags.ITk.Tracking.ActivePass.extension)
    kwargs.setdefault("TrackSummaryTool", ambi_track_summary_tool)
    kwargs.setdefault("ScoringTool", ITkAmbiScoringTool)
    kwargs.setdefault("SelectionTool", ITkAmbiTrackSelectionTool)
    kwargs.setdefault("InputClusterSplitProbabilityName",
                      'SplitProb'+flags.ITk.Tracking.ActivePass.extension)
    kwargs.setdefault("OutputClusterSplitProbabilityName",
                      'ITkAmbiguityProcessorSplitProb'+flags.ITk.Tracking.ActivePass.extension)
    kwargs.setdefault("SuppressHoleSearch", False)
    # Disabled for second passes in reco
    kwargs.setdefault(
        "tryBremFit", flags.ITk.Tracking.doBremRecovery and
        flags.Detector.EnableCalo and
        flags.ITk.Tracking.ActivePass.extension == "")
    kwargs.setdefault(
        "caloSeededBrem", flags.ITk.Tracking.doCaloSeededBrem and
        flags.Detector.EnableCalo)
    kwargs.setdefault("pTminBrem", flags.ITk.Tracking.ActivePass.minPTBrem[0])
    kwargs.setdefault("RefitPrds", True)
    kwargs.setdefault("KeepHolesFromBeforeRefit", False)

    # DenseEnvironmentsAmbiguityProcessorTool
    ProcessorTool = CompFactory.Trk.DenseEnvironmentsAmbiguityProcessorTool
    ITkAmbiguityProcessor = ProcessorTool(
        name=name+flags.ITk.Tracking.ActivePass.extension, **kwargs)
    acc.setPrivateTools(ITkAmbiguityProcessor)
    return acc
