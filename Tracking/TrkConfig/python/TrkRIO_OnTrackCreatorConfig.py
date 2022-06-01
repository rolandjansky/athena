# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Configuration of TrkRIO_OnTrackCreator package
# RIO (Reconstruction Input Objects) On Track (ROT)
# Represent clusters, drift circles (PrepRawData)
# with additonal detector specific calibration corrections.

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import LHCPeriod


def InDetRotCreatorCfg(flags,
                       name='InDetRotCreator',
                       **kwargs):
    if flags.Detector.GeometryITk:
        name = name.replace("InDet", "ITk")
        from InDetConfig.ITkTrackingCommonConfig import ITkRotCreatorCfg
        return ITkRotCreatorCfg(flags, name, **kwargs)

    acc = ComponentAccumulator()

    use_broad_cluster_pix = (flags.InDet.Tracking.useBroadPixClusterErrors and
                             not flags.InDet.Tracking.doDBMstandalone)
    use_broad_cluster_sct = (flags.InDet.Tracking.useBroadSCTClusterErrors and
                             not flags.InDet.Tracking.doDBMstandalone)

    if 'ToolPixelCluster' not in kwargs:
        if use_broad_cluster_pix:
            from InDetConfig.SiClusterOnTrackTool_PixelConfig import (
                InDetBroadPixelClusterOnTrackToolCfg)
            ToolPixelCluster = acc.popToolsAndMerge(
                InDetBroadPixelClusterOnTrackToolCfg(flags))
        else:
            from InDetConfig.SiClusterOnTrackTool_PixelConfig import (
                InDetPixelClusterOnTrackToolCfg)
            ToolPixelCluster = acc.popToolsAndMerge(
                InDetPixelClusterOnTrackToolCfg(flags))
        kwargs.setdefault("ToolPixelCluster", ToolPixelCluster)

    if 'ToolSCT_Cluster' not in kwargs:
        if use_broad_cluster_sct:
            from InDetConfig.SiClusterOnTrackTool_SCTStripConfig import (
                InDetBroadSCT_ClusterOnTrackToolCfg)
            ToolSCT_Cluster = acc.popToolsAndMerge(
                InDetBroadSCT_ClusterOnTrackToolCfg(flags))
        else:
            from InDetConfig.SiClusterOnTrackTool_SCTStripConfig import (
                InDetSCT_ClusterOnTrackToolCfg)
            ToolSCT_Cluster = acc.popToolsAndMerge(
                InDetSCT_ClusterOnTrackToolCfg(flags))
        kwargs.setdefault("ToolSCT_Cluster", ToolSCT_Cluster)

    if 'ToolTRT_DriftCircle' not in kwargs:
        from InDetConfig.TrackingCommonConfig import (
            InDetTRT_DriftCircleOnTrackToolCfg)
        kwargs.setdefault("ToolTRT_DriftCircle",
                          acc.popToolsAndMerge(
                              InDetTRT_DriftCircleOnTrackToolCfg(flags))
                          )

    kwargs.setdefault('Mode', 'indet')

    acc.setPrivateTools(CompFactory.Trk.RIO_OnTrackCreator(name, **kwargs))
    return acc


def InDetBroadRotCreatorCfg(flags,
                            name='InDetBroadInDetRotCreator',
                            **kwargs):
    acc = ComponentAccumulator()

    if 'ToolPixelCluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_PixelConfig import (
            InDetBroadPixelClusterOnTrackToolCfg)
        InDetBroadPixelClusterOnTrackTool = acc.popToolsAndMerge(
            InDetBroadPixelClusterOnTrackToolCfg(flags))
        kwargs.setdefault('ToolPixelCluster',
                          InDetBroadPixelClusterOnTrackTool)

    if 'ToolSCT_Cluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_SCTStripConfig import (
            InDetBroadSCT_ClusterOnTrackToolCfg)
        InDetBroadSCT_ClusterOnTrackTool = acc.popToolsAndMerge(
            InDetBroadSCT_ClusterOnTrackToolCfg(flags))
        kwargs.setdefault('ToolSCT_Cluster', InDetBroadSCT_ClusterOnTrackTool)

    if flags.Detector.EnableTRT:
        if 'ToolTRT_DriftCircle' not in kwargs:
            from InDetConfig.TrackingCommonConfig import (
                InDetBroadTRT_DriftCircleOnTrackToolCfg)
            InDetBroadTRT_DriftCircleOnTrackTool = acc.popToolsAndMerge(
                InDetBroadTRT_DriftCircleOnTrackToolCfg())
            kwargs.setdefault('ToolTRT_DriftCircle',
                              InDetBroadTRT_DriftCircleOnTrackTool)

    InDetRotCreator = acc.popToolsAndMerge(
        InDetRotCreatorCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetRotCreator)
    return acc


