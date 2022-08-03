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
    if "ScoringTool" not in kwargs:
        if (flags.Beam.Type is BeamType.Cosmics and
            flags.InDet.Tracking.ActivePass.extension != "DBM"):
            from InDetConfig.InDetTrackScoringToolsConfig import InDetCosmicsScoringToolCfg
            InDetAmbiScoringTool = acc.popToolsAndMerge(
                InDetCosmicsScoringToolCfg(flags))
        elif (flags.InDet.Tracking.ActivePass.extension == "R3LargeD0" and
              flags.InDet.Tracking.nnCutLargeD0Threshold > 0):
            # Set up NN config
            from InDetConfig.InDetTrackScoringToolsConfig import InDetNNScoringToolSiCfg
            InDetAmbiScoringTool = acc.popToolsAndMerge(
                InDetNNScoringToolSiCfg(flags))
        else:
            from InDetConfig.InDetTrackScoringToolsConfig import InDetAmbiScoringToolSiCfg
            InDetAmbiScoringTool = acc.popToolsAndMerge(
                InDetAmbiScoringToolSiCfg(flags))
        kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)

    if "Fitter" not in kwargs:
        if flags.InDet.Tracking.ActivePass.isLowPt:
            from TrkConfig.CommonTrackFitterConfig import InDetTrackFitterLowPtCfg
            InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterLowPtCfg(flags))
        else:
            from TrkConfig.CommonTrackFitterConfig import InDetTrackFitterCfg
            InDetTrackFitter = acc.popToolsAndMerge(InDetTrackFitterCfg(flags))
        kwargs.setdefault("Fitter", InDetTrackFitter)

    if "AssociationTool" not in kwargs:
        from InDetConfig.InDetAssociationToolsConfig import (
            InDetPRDtoTrackMapToolGangedPixelsCfg)
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(
            InDetPRDtoTrackMapToolGangedPixelsCfg(flags)))

    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import (
            InDetTrackSummaryToolAmbiCfg)
        kwargs.setdefault("TrackSummaryTool", acc.popToolsAndMerge(
            InDetTrackSummaryToolAmbiCfg(flags, name="InDetAmbiguityProcessorSplitProbTrackSummaryTool" + flags.InDet.Tracking.ActivePass.extension)))

    if "SelectionTool" not in kwargs:
        from InDetConfig.InDetAmbiTrackSelectionToolConfig import (
            InDetAmbiTrackSelectionToolCfg)
        kwargs.setdefault("SelectionTool", acc.popToolsAndMerge(
            InDetAmbiTrackSelectionToolCfg(flags)))

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

    acc.setPrivateTools(CompFactory.Trk.SimpleAmbiguityProcessorTool(
        name+flags.InDet.Tracking.ActivePass.extension, **kwargs))
    return acc


