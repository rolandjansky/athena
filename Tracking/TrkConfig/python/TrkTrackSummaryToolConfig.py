# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Configuration of TrkTrackSummaryTool package
# In the current workflow, we need to add information
# to the Trk::Track(s) as we build them.
#
# Given the set of measurement and parametersin a Trk::Track
# We always add
# - The number of hits Pixel/SCT/TRT (default)
# We optionally can add
# - Holes  in the measurement set (default False)
# - Shared Hits between tracks (default False)
# - Expected Hits in Inner/next to Inner most using
# the fitted perigee parameters (default False)
# - Muon specific additional information
#
# Note that we try to avoid the Hole search if
# is not absolutely needed as is expensive.

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def InDetTrackSummaryToolCfg(flags,
                             name='InDetTrackSummaryTool',
                             **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        return ITkTrackSummaryToolCfg(flags, name, **kwargs)

    acc = ComponentAccumulator()
    do_holes = kwargs.get("doHolesInDet", True)

    if 'InDetSummaryHelperTool' not in kwargs:
        if do_holes:
            from InDetConfig.InDetTrackSummaryHelperToolConfig import (
                InDetTrackSummaryHelperToolCfg)
            InDetSummaryHelperTool = acc.popToolsAndMerge(
                InDetTrackSummaryHelperToolCfg(flags))
        else:
            from InDetConfig.InDetTrackSummaryHelperToolConfig import (
                InDetSummaryHelperNoHoleSearchCfg)
            InDetSummaryHelperTool = acc.popToolsAndMerge(
                InDetSummaryHelperNoHoleSearchCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperTool)

    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)

    acc.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name, **kwargs))
    return acc


def InDetTrackSummaryToolAmbiCfg(flags,
                                 name='InDetTrackSummaryToolAmbi',
                                 **kwargs):
    acc = ComponentAccumulator()

    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import (
            InDetTrackSummaryHelperToolCfg)
        InDetSummaryHelperTool = acc.popToolsAndMerge(
            InDetTrackSummaryHelperToolCfg(
                flags,
                name="InDetAmbiguityProcessorSplitProbSummaryHelper" +
                flags.InDet.Tracking.ActivePass.extension,
                ClusterSplitProbabilityName="InDetAmbiguityProcessorSplitProb" + flags.InDet.Tracking.ActivePass.extension))
        kwargs.setdefault("InDetSummaryHelperTool", InDetSummaryHelperTool)

    InDetTrackSummaryTool = acc.popToolsAndMerge(
        InDetTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetTrackSummaryTool)
    return acc


def InDetTrackSummaryToolSharedHitsCfg(
        flags,
        name='InDetTrackSummaryToolSharedHits',
        **kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import (
            InDetSummaryHelperSharedHitsCfg)
        InDetSummaryHelperSharedHits = acc.popToolsAndMerge(
            InDetSummaryHelperSharedHitsCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool",
                          InDetSummaryHelperSharedHits)

    kwargs.setdefault("doSharedHits", flags.InDet.Tracking.doSharedHits)

    InDetTrackSummaryTool = acc.popToolsAndMerge(
        InDetTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetTrackSummaryTool)
    return acc


def InDetTrackSummaryToolNoHoleSearchCfg(flags,
                                         name='InDetTrackSummaryToolNoHoleSearch',
                                         **kwargs):
    kwargs.setdefault('doHolesInDet', False)
    return InDetTrackSummaryToolCfg(flags, name, **kwargs)


def InDetTrigTrackSummaryToolCfg(flags,
                                 name="InDetTrigTrackSummaryTool",
                                 **kwargs):
    acc = ComponentAccumulator()

    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import (
            TrigTrackSummaryHelperToolCfg)
        summaryHelperTool = acc.popToolsAndMerge(
            TrigTrackSummaryHelperToolCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", summaryHelperTool)

    kwargs.setdefault("doSharedHits", True)
    kwargs.setdefault("doHolesInDet", True)

    acc.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name, **kwargs))
    return acc


def ITkTrackSummaryToolCfg(flags, name='ITkTrackSummaryTool', **kwargs):
    acc = ComponentAccumulator()
    do_holes = kwargs.get("doHolesInDet", True)

    if 'InDetSummaryHelperTool' not in kwargs:
        if do_holes:
            from InDetConfig.InDetTrackSummaryHelperToolConfig import (
                ITkTrackSummaryHelperToolCfg)
            ITkSummaryHelperTool = acc.popToolsAndMerge(
                ITkTrackSummaryHelperToolCfg(flags))
        else:
            from InDetConfig.InDetTrackSummaryHelperToolConfig import (
                ITkSummaryHelperNoHoleSearchCfg)
            ITkSummaryHelperTool = acc.popToolsAndMerge(
                ITkSummaryHelperNoHoleSearchCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperTool)

    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", do_holes)

    acc.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name, **kwargs))
    return acc


