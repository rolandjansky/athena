# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkTrackSummaryTool package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetTrackSummaryToolCfg(flags, name='InDetTrackSummaryTool', **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackingCommonConfig import ITkTrackSummaryToolCfg
        return ITkTrackSummaryToolCfg(flags, name, **kwargs)

    acc = ComponentAccumulator()
    do_holes = kwargs.get("doHolesInDet",True)

    if 'InDetSummaryHelperTool' not in kwargs:
        if do_holes:
            from InDetConfig.InDetTrackSummaryHelperToolConfig import InDetTrackSummaryHelperToolCfg
            InDetSummaryHelperTool = acc.popToolsAndMerge(InDetTrackSummaryHelperToolCfg(flags))
        else:
            from InDetConfig.InDetTrackSummaryHelperToolConfig import InDetSummaryHelperNoHoleSearchCfg
            InDetSummaryHelperTool = acc.popToolsAndMerge(InDetSummaryHelperNoHoleSearchCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperTool)

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)

    acc.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name, **kwargs))
    return acc

def InDetTrackSummaryToolAmbiCfg(flags, name='InDetTrackSummaryToolAmbi', **kwargs):
    acc = ComponentAccumulator()

    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import InDetTrackSummaryHelperToolCfg
        InDetSummaryHelperTool = acc.popToolsAndMerge(InDetTrackSummaryHelperToolCfg(flags,
                                                                                     name = "InDetAmbiguityProcessorSplitProbSummaryHelper" + flags.InDet.Tracking.ActivePass.extension,
                                                                                     ClusterSplitProbabilityName = "InDetAmbiguityProcessorSplitProb" + flags.InDet.Tracking.ActivePass.extension))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperTool)

    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetTrackSummaryTool)
    return acc

def InDetTrackSummaryToolSharedHitsCfg(flags, name='InDetTrackSummaryToolSharedHits',**kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import InDetSummaryHelperSharedHitsCfg
        InDetSummaryHelperSharedHits = acc.popToolsAndMerge(InDetSummaryHelperSharedHitsCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperSharedHits)

    kwargs.setdefault( "doSharedHits", flags.InDet.Tracking.doSharedHits)

    InDetTrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetTrackSummaryTool)
    return acc

def InDetTrackSummaryToolNoHoleSearchCfg(flags, name='InDetTrackSummaryToolNoHoleSearch',**kwargs):
    kwargs.setdefault('doHolesInDet', False)
    return InDetTrackSummaryToolCfg(flags, name, **kwargs)

def InDetTrigTrackSummaryToolCfg(flags, name="InDetTrigTrackSummaryTool", **kwargs):
    acc = ComponentAccumulator()

    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import TrigTrackSummaryHelperToolCfg
        summaryHelperTool = acc.popToolsAndMerge(TrigTrackSummaryHelperToolCfg(flags) )
        kwargs.setdefault("InDetSummaryHelperTool", summaryHelperTool)

    kwargs.setdefault("doSharedHits", True)
    kwargs.setdefault("doHolesInDet", True)

    acc.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name, **kwargs))
    return acc

def ITkTrackSummaryToolCfg(flags, name='ITkTrackSummaryTool', **kwargs):
    acc = ComponentAccumulator()
    do_holes=kwargs.get("doHolesInDet",True)

    if 'InDetSummaryHelperTool' not in kwargs:
        if do_holes:
            from  InDetConfig.InDetTrackSummaryHelperToolConfig import ITkTrackSummaryHelperToolCfg
            ITkSummaryHelperTool = acc.popToolsAndMerge(ITkTrackSummaryHelperToolCfg(flags))
        else:
            from  InDetConfig.InDetTrackSummaryHelperToolConfig import ITkSummaryHelperNoHoleSearchCfg
            ITkSummaryHelperTool = acc.popToolsAndMerge(ITkSummaryHelperNoHoleSearchCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperTool)

    #
    # Configurable version of TrkTrackSummaryTool: no TRT_PID tool needed here (no shared hits)
    #
    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)

    acc.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name, **kwargs))
    return acc

def ITkTrackSummaryToolAmbiCfg(flags, name='ITkTrackSummaryToolAmbi', **kwargs):
    acc = ComponentAccumulator()

    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import ITkTrackSummaryHelperToolCfg
        ITkSummaryHelperTool = acc.popToolsAndMerge(ITkTrackSummaryHelperToolCfg(flags,
                                                                                 ClusterSplitProbabilityName = "ITkAmbiguityProcessorSplitProb" + flags.ITk.Tracking.ActivePass.extension))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperTool)

    ITkTrackSummaryTool = acc.popToolsAndMerge(ITkTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkTrackSummaryTool)
    return acc

def ITkTrackSummaryToolSharedHitsCfg(flags, name='ITkTrackSummaryToolSharedHits',**kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import ITkSummaryHelperSharedHitsCfg
        ITkSummaryHelperSharedHits = acc.popToolsAndMerge(ITkSummaryHelperSharedHitsCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperSharedHits)

    kwargs.setdefault( "doSharedHits", flags.ITk.Tracking.doSharedHits)

    ITkTrackSummaryTool = acc.popToolsAndMerge(ITkTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkTrackSummaryTool)
    return acc

def ITkTrackSummaryToolNoHoleSearchCfg(flags, name='ITkTrackSummaryToolNoHoleSearch',**kwargs) :
    kwargs.setdefault('doHolesInDet', False)
    return ITkTrackSummaryToolCfg(flags, name, **kwargs)
