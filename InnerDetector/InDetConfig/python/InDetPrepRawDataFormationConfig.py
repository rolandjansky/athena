# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetPrepRawDataFormation package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import Format, BeamType

def ITkInDetToXAODClusterConversionCfg(flags, name="ITkInDetToXAODClusterConversion", **kwargs):
    acc = ComponentAccumulator()
    conversionAlg=CompFactory.InDet.InDetToXAODClusterConversion(name, **kwargs)
    acc.addEventAlgo(conversionAlg)
    return acc

def PixelClusterizationCfg(flags, name = "InDetPixelClusterization", **kwargs):
    acc = ComponentAccumulator()
    from InDetConfig.SiClusterizationToolConfig import MergedPixelsToolCfg, PixelGangedAmbiguitiesFinderCfg
    merged_pixels_tool = acc.popToolsAndMerge(MergedPixelsToolCfg(flags))
    ambi_finder = acc.popToolsAndMerge(PixelGangedAmbiguitiesFinderCfg(flags))

    kwargs.setdefault("clusteringTool", merged_pixels_tool)
    kwargs.setdefault("gangedAmbiguitiesFinder", ambi_finder)
    kwargs.setdefault("DataObjectName", "PixelRDOs")
    kwargs.setdefault("ClustersName", "PixelClusters")

    acc.addEventAlgo(CompFactory.InDet.PixelClusterization(name, **kwargs))
    return acc

def PixelClusterizationPUCfg(flags, name="InDetPixelClusterizationPU", **kwargs):
    kwargs.setdefault("DataObjectName", "Pixel_PU_RDOs")
    kwargs.setdefault("ClustersName", "PixelPUClusters")
    kwargs.setdefault("AmbiguitiesMap", "PixelClusterAmbiguitiesMapPU")
    return PixelClusterizationCfg(flags, name, **kwargs)

def TrigPixelClusterizationCfg(flags, name="InDetPixelClusterization", roisKey="", signature="", **kwargs):
    acc = ComponentAccumulator()
   
    # Region selector tools for Pixel
    from RegionSelector.RegSelToolConfig import regSelTool_Pixel_Cfg
    RegSelTool_Pixel = acc.popToolsAndMerge(regSelTool_Pixel_Cfg(flags))
    from InDetConfig.SiClusterizationToolConfig import TrigMergedPixelsToolCfg, PixelGangedAmbiguitiesFinderCfg
    merged_pixels_tool = acc.popToolsAndMerge(TrigMergedPixelsToolCfg(flags))
    ambi_finder = acc.popToolsAndMerge(PixelGangedAmbiguitiesFinderCfg(flags))

    kwargs.setdefault("clusteringTool", merged_pixels_tool)
    kwargs.setdefault("gangedAmbiguitiesFinder", ambi_finder)
    kwargs.setdefault("AmbiguitiesMap", "TrigPixelClusterAmbiguitiesMap")
    kwargs.setdefault("ClustersName", "PixelTrigClusters")
    kwargs.setdefault("isRoI_Seeded", True)
    kwargs.setdefault("RoIs", roisKey)
    kwargs.setdefault("ClusterContainerCacheKey", "PixelTrigClustersCache")
    kwargs.setdefault("RegSelTool", RegSelTool_Pixel)

    acc.addEventAlgo(CompFactory.InDet.PixelClusterization(name+signature, **kwargs))
    return acc

def ITkPixelClusterizationCfg(flags, name = "ITkPixelClusterization", **kwargs):
    acc = ComponentAccumulator()
    from InDetConfig.SiClusterizationToolConfig import ITkMergedPixelsToolCfg, ITkPixelGangedAmbiguitiesFinderCfg
    merged_pixels_tool = acc.popToolsAndMerge(ITkMergedPixelsToolCfg(flags, **kwargs))
    ambi_finder = acc.popToolsAndMerge(ITkPixelGangedAmbiguitiesFinderCfg(flags, **kwargs))

    kwargs.setdefault("clusteringTool", merged_pixels_tool)
    kwargs.setdefault("gangedAmbiguitiesFinder", ambi_finder)
    kwargs.setdefault("DataObjectName", "ITkPixelRDOs")
    kwargs.setdefault("ClustersName", "ITkPixelClusters")
    kwargs.setdefault("AmbiguitiesMap", "ITkPixelClusterAmbiguitiesMap")

    acc.addEventAlgo(CompFactory.InDet.PixelClusterization(name, **kwargs))
    return acc