def SimpleAmbiguityProcessorTool_TRT_Cfg(
        flags,
        name='InDetTRT_SeededAmbiguityProcessor',
        ClusterSplitProbContainer="",
        **kwargs):
    acc = ComponentAccumulator()

    if "Fitter" not in kwargs:
        from TrkConfig.CommonTrackFitterConfig import InDetTrackFitterBTCfg
        kwargs.setdefault("Fitter", acc.popToolsAndMerge(
            InDetTrackFitterBTCfg(flags)))

    if "AssociationTool" not in kwargs:
        from InDetConfig.InDetAssociationToolsConfig import (
            InDetPRDtoTrackMapToolGangedPixelsCfg)
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(
            InDetPRDtoTrackMapToolGangedPixelsCfg(flags)))

    #
    # --- set up special Scoring Tool for TRT seeded tracks
    #
    if "ScoringTool" not in kwargs:
        if flags.Beam.Type is BeamType.Cosmics:
            from InDetConfig.InDetTrackScoringToolsConfig import (
                InDetCosmicScoringTool_TRTCfg)
            InDetTRT_SeededScoringTool = acc.popToolsAndMerge(
                InDetCosmicScoringTool_TRTCfg(flags))
        else:
            from InDetConfig.InDetTrackScoringToolsConfig import (
                InDetTRT_SeededScoringToolCfg)
            InDetTRT_SeededScoringTool = acc.popToolsAndMerge(
                InDetTRT_SeededScoringToolCfg(flags))
        kwargs.setdefault("ScoringTool", InDetTRT_SeededScoringTool)

    if "TrackSummaryTool" not in kwargs:
        if flags.Beam.Type is BeamType.Cosmics:
            from TrkConfig.TrkTrackSummaryToolConfig import (
                InDetTrackSummaryToolSharedHitsCfg)
            InDetTRT_SeededSummaryTool = acc.popToolsAndMerge(
                InDetTrackSummaryToolSharedHitsCfg(flags))
        else:
            from TrkConfig.TrkTrackSummaryToolConfig import (
                InDetTrackSummaryToolCfg)
            InDetTRT_SeededSummaryTool = acc.popToolsAndMerge(
                InDetTrackSummaryToolCfg(flags))
        kwargs.setdefault("TrackSummaryTool", InDetTRT_SeededSummaryTool)

    if "SelectionTool" not in kwargs:
        from InDetConfig.InDetAmbiTrackSelectionToolConfig import (
            InDetTRTAmbiTrackSelectionToolCfg)
        kwargs.setdefault("SelectionTool", acc.popToolsAndMerge(
            InDetTRTAmbiTrackSelectionToolCfg(flags)))

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

    acc.setPrivateTools(CompFactory.Trk.SimpleAmbiguityProcessorTool(name, **kwargs))
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

    if "Fitter" not in kwargs:
        from TrkConfig.TrkGlobalChi2FitterConfig import (
            InDetTrigGlobalChi2FitterCfg)
        kwargs.setdefault("Fitter", acc.popToolsAndMerge(
            InDetTrigGlobalChi2FitterCfg(flags)))

    if "ScoringTool" not in kwargs:
        from InDetConfig.InDetTrackScoringToolsConfig import (
            InDetTrigAmbiScoringToolCfg)
        kwargs.setdefault("ScoringTool", acc.popToolsAndMerge(
            InDetTrigAmbiScoringToolCfg(flags)))

    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import (
            InDetTrigTrackSummaryToolCfg)
        kwargs.setdefault("TrackSummaryTool", acc.popToolsAndMerge(
            InDetTrigTrackSummaryToolCfg(flags)))

    if "AssociationTool" not in kwargs:
        from InDetConfig.InDetAssociationToolsConfig import (
            TrigPRDtoTrackMapToolGangedPixelsCfg)
        kwargs.setdefault("AssociationTool", acc.getPrimaryAndMerge(
            TrigPRDtoTrackMapToolGangedPixelsCfg(flags)))

    if "SelectionTool" not in kwargs:
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
    if "ScoringTool" not in kwargs:
        if (flags.Beam.Type is BeamType.Cosmics and
            flags.InDet.Tracking.ActivePass.extension != "DBM"):
            from InDetConfig.InDetTrackScoringToolsConfig import InDetCosmicsScoringToolCfg
            InDetAmbiScoringTool = acc.popToolsAndMerge(
                InDetCosmicsScoringToolCfg(flags))
        elif (flags.InDet.Tracking.ActivePass.extension == "R3LargeD0"
              and flags.InDet.Tracking.nnCutLargeD0Threshold > 0):
            from InDetConfig.InDetTrackScoringToolsConfig import InDetNNScoringToolSiCfg
            InDetAmbiScoringTool = acc.popToolsAndMerge(
                InDetNNScoringToolSiCfg(flags))
        else:
            from InDetConfig.InDetTrackScoringToolsConfig import InDetAmbiScoringToolSiCfg
            InDetAmbiScoringTool = acc.popToolsAndMerge(
                InDetAmbiScoringToolSiCfg(flags))
        kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)

    if "SplitProbTool" not in kwargs:
        from InDetConfig.SiClusterizationToolConfig import (
            NnPixelClusterSplitProbToolCfg)
        kwargs.setdefault("SplitProbTool", acc.popToolsAndMerge(
            NnPixelClusterSplitProbToolCfg(flags)) if flags.InDet.Tracking.doPixelClusterSplitting else None)

    if "AssociationTool" not in kwargs:
        from InDetConfig.InDetAssociationToolsConfig import (
            InDetPRDtoTrackMapToolGangedPixelsCfg)
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(
            InDetPRDtoTrackMapToolGangedPixelsCfg(flags)))

    if "AssociationToolNotGanged" not in kwargs:
        from TrkConfig.TrkAssociationToolsConfig import PRDtoTrackMapToolCfg
        kwargs.setdefault("AssociationToolNotGanged",
                          acc.popToolsAndMerge(PRDtoTrackMapToolCfg(flags)))

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

    acc.setPrivateTools(CompFactory.Trk.DenseEnvironmentsAmbiguityScoreProcessorTool(
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
    if "ScoringTool" not in kwargs:
        if flags.Beam.Type is BeamType.Cosmics:
            from InDetConfig.InDetTrackScoringToolsConfig import (
                ITkCosmicsScoringToolCfg)
            ITkAmbiScoringTool = acc.popToolsAndMerge(
                ITkCosmicsScoringToolCfg(flags))
        else:
            from InDetConfig.InDetTrackScoringToolsConfig import (
                ITkAmbiScoringToolCfg)
            ITkAmbiScoringTool = acc.popToolsAndMerge(ITkAmbiScoringToolCfg(flags))
        kwargs.setdefault("ScoringTool", ITkAmbiScoringTool)

    if "SplitProbTool" not in kwargs:
        from InDetConfig.SiClusterizationToolConfig import (
            ITkTruthPixelClusterSplitProbToolCfg)
        kwargs.setdefault("SplitProbTool", acc.popToolsAndMerge(
            ITkTruthPixelClusterSplitProbToolCfg(flags)) if flags.ITk.Tracking.doPixelClusterSplitting else None)

    if "AssociationTool" not in kwargs:
        from InDetConfig.InDetAssociationToolsConfig import (
            ITkPRDtoTrackMapToolGangedPixelsCfg)
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(
            ITkPRDtoTrackMapToolGangedPixelsCfg(flags)))

    if "AssociationToolNotGanged" not in kwargs:
        from TrkConfig.TrkAssociationToolsConfig import (
            PRDtoTrackMapToolCfg)
        kwargs.setdefault("AssociationToolNotGanged", acc.popToolsAndMerge(
            PRDtoTrackMapToolCfg(flags)))

    kwargs.setdefault("sharedProbCut",
                      flags.ITk.Tracking.pixelClusterSplitProb1)
    kwargs.setdefault("sharedProbCut2",
                      flags.ITk.Tracking.pixelClusterSplitProb2)
    kwargs.setdefault("SplitClusterMap_new", 'SplitClusterAmbiguityMap' +
                      flags.ITk.Tracking.ActivePass.extension)
    kwargs.setdefault("AssociationMapName", 'ITkPRDToTrackMap' +
                      flags.ITk.Tracking.ActivePass.extension)
    kwargs.setdefault("InputClusterSplitProbabilityName",
                      ClusterSplitProbContainer)
    kwargs.setdefault("OutputClusterSplitProbabilityName",
                      'SplitProb'+flags.ITk.Tracking.ActivePass.extension)

    # DenseEnvironmentsAmbiguityScoreProcessorTool
    acc.setPrivateTools(CompFactory.Trk.DenseEnvironmentsAmbiguityScoreProcessorTool(
        name+flags.ITk.Tracking.ActivePass.extension, **kwargs))
    return acc


def DenseEnvironmentsAmbiguityProcessorToolCfg(
        flags,
        name="InDetAmbiguityProcessor",
        **kwargs):

    acc = ComponentAccumulator()

    # --- set up different Scoring Tool for collisions and cosmics
    if "ScoringTool" not in kwargs:
        if (flags.Beam.Type is BeamType.Cosmics and
            flags.InDet.Tracking.ActivePass.extension != "DBM"):
            from InDetConfig.InDetTrackScoringToolsConfig import InDetCosmicsScoringToolCfg
            InDetAmbiScoringTool = acc.popToolsAndMerge(
                InDetCosmicsScoringToolCfg(flags))
        elif (flags.InDet.Tracking.ActivePass.extension == "R3LargeD0" and
              flags.InDet.Tracking.nnCutLargeD0Threshold > 0):
            from InDetConfig.InDetTrackScoringToolsConfig import InDetNNScoringToolSiCfg
            InDetAmbiScoringTool = acc.popToolsAndMerge(
                InDetNNScoringToolSiCfg(flags))
        else:
            from InDetConfig.InDetTrackScoringToolsConfig import InDetAmbiScoringToolSiCfg
            InDetAmbiScoringTool = acc.popToolsAndMerge(
                InDetAmbiScoringToolSiCfg(flags))
        kwargs.setdefault("ScoringTool", InDetAmbiScoringTool)

    if "Fitter" not in kwargs:
        fitter_list = []

        if flags.InDet.Tracking.ActivePass.isLowPt:
            from TrkConfig.CommonTrackFitterConfig import (
                InDetTrackFitterLowPtAmbiCfg)
            InDetTrackFitterLowPt = acc.popToolsAndMerge(
                InDetTrackFitterLowPtAmbiCfg(
                    flags,
                    name='InDetTrackFitterLowPt'+flags.InDet.Tracking.ActivePass.extension))
            fitter_list.append(InDetTrackFitterLowPt)
        else:
            from TrkConfig.CommonTrackFitterConfig import  (
                InDetTrackFitterAmbiCfg)
            InDetTrackFitterAmbi = acc.popToolsAndMerge(
                InDetTrackFitterAmbiCfg(
                    flags,
                    name='InDetTrackFitterAmbi'+flags.InDet.Tracking.ActivePass.extension))
            fitter_list.append(InDetTrackFitterAmbi)

        kwargs.setdefault("Fitter", fitter_list)

    if "AssociationTool" not in kwargs:
        from InDetConfig.InDetAssociationToolsConfig import (
            InDetPRDtoTrackMapToolGangedPixelsCfg)
        kwargs.setdefault("AssociationTool", acc.popToolsAndMerge(
            InDetPRDtoTrackMapToolGangedPixelsCfg(flags)))

    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import (
            InDetTrackSummaryToolAmbiCfg)
        kwargs.setdefault("TrackSummaryTool", acc.popToolsAndMerge(
            InDetTrackSummaryToolAmbiCfg(
                flags,
                name="InDetAmbiguityProcessorSplitProbTrackSummaryTool" + flags.InDet.Tracking.ActivePass.extension)))

    if "SelectionTool" not in kwargs:
        from InDetConfig.InDetAmbiTrackSelectionToolConfig import (
            InDetAmbiTrackSelectionToolCfg)
        kwargs.setdefault("SelectionTool", acc.popToolsAndMerge(
            InDetAmbiTrackSelectionToolCfg(flags)))

    kwargs.setdefault("AssociationMapName", 'PRDToTrackMap' +
                      flags.InDet.Tracking.ActivePass.extension)
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
        from InDetConfig.InDetTrackScoringToolsConfig import (
            ITkCosmicsScoringToolCfg)
        ITkAmbiScoringTool = acc.popToolsAndMerge(
            ITkCosmicsScoringToolCfg(flags))
    else:
        from InDetConfig.InDetTrackScoringToolsConfig import (
            ITkAmbiScoringToolCfg)
        ITkAmbiScoringTool = acc.popToolsAndMerge(ITkAmbiScoringToolCfg(flags))

    if "Fitter" not in kwargs:
        from TrkConfig.CommonTrackFitterConfig import (
            ITkTrackFitterAmbiCfg)
        fitter_list = []
        ITkTrackFitterAmbi = acc.popToolsAndMerge(
            ITkTrackFitterAmbiCfg(
                flags,
                name='ITkTrackFitterAmbi'+flags.ITk.Tracking.ActivePass.extension))
        fitter_list.append(ITkTrackFitterAmbi)
        kwargs.setdefault("Fitter", fitter_list)


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
