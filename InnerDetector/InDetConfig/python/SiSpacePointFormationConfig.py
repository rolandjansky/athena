# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of SiSpacePointFormation and SiSpacePointTool packages

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def InDetSiElementPropertiesTableCondAlgCfg(flags, name="InDetSiElementPropertiesTableCondAlg", **kwargs):
    # For SCT DetectorElementCollection used
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc = SCT_ReadoutGeometryCfg(flags)

    acc.addCondAlgo(CompFactory.InDet.SiElementPropertiesTableCondAlg(name, **kwargs))
    return acc

def ITkSiElementPropertiesTableCondAlgCfg(flags, name="ITkSiElementPropertiesTableCondAlg", **kwargs):
    # For strip DetectorElementCollection used
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    acc = ITkStripReadoutGeometryCfg(flags)
    
    kwargs.setdefault("ReadKey", "ITkStripDetectorElementCollection")
    kwargs.setdefault("WriteKey", "ITkStripElementPropertiesTable")

    acc.addCondAlgo(CompFactory.InDet.SiElementPropertiesTableCondAlg(name, **kwargs))
    return acc

def SiSpacePointMakerToolCfg(flags, name="InDetSiSpacePointMakerTool", **kwargs):
    acc = ComponentAccumulator()
    if flags.Beam.Type is BeamType.Cosmics or flags.InDet.Tracking.doBeamGas:
        kwargs.setdefault("StripLengthTolerance", 0.05)
    acc.setPrivateTools(CompFactory.InDet.SiSpacePointMakerTool(name, **kwargs))
    return acc