def SCTClusterizationCfg(flags, name="InDetSCT_Clusterization", **kwargs):
    acc = ComponentAccumulator()

    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags, withFlaggedCondTool=False))

    if "SCTDetElStatus" not in kwargs :
        from SCT_ConditionsAlgorithms.SCT_ConditionsAlgorithmsConfig  import SCT_DetectorElementStatusAlgWithoutFlaggedCfg
        acc.merge( SCT_DetectorElementStatusAlgWithoutFlaggedCfg(flags) )
        kwargs.setdefault("SCTDetElStatus", "SCTDetectorElementStatusWithoutFlagged" )

    from InDetConfig.SiClusterizationToolConfig import SCT_ClusteringToolCfg
    InDetSCT_ClusteringTool = acc.popToolsAndMerge(SCT_ClusteringToolCfg(flags))

    kwargs.setdefault("clusteringTool", InDetSCT_ClusteringTool)
    kwargs.setdefault("DataObjectName", 'SCT_RDOs')
    kwargs.setdefault("ClustersName", 'SCT_Clusters')
    kwargs.setdefault("conditionsTool", InDetSCT_ConditionsSummaryToolWithoutFlagged)

    acc.addEventAlgo(CompFactory.InDet.SCT_Clusterization(name, **kwargs))
    return acc

def SCTClusterizationPUCfg(flags, name="InDetSCT_ClusterizationPU", **kwargs):
    kwargs.setdefault("DataObjectName", "SCT_PU_RDOs" )
    kwargs.setdefault("ClustersName", "SCT_PU_Clusters")
    return SCTClusterizationCfg(flags, name, **kwargs)

def TrigSCTClusterizationCfg(flags, name="InDetSCT_Clusterization", roisKey="", signature="", **kwargs):
    acc = ComponentAccumulator()

    from RegionSelector.RegSelToolConfig import regSelTool_SCT_Cfg
    RegSelTool_SCT   = acc.popToolsAndMerge(regSelTool_SCT_Cfg(flags))

    from SCT_ConditionsTools.SCT_ConditionsToolsConfig import SCT_ConditionsSummaryToolCfg
    InDetSCT_ConditionsSummaryToolWithoutFlagged = acc.popToolsAndMerge(SCT_ConditionsSummaryToolCfg(flags, withFlaggedCondTool=False, withTdaqTool=False))

    from InDetConfig.SiClusterizationToolConfig import Trig_SCT_ClusteringToolCfg
    InDetSCT_ClusteringTool = acc.popToolsAndMerge(Trig_SCT_ClusteringToolCfg(flags))

    kwargs.setdefault("clusteringTool", InDetSCT_ClusteringTool)
    kwargs.setdefault("DataObjectName", 'SCT_RDOs')
    kwargs.setdefault("ClustersName", 'SCT_TrigClusters')
    kwargs.setdefault("conditionsTool", InDetSCT_ConditionsSummaryToolWithoutFlagged)
    kwargs.setdefault("isRoI_Seeded", True)
    kwargs.setdefault("RoIs", roisKey)
    kwargs.setdefault("ClusterContainerCacheKey", "SCT_ClustersCache")
    kwargs.setdefault("FlaggedCondCacheKey", "SctFlaggedCondCache")
    kwargs.setdefault("RegSelTool", RegSelTool_SCT)

    acc.addEventAlgo(CompFactory.InDet.SCT_Clusterization(name+signature, **kwargs))
    return acc

