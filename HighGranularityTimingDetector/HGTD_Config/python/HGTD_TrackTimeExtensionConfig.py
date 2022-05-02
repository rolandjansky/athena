# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ClusterTruthToolCfg(flags, name = "ClusterTruthTool", **kwargs):
    """Configures a tool to classify clusters as coming from HS/PU/... """
    acc = ComponentAccumulator()

    acc.setPrivateTools(CompFactory.HGTD.ClusterTruthTool(name, **kwargs))
    return acc

def StraightLineTOFcorrectionToolCfg(flags, name = "StraightLineTOFcorrectionTool", **kwargs):
    """Configures a tool that uses the straight line at speed of light hypothesis for the time of flight correction """
    acc = ComponentAccumulator()

    acc.setPrivateTools(CompFactory.HGTD.StraightLineTOFcorrectionTool(name, **kwargs))
    return acc

def HGTD_IterativeExtensionToolCfg(flags, name = "HGTD_IterativeExtensionTool", **kwargs):
    """Configures a tool that performs the extension of tracks from ITk to HGTD """
    acc = ComponentAccumulator()

    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    kwargs.setdefault("ExtrapolatorTool", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    from TrkConfig.TrkMeasurementUpdatorConfig import ITkUpdatorCfg
    kwargs.setdefault("UpdatorTool", acc.popToolsAndMerge(ITkUpdatorCfg(flags)))

    kwargs.setdefault("ClusterTruthTool", acc.popToolsAndMerge(ClusterTruthToolCfg(flags)))
    kwargs.setdefault("TOFCorrTool", acc.popToolsAndMerge(StraightLineTOFcorrectionToolCfg(flags)))
    kwargs.setdefault("Chi2Cut", 5.0)
    acc.setPrivateTools(CompFactory.HGTD_IterativeExtensionTool(name, **kwargs))
    return acc

def TrackTimeExtensionCfg(flags, name = "TrackTimeExtensionAlg", **kwargs):
    """Schedules an extension algorithm from ITk to HGTD """
    acc = ComponentAccumulator()

    kwargs.setdefault("TruthTool", acc.popToolsAndMerge(ClusterTruthToolCfg(flags)))
    kwargs.setdefault("TimeExtensionTool", acc.popToolsAndMerge(HGTD_IterativeExtensionToolCfg(flags)))
    kwargs.setdefault("HGTDClusterContainerName", "HGTD_Clusters")
    acc.addEventAlgo(CompFactory.HGTD.TrackTimeExtensionAlg(name, **kwargs))
    return acc