def InDetRefitRotCreatorCfg(flags, name='InDetRefitRotCreator', **kwargs):
    acc = ComponentAccumulator()
    default_ScaleHitUncertainty = 2.5
    ScaleHitUncertainty = kwargs.pop(
        'ScaleHitUncertainty', default_ScaleHitUncertainty)
    if flags.Detector.EnableTRT and flags.InDet.Tracking.redoTRT_LR:
        if 'ToolTRT_DriftCircle' not in kwargs:
            from InDetConfig.TrackingCommonConfig import (
                InDetTRT_DriftCircleOnTrackUniversalToolCfg)
            ToolTRT_DriftCircle = acc.popToolsAndMerge(
                InDetTRT_DriftCircleOnTrackUniversalToolCfg(
                    ScaleHitUncertainty=ScaleHitUncertainty)
            )
            kwargs.setdefault("ToolTRT_DriftCircle", ToolTRT_DriftCircle)
    InDetRotCreator = acc.popToolsAndMerge(
        InDetRotCreatorCfg(flags, name, **kwargs))
    acc.setPrivateTools(InDetRotCreator)
    return acc


def InDetRotCreatorDigitalCfg(flags,
                              name='InDetRotCreatorDigital',
                              **kwargs):
    acc = ComponentAccumulator()
    if 'ToolPixelCluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_PixelConfig import (
            InDetPixelClusterOnTrackToolDigitalCfg)
        ToolPixelCluster = acc.popToolsAndMerge(
            InDetPixelClusterOnTrackToolDigitalCfg(flags))
        kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)

    acc.setPrivateTools(
        acc.popToolsAndMerge(
            InDetRotCreatorCfg(flags, name=name, **kwargs)))
    return acc


def InDetRotCreatorDBMCfg(flags, name='InDetRotCreatorDBM', **kwargs):
    acc = ComponentAccumulator()
    if 'ToolPixelCluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_PixelConfig import (
            InDetPixelClusterOnTrackToolDBMCfg)
        ToolPixelCluster = InDetPixelClusterOnTrackToolDBMCfg(flags)
        kwargs.setdefault('ToolPixelCluster', ToolPixelCluster)
    acc.setPrivateTools(
        acc.popToolsAndMerge(InDetRotCreatorCfg(flags, name=name, **kwargs)))
    return acc


def TrigRotCreatorCfg(flags, name='InDetTrigRotCreator', **kwargs):
    acc = ComponentAccumulator()

    if 'ToolPixelCluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_PixelConfig import (
            TrigPixelClusterOnTrackToolBaseCfg)
        ToolPixelCluster = acc.popToolsAndMerge(
            TrigPixelClusterOnTrackToolBaseCfg(flags))
        kwargs.setdefault("ToolPixelCluster", ToolPixelCluster)

    if 'ToolSCT_Cluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_SCTStripConfig import (
            InDetSCT_ClusterOnTrackToolCfg)
        ToolSCT_Cluster = acc.popToolsAndMerge(
            InDetSCT_ClusterOnTrackToolCfg(flags))
        kwargs.setdefault("ToolSCT_Cluster", ToolSCT_Cluster)

    kwargs.setdefault('Mode', 'indet')

    acc.setPrivateTools(CompFactory.Trk.RIO_OnTrackCreator(name, **kwargs))
    return acc


def ITkRotCreatorCfg(flags, name='ITkRotCreator', **kwargs):
    acc = ComponentAccumulator()

    if 'ToolPixelCluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_PixelConfig import (
            ITkPixelClusterOnTrackToolCfg)
        ToolPixelCluster = acc.popToolsAndMerge(
            ITkPixelClusterOnTrackToolCfg(flags))
        kwargs.setdefault("ToolPixelCluster", ToolPixelCluster)

    if 'ToolSCT_Cluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_SCTStripConfig import (
            ITkStripClusterOnTrackToolCfg)
        ToolStripCluster = acc.popToolsAndMerge(
            ITkStripClusterOnTrackToolCfg(flags))
        kwargs.setdefault("ToolSCT_Cluster", ToolStripCluster)

    kwargs.setdefault("ToolTRT_DriftCircle", None)
    kwargs.setdefault('Mode', 'indet')

    acc.setPrivateTools(CompFactory.Trk.RIO_OnTrackCreator(name, **kwargs))
    return acc