def ITkStripClusterizationCfg(flags, name="ITkStripClusterization", **kwargs):
    acc = ComponentAccumulator()

    from SCT_ConditionsTools.ITkStripConditionsToolsConfig import ITkStripConditionsSummaryToolCfg
    ITkStripConditionsSummaryTool = acc.popToolsAndMerge(ITkStripConditionsSummaryToolCfg(flags))

    from InDetConfig.SiClusterizationToolConfig import ITKStrip_SCT_ClusteringToolCfg
    ITkStripClusteringTool = acc.popToolsAndMerge(ITKStrip_SCT_ClusteringToolCfg(flags))

    kwargs.setdefault("clusteringTool", ITkStripClusteringTool)
    kwargs.setdefault("DataObjectName", 'ITkStripRDOs')
    kwargs.setdefault("ClustersName", 'ITkStripClusters')
    kwargs.setdefault("SCT_FlaggedCondData", "ITkStripFlaggedCondData")
    kwargs.setdefault("conditionsTool", ITkStripConditionsSummaryTool)

    acc.addEventAlgo( CompFactory.InDet.SCT_Clusterization(name, **kwargs))

    return acc

def InDetTRT_RIO_MakerCfg(flags, name = "InDetTRT_RIO_Maker", **kwargs):
    acc = ComponentAccumulator()

    from InDetConfig.TRTPreProcessing import TRT_DriftCircleToolCfg
    InDetTRT_DriftCircleTool = acc.popToolsAndMerge(TRT_DriftCircleToolCfg(flags))
    kwargs.setdefault("TRT_DriftCircleTool", InDetTRT_DriftCircleTool)
    kwargs.setdefault("TrtDescrManageLocation", 'TRT')
    kwargs.setdefault("TRTRDOLocation", 'TRT_RDOs')    
    kwargs.setdefault("TRTRIOLocation", 'TRT_DriftCirclesUncalibrated' if flags.Beam.Type is BeamType.Cosmics else 'TRT_DriftCircles')

    acc.addEventAlgo(CompFactory.InDet.TRT_RIO_Maker(name, **kwargs))
    return acc

def InDetTRT_RIO_MakerPUCfg(flags, name = "InDetTRT_RIO_MakerPU", **kwargs):
    kwargs.setdefault("TRTRDOLocation", 'TRT_PU_RDOs')    
    kwargs.setdefault("TRTRIOLocation", 'TRT_PU_DriftCirclesUncalibrated' if flags.Beam.Type is BeamType.Cosmics else 'TRT_PU_DriftCircles')
    return InDetTRT_RIO_MakerCfg(flags, name, **kwargs)

def TrigTRTRIOMakerCfg(flags, name="InDetTrigMTTRTDriftCircleMaker", **kwargs):
    acc = ComponentAccumulator()

    from RegionSelector.RegSelToolConfig import regSelTool_TRT_Cfg
    RegSelTool_TRT = acc.popToolsAndMerge(regSelTool_TRT_Cfg(flags))
    from InDetConfig.TRTPreProcessing import TRT_DriftCircleToolCfg
    InDetTRT_DriftCircleTool = acc.popToolsAndMerge(TRT_DriftCircleToolCfg(flags))

    kwargs.setdefault("TRT_DriftCircleTool", InDetTRT_DriftCircleTool)
    kwargs.setdefault("TRTRIOLocation", "TRT_TrigDriftCircles")
    kwargs.setdefault("TRTRDOLocation", "TRT_RDOs_TRIG" if flags.Input.Format is Format.BS else "TRT_RDOs")
    kwargs.setdefault("isRoI_Seeded", True)
    kwargs.setdefault("RoIs", flags.InDet.Tracking.ActivePass.roi)
    kwargs.setdefault("RegSelTool", RegSelTool_TRT)
    
    acc.addEventAlgo(CompFactory.InDet.TRT_RIO_Maker(name+"_"+flags.InDet.Tracking.ActivePass.name, **kwargs))
    return acc