def ITkSiSpacePointMakerToolCfg(flags, name="ITkSiSpacePointMakerTool", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SCTGapParameter", 0.0015)
    acc.setPrivateTools(CompFactory.InDet.SiSpacePointMakerTool(name, **kwargs))
    return acc

def InDetSiTrackerSpacePointFinderCfg(flags, name = "InDetSiTrackerSpacePointFinder", **kwargs):
    # For SCT DetectorElementCollection used
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc = SCT_ReadoutGeometryCfg(flags)

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc.merge(BeamSpotCondAlgCfg(flags))
    acc.merge(InDetSiElementPropertiesTableCondAlgCfg(flags))

    InDetSiSpacePointMakerTool = acc.popToolsAndMerge(SiSpacePointMakerToolCfg(flags))

    kwargs.setdefault("SiSpacePointMakerTool", InDetSiSpacePointMakerTool)
    kwargs.setdefault("PixelsClustersName", 'PixelClusters')
    kwargs.setdefault("SCT_ClustersName", 'SCT_Clusters')
    kwargs.setdefault("SpacePointsPixelName", 'PixelSpacePoints')
    kwargs.setdefault("SpacePointsSCTName", 'SCT_SpacePoints')
    kwargs.setdefault("SpacePointsOverlapName", 'OverlapSpacePoints')
    kwargs.setdefault("ProcessPixels", flags.Detector.EnablePixel)
    kwargs.setdefault("ProcessSCTs", flags.Detector.EnableSCT)
    kwargs.setdefault("ProcessOverlaps", flags.Detector.EnableSCT)

    if flags.InDet.Tracking.doDBMstandalone:
        kwargs.setdefault("OverlapLimitEtaMax", 5.0)
        kwargs.setdefault("OverlapLimitEtaMin", 0)

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("ProcessOverlaps", False)
        kwargs.setdefault("OverrideBeamSpot", True)
        kwargs.setdefault("VertexZ", 0)
        kwargs.setdefault("VertexX", 0)
        kwargs.setdefault("VertexY", 99999999)
        kwargs.setdefault("OverlapLimitOpposite", 5)

    acc.addEventAlgo(CompFactory.InDet.SiTrackerSpacePointFinder(name, **kwargs))
    return acc

def TrigSiTrackerSpacePointFinderCfg(flags, name = "InDetTrigSiTrackerSpacePointFinder", **kwargs):
    # For SCT DetectorElementCollection used
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    acc = SCT_ReadoutGeometryCfg(flags)

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc.merge(BeamSpotCondAlgCfg(flags))
    acc.merge(InDetSiElementPropertiesTableCondAlgCfg(flags))

    InDetSiSpacePointMakerTool = acc.popToolsAndMerge(SiSpacePointMakerToolCfg(flags))

    kwargs.setdefault("SiSpacePointMakerTool", InDetSiSpacePointMakerTool)
    kwargs.setdefault("PixelsClustersName", 'PixelTrigClusters')
    kwargs.setdefault("SCT_ClustersName", 'SCT_TrigClusters')
    kwargs.setdefault("SpacePointsPixelName", 'PixelTrigSpacePoints')
    kwargs.setdefault("SpacePointsSCTName", 'SCT_TrigSpacePoints')
    kwargs.setdefault("SpacePointsOverlapName", 'OverlapSpacePoints')
    kwargs.setdefault("ProcessPixels", flags.Detector.EnablePixel)
    kwargs.setdefault("ProcessSCTs", flags.Detector.EnableSCT)
    kwargs.setdefault("ProcessOverlaps", flags.Detector.EnableSCT)
    kwargs.setdefault("SpacePointCacheSCT", "SctSpacePointCache")
    kwargs.setdefault("SpacePointCachePix", "PixelSpacePointCache")

    acc.addEventAlgo(CompFactory.InDet.SiTrackerSpacePointFinder(name, **kwargs))
    return acc

def ITkSiTrackerSpacePointFinderCfg(flags, name = "ITkSiTrackerSpacePointFinder", **kwargs):
    # For strip DetectorElementCollection used
    from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
    acc = ITkStripReadoutGeometryCfg(flags)

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    acc.merge(BeamSpotCondAlgCfg(flags))
    acc.merge(ITkSiElementPropertiesTableCondAlgCfg(flags))

    ITkSiSpacePointMakerTool = acc.popToolsAndMerge(ITkSiSpacePointMakerToolCfg(flags))

    kwargs.setdefault("SiSpacePointMakerTool", ITkSiSpacePointMakerTool)
    kwargs.setdefault("PixelsClustersName", 'ITkPixelClusters')
    kwargs.setdefault("SCT_ClustersName", 'ITkStripClusters')
    kwargs.setdefault("SCTPropertiesKey", "ITkStripElementPropertiesTable")
    kwargs.setdefault("SCTDetEleCollKey", "ITkStripDetectorElementCollection")
    kwargs.setdefault("SpacePointsPixelName", 'ITkPixelSpacePoints')
    kwargs.setdefault("SpacePointsSCTName", 'ITkStripSpacePoints')
    kwargs.setdefault("SpacePointsOverlapName", 'ITkOverlapSpacePoints')
    kwargs.setdefault("ProcessPixels", flags.Detector.EnableITkPixel)
    kwargs.setdefault("ProcessSCTs", flags.Detector.EnableITkStrip and (not flags.ITk.Tracking.doFastTracking or flags.ITk.Tracking.doLargeD0))
    kwargs.setdefault("ProcessOverlaps", flags.Detector.EnableITkStrip and (not flags.ITk.Tracking.doFastTracking or flags.ITk.Tracking.doLargeD0))

    if flags.Beam.Type is BeamType.Cosmics:
        kwargs.setdefault("ProcessOverlaps", False)
        kwargs.setdefault("OverrideBeamSpot", True)
        kwargs.setdefault("VertexZ", 0)
        kwargs.setdefault("VertexX", 0)
        kwargs.setdefault("VertexY", 99999999)
        kwargs.setdefault("OverlapLimitOpposite", 5)

    acc.addEventAlgo(CompFactory.InDet.SiTrackerSpacePointFinder(name, **kwargs))
    return acc