def ITkBroadRotCreatorCfg(flags, name='ITkBroadRotCreator', **kwargs):
    acc = ComponentAccumulator()

    if 'ToolPixelCluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_PixelConfig import (
            ITkBroadPixelClusterOnTrackToolCfg)
        ITkBroadPixelClusterOnTrackTool = acc.popToolsAndMerge(
            ITkBroadPixelClusterOnTrackToolCfg(flags))
        kwargs.setdefault('ToolPixelCluster', ITkBroadPixelClusterOnTrackTool)

    if 'ToolSCT_Cluster' not in kwargs:
        from InDetConfig.SiClusterOnTrackTool_SCTStripConfig import (
            ITkBroadStripClusterOnTrackToolCfg)
        ITkBroadStripClusterOnTrackTool = acc.popToolsAndMerge(
            ITkBroadStripClusterOnTrackToolCfg(flags))
        kwargs.setdefault('ToolSCT_Cluster', ITkBroadStripClusterOnTrackTool)

    ITkRotCreator = acc.popToolsAndMerge(
        ITkRotCreatorCfg(flags, name, **kwargs))
    acc.setPrivateTools(ITkRotCreator)
    return acc


def MuonRotCreatorCfg(flags, name="MuonRotCreator", **kwargs):
    result = ComponentAccumulator()

    from MuonConfig.MuonRIO_OnTrackCreatorToolConfig import (
        MdtDriftCircleOnTrackCreatorCfg, MuonClusterOnTrackCreatorCfg)
    mdt_rot_creator = result.popToolsAndMerge(
        MdtDriftCircleOnTrackCreatorCfg(flags))
    cluster_rot_creator = result.popToolsAndMerge(
        MuonClusterOnTrackCreatorCfg(flags))

    kwargs.setdefault("ToolMuonDriftCircle", mdt_rot_creator)
    kwargs.setdefault("ToolMuonCluster", cluster_rot_creator)
    kwargs.setdefault("ToolPixelCluster", None)
    kwargs.setdefault("ToolSCT_Cluster", None)
    kwargs.setdefault("ToolTRT_DriftCircle", None)
    kwargs.setdefault("Mode", 'muon')

    muon_rot_creator = CompFactory.Trk.RIO_OnTrackCreator(name, **kwargs)
    result.setPrivateTools(muon_rot_creator)
    return result


def RIO_OnTrackErrorScalingCondAlgCfg(flags,
                                      name='RIO_OnTrackErrorScalingCondAlg',
                                      **kwargs):
    acc = ComponentAccumulator()

    if flags.GeoModel.Run is LHCPeriod.Run1:
        error_scaling_type = ["PixelRIO_OnTrackErrorScalingRun1"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]
    else:  # Run 2 and 3
        error_scaling_type = ["PixelRIO_OnTrackErrorScaling"]
        error_scaling_outkey = ["/Indet/TrkErrorScalingPixel"]

    error_scaling_type += ["SCTRIO_OnTrackErrorScaling"]
    error_scaling_outkey += ["/Indet/TrkErrorScalingSCT"]

    # TODO: cover Run 4 and beyond

    if flags.GeoModel.Run <= LHCPeriod.Run3:
        error_scaling_type += ["TRTRIO_OnTrackErrorScaling"]
        error_scaling_outkey += ["/Indet/TrkErrorScalingTRT"]

    from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
    acc.merge(addFoldersSplitOnline(flags,
                                    'INDET',
                                    '/Indet/Onl/TrkErrorScaling',
                                    '/Indet/TrkErrorScaling',
                                    className="CondAttrListCollection"))

    kwargs.setdefault("ReadKey", "/Indet/TrkErrorScaling")
    kwargs.setdefault("ErrorScalingType", error_scaling_type)
    kwargs.setdefault("OutKeys", error_scaling_outkey)

    acc.addCondAlgo(CompFactory.RIO_OnTrackErrorScalingCondAlg(name, **kwargs))
    return acc