def ITkTrackSummaryToolAmbiCfg(flags,
                               name='ITkTrackSummaryToolAmbi',
                               **kwargs):
    acc = ComponentAccumulator()

    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import (
            ITkTrackSummaryHelperToolCfg)
        ITkSummaryHelperTool = acc.popToolsAndMerge(
            ITkTrackSummaryHelperToolCfg(
                flags,
                ClusterSplitProbabilityName="ITkAmbiguityProcessorSplitProb" + flags.ITk.Tracking.ActivePass.extension))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperTool)

    ITkTrackSummaryTool = acc.popToolsAndMerge(
        ITkTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkTrackSummaryTool)
    return acc


def ITkTrackSummaryToolSharedHitsCfg(flags,
                                     name='ITkTrackSummaryToolSharedHits',
                                     **kwargs):
    acc = ComponentAccumulator()
    if 'InDetSummaryHelperTool' not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import (
            ITkSummaryHelperSharedHitsCfg)
        ITkSummaryHelperSharedHits = acc.popToolsAndMerge(
            ITkSummaryHelperSharedHitsCfg(flags))
        kwargs.setdefault("InDetSummaryHelperTool", ITkSummaryHelperSharedHits)

    kwargs.setdefault("doSharedHits", flags.ITk.Tracking.doSharedHits)

    ITkTrackSummaryTool = acc.popToolsAndMerge(
        ITkTrackSummaryToolCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkTrackSummaryTool)
    return acc


def ITkTrackSummaryToolNoHoleSearchCfg(flags,
                                       name='ITkTrackSummaryToolNoHoleSearch',
                                       **kwargs):
    kwargs.setdefault('doHolesInDet', False)
    return ITkTrackSummaryToolCfg(flags, name, **kwargs)


def GSFTrackSummaryToolCfg(flags,
                           name="GSFTrackSummaryTool",
                           **kwargs):
    """ The Track Summary for the GSF refitted Tracks/TrackParticles"""

    acc = ComponentAccumulator()

    if "InDetSummaryHelperTool" not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import (
            InDetTrackSummaryHelperToolCfg)
        kwargs["InDetSummaryHelperTool"] = acc.popToolsAndMerge(
            InDetTrackSummaryHelperToolCfg(
                flags,
                name="GSFBuildTrackSummaryHelperTool",
                HoleSearch=None,
                AssoTool=None
            ))

    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", False)
    kwargs.setdefault("AddExpectedHits", True)

    # Particle creator needs a public one
    acc.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name, **kwargs))
    return acc


def MuonTrackSummaryToolCfg(flags, name="MuonTrackSummaryTool", **kwargs):
    result = ComponentAccumulator()
    from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryHelperToolCfg
    track_summary_helper = result.popToolsAndMerge(
        MuonTrackSummaryHelperToolCfg(flags))
    kwargs.setdefault("MuonSummaryHelperTool", track_summary_helper)
    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("AddDetailedMuonSummary", True)
    result.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name, **kwargs))
    return result


def MuonCombinedTrackSummaryToolCfg(flags,
                                    name="CombinedMuonTrackSummary",
                                    **kwargs):

    result = ComponentAccumulator()

    if "InDetSummaryHelperTool" not in kwargs:
        from InDetConfig.InDetTrackSummaryHelperToolConfig import (
            AtlasTrackSummaryHelperToolCfg)
        kwargs.setdefault("InDetSummaryHelperTool", result.popToolsAndMerge(
            AtlasTrackSummaryHelperToolCfg(
                flags, name="CombinedMuonIDSummaryHelper", RunningTIDE_Ambi=False)))
    if "MuonSummaryHelperTool" not in kwargs:
        from MuonConfig.MuonRecToolsConfig import MuonTrackSummaryHelperToolCfg
        kwargs.setdefault("MuonSummaryHelperTool", result.popToolsAndMerge(
            MuonTrackSummaryHelperToolCfg(flags)))

    kwargs.setdefault("doSharedHits", False)
    kwargs.setdefault("doHolesInDet", True)
    kwargs.setdefault("doHolesMuon", False)
    kwargs.setdefault("AddDetailedMuonSummary", True)
    kwargs.setdefault("PixelExists", True)

    result.setPrivateTools(CompFactory.Trk.TrackSummaryTool(name, **kwargs))
    return result
