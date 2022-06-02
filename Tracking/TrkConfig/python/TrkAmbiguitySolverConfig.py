# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Configuration of TrkAmbiguitySolver
# The Ambiguity Solver algorithm wraps a dedicated
# ambiguity processor tool and performs
# the ambiguity resolution step.
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def TrkAmbiguityScoreCfg(flags,
                         name="InDetAmbiguityScore",
                         SiSPSeededTrackCollectionKey=None,
                         ClusterSplitProbContainer='',
                         **kwargs):
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        from TrkConfig.TrkAmbiguityProcessorConfig import (
            DenseEnvironmentsAmbiguityScoreProcessorToolCfg)
        InDetAmbiguityScoreProcessor = acc.popToolsAndMerge(
            DenseEnvironmentsAmbiguityScoreProcessorToolCfg(
                flags,
                ClusterSplitProbContainer=ClusterSplitProbContainer))
    else:
        InDetAmbiguityScoreProcessor = None

    #
    # --- configure Ambiguity (score) solver
    #
    kwargs.setdefault("TrackInput",
                      [SiSPSeededTrackCollectionKey])
    kwargs.setdefault("TrackOutput",
                      'ScoredMapInDetAmbiguityScore' + flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("AmbiguityScoreProcessor",
                      InDetAmbiguityScoreProcessor)  # TODO: check the case when it is None object

    InDetAmbiguityScore = CompFactory.Trk.TrkAmbiguityScore(
        name=name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.addEventAlgo(InDetAmbiguityScore)
    return acc


def TrkAmbiguityScore_TRT_Cfg(
        flags,
        name='InDetTRT_SeededAmbiguityScore',
        **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("TrackInput",
                      ['TRTSeededTracks'])
    kwargs.setdefault("TrackOutput",
                      'ScoredMapInDetTRT_SeededAmbiguityScore')

    InDetAmbiguityScore = CompFactory.Trk.TrkAmbiguityScore(
        name=name,
        **kwargs)
    acc.addEventAlgo(InDetAmbiguityScore)
    return acc


def TrkAmbiguityScore_Trig_Cfg(
        flags,
        name='InDetTrig_SeededAmbiguityScore',
        **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("TrackInput",
                      [flags.InDet.Tracking.ActivePass.trkTracks_FTF])
    kwargs.setdefault("TrackOutput",
                      f"ScoreMap{flags.InDet.Tracking.ActivePass.input_name}")
    kwargs.setdefault("AmbiguityScoreProcessor", None)

    InDetAmbiguityScore = CompFactory.Trk.TrkAmbiguityScore(
        name=name, **kwargs)
    acc.addEventAlgo(InDetAmbiguityScore)
    return acc


def ITkTrkAmbiguityScoreCfg(
        flags,
        name="ITkAmbiguityScore",
        SiSPSeededTrackCollectionKey=None,
        ClusterSplitProbContainer='',
        **kwargs):
    acc = ComponentAccumulator()

    from TrkConfig.TrkAmbiguityProcessorConfig import (
        ITkDenseEnvironmentsAmbiguityScoreProcessorToolCfg)
    ITkAmbiguityScoreProcessor = acc.popToolsAndMerge(
        ITkDenseEnvironmentsAmbiguityScoreProcessorToolCfg(
            flags,
            ClusterSplitProbContainer=ClusterSplitProbContainer))

    #
    # --- configure Ambiguity (score) solver
    #
    kwargs.setdefault("TrackInput",
                      [SiSPSeededTrackCollectionKey])
    kwargs.setdefault("TrackOutput",
                      'ScoredMapITkAmbiguityScore' + flags.ITk.Tracking.ActivePass.extension)
    # TODO: check the case when it is None object
    kwargs.setdefault("AmbiguityScoreProcessor",  ITkAmbiguityScoreProcessor)

    ITkAmbiguityScore = CompFactory.Trk.TrkAmbiguityScore(
        name=name+flags.ITk.Tracking.ActivePass.extension, **kwargs)
    acc.addEventAlgo(ITkAmbiguityScore)
    return acc


def TrkAmbiguitySolverCfg(
        flags,
        name="InDetAmbiguitySolver",
        ResolvedTrackCollectionKey=None,
        ClusterSplitProbContainer='', **kwargs):
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.ActivePass.useTIDE_Ambi:
        from TrkConfig.TrkAmbiguityProcessorConfig import (
            DenseEnvironmentsAmbiguityProcessorToolCfg)
        InDetAmbiguityProcessor = acc.popToolsAndMerge(
            DenseEnvironmentsAmbiguityProcessorToolCfg(flags))
    else:
        from TrkConfig.TrkAmbiguityProcessorConfig import (
            SimpleAmbiguityProcessorToolCfg)
        InDetAmbiguityProcessor = acc.popToolsAndMerge(
            SimpleAmbiguityProcessorToolCfg(
                flags,
                ClusterSplitProbContainer=ClusterSplitProbContainer))

    #
    # --- configure Ambiguity solver
    #
    kwargs.setdefault("TrackInput", 'ScoredMapInDetAmbiguityScore' +
                      flags.InDet.Tracking.ActivePass.extension)
    kwargs.setdefault("TrackOutput", ResolvedTrackCollectionKey)
    kwargs.setdefault("AmbiguityProcessor", InDetAmbiguityProcessor)

    InDetAmbiguitySolver = CompFactory.Trk.TrkAmbiguitySolver(
        name=name+flags.InDet.Tracking.ActivePass.extension, **kwargs)
    acc.addEventAlgo(InDetAmbiguitySolver)
    return acc


def TrkAmbiguitySolver_TRT_Cfg(
        flags,
        name='InDetTRT_SeededAmbiguitySolver',
        ClusterSplitProbContainer='',
        **kwargs):
    acc = ComponentAccumulator()

    from TrkConfig.TrkAmbiguityProcessorConfig import (
        SimpleAmbiguityProcessorTool_TRT_Cfg)
    InDetTRT_SeededAmbiguityProcessor = acc.popToolsAndMerge(
        SimpleAmbiguityProcessorTool_TRT_Cfg(
            flags,
            ClusterSplitProbContainer=ClusterSplitProbContainer))
    acc.addPublicTool(InDetTRT_SeededAmbiguityProcessor)

    kwargs.setdefault("TrackInput", 'ScoredMapInDetTRT_SeededAmbiguityScore')
    kwargs.setdefault("TrackOutput", 'ResolvedTRTSeededTracks')
    kwargs.setdefault("AmbiguityProcessor", InDetTRT_SeededAmbiguityProcessor)

    InDetTRT_SeededAmbiguitySolver = CompFactory.Trk.TrkAmbiguitySolver(
        name=name, **kwargs)
    acc.addEventAlgo(InDetTRT_SeededAmbiguitySolver)
    return acc


def TrkAmbiguitySolver_Trig_Cfg(
        flags,
        name='InDetTrig_SeededAmbiguitySolver',
        **kwargs):
    acc = ComponentAccumulator()

    from TrkConfig.TrkAmbiguityProcessorConfig import (
        SimpleAmbiguityProcessorTool_Trig_Cfg)
    processorTool = acc.popToolsAndMerge(
        SimpleAmbiguityProcessorTool_Trig_Cfg(
            flags,
            name=f"InDetTrigMT_AmbiguityProcessor_{flags.InDet.Tracking.ActivePass.name}"))

    kwargs.setdefault(
        "TrackInput", f"ScoreMap{flags.InDet.Tracking.ActivePass.input_name}")
    kwargs.setdefault(
        "TrackOutput", flags.InDet.Tracking.ActivePass.trkTracks_IDTrig+"_Amb")
    kwargs.setdefault("AmbiguityProcessor", processorTool)

    AmbiguitySolver = CompFactory.Trk.TrkAmbiguitySolver(name=name, **kwargs)
    acc.addEventAlgo(AmbiguitySolver)
    return acc


def ITkTrkAmbiguitySolverCfg(
        flags,
        name="ITkAmbiguitySolver",
        ResolvedTrackCollectionKey=None, **kwargs):
    acc = ComponentAccumulator()

    from TrkConfig.TrkAmbiguityProcessorConfig import (
        ITkDenseEnvironmentsAmbiguityProcessorToolCfg)
    ITkAmbiguityProcessor = acc.popToolsAndMerge(
        ITkDenseEnvironmentsAmbiguityProcessorToolCfg(flags))

    #
    # --- configure Ambiguity solver
    #
    kwargs.setdefault("TrackInput", 'ScoredMapITkAmbiguityScore' +
                      flags.ITk.Tracking.ActivePass.extension)
    kwargs.setdefault("TrackOutput", ResolvedTrackCollectionKey)
    kwargs.setdefault("AmbiguityProcessor", ITkAmbiguityProcessor)

    ITkAmbiguitySolver = CompFactory.Trk.TrkAmbiguitySolver(
        name=name+flags.ITk.Tracking.ActivePass.extension, **kwargs)
    acc.addEventAlgo(ITkAmbiguitySolver)
    return acc
