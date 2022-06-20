# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTruthAlgs package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

def InDetPRD_MultiTruthMakerSiCfg(flags, name="InDetPRD_MultiTruthMakerSi", **kwargs):
    acc = ComponentAccumulator()

    # For pixel + SCT DetectorElementCollection used
    if flags.Detector.EnablePixel:
        from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
        acc.merge(PixelReadoutGeometryCfg(flags))
        kwargs.setdefault("PixelClusterContainerName", 'PixelClusters')
        kwargs.setdefault("SimDataMapNamePixel", 'PixelSDO_Map')
        kwargs.setdefault("TruthNamePixel", 'PRD_MultiTruthPixel')
    else:
        kwargs.setdefault("PixelClusterContainerName", "")
        kwargs.setdefault("SimDataMapNamePixel", "")
        kwargs.setdefault("TruthNamePixel", "")

    if flags.Detector.EnableSCT:
        from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
        acc.merge(SCT_ReadoutGeometryCfg(flags))
        kwargs.setdefault("SCTClusterContainerName", 'SCT_Clusters')
        kwargs.setdefault("SimDataMapNameSCT", 'SCT_SDO_Map')
        kwargs.setdefault("TruthNameSCT", 'PRD_MultiTruthSCT')
    else:
        kwargs.setdefault("SCTClusterContainerName", "")
        kwargs.setdefault("SimDataMapNameSCT", "")
        kwargs.setdefault("TruthNameSCT", "")

    kwargs.setdefault("TRTDriftCircleContainerName", "")
    kwargs.setdefault("SimDataMapNameTRT", "")
    kwargs.setdefault("TruthNameTRT", "")

    InDetPRD_MultiTruthMakerSi = CompFactory.InDet.PRD_MultiTruthMaker(name, **kwargs)
    acc.addEventAlgo(InDetPRD_MultiTruthMakerSi)
    return acc

def InDetPRD_MultiTruthMakerSiPUCfg(flags, name="InDetPRD_MultiTruthMakerSiPU", **kwargs):

    if flags.Detector.EnablePixel:
        kwargs.setdefault("PixelClusterContainerName", 'PixelPUClusters')
        kwargs.setdefault("SimDataMapNamePixel", 'Pixel_PU_SDO_Map')
        kwargs.setdefault("TruthNamePixel", 'PRD_PU_MultiTruthPixel')
    if flags.Detector.EnableSCT:
        kwargs.setdefault("SCTClusterContainerName", 'SCT_PU_Clusters')
        kwargs.setdefault("SimDataMapNameSCT", 'SCT_PU_SDO_Map')
        kwargs.setdefault("TruthNameSCT", 'PRD_PU_MultiTruthSCT')

    return InDetPRD_MultiTruthMakerSiCfg(flags, name, **kwargs)

def InDetPRD_MultiTruthMakerTRTCfg(flags, name = "InDetTRT_PRD_MultiTruthMaker", **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("PixelDetEleCollKey", "")
    kwargs.setdefault("SCTDetEleCollKey", "")
    kwargs.setdefault("PixelClusterContainerName", "")
    kwargs.setdefault("SCTClusterContainerName", "")
    kwargs.setdefault("SimDataMapNamePixel", "")
    kwargs.setdefault("SimDataMapNameSCT", "")
    kwargs.setdefault("TruthNamePixel", "")
    kwargs.setdefault("TruthNameSCT", "")

    kwargs.setdefault("TRTDriftCircleContainerName", 'TRT_DriftCircles')
    kwargs.setdefault("SimDataMapNameTRT", 'TRT_SDO_Map')
    kwargs.setdefault("TruthNameTRT", 'PRD_MultiTruthTRT')

    acc.addEventAlgo(CompFactory.InDet.PRD_MultiTruthMaker(name, **kwargs))
    return acc

def InDetPRD_MultiTruthMakerTRTPUCfg(flags, name = "InDetTRT_PRD_MultiTruthMakerPU", **kwargs):
    kwargs.setdefault("TRTDriftCircleContainerName", 'TRT_PU_DriftCircles')
    kwargs.setdefault("SimDataMapNameTRT", 'TRT_PU_SDO_Map')
    kwargs.setdefault("TruthNameTRT", 'PRD_PU_MultiTruthTRT')
    return InDetPRD_MultiTruthMakerTRTCfg(flags, name, **kwargs)

def ITkPRD_MultiTruthMakerSiCfg(flags, name="ITkPRD_MultiTruthMakerSi", **kwargs):
    acc = ComponentAccumulator()

    if flags.Detector.EnableITkPixel:
        from PixelGeoModelXml.ITkPixelGeoModelConfig import ITkPixelReadoutGeometryCfg
        acc.merge(ITkPixelReadoutGeometryCfg(flags))
        kwargs.setdefault("PixelClusterContainerName", 'ITkPixelClusters')
        kwargs.setdefault("SimDataMapNamePixel", 'ITkPixelSDO_Map')
        kwargs.setdefault("TruthNamePixel", 'PRD_MultiTruthITkPixel')
    else:
        kwargs.setdefault("PixelClusterContainerName", "")
        kwargs.setdefault("SimDataMapNamePixel", "")
        kwargs.setdefault("TruthNamePixel", "")

    if flags.Detector.EnableITkStrip:
        from StripGeoModelXml.ITkStripGeoModelConfig import ITkStripReadoutGeometryCfg
        acc.merge(ITkStripReadoutGeometryCfg(flags))
        kwargs.setdefault("SCTClusterContainerName", 'ITkStripClusters')
        kwargs.setdefault("SimDataMapNameSCT", 'ITkStripSDO_Map')
        kwargs.setdefault("TruthNameSCT", 'PRD_MultiTruthITkStrip')
    else:
        kwargs.setdefault("SCTClusterContainerName", "")
        kwargs.setdefault("SimDataMapNameSCT", "")
        kwargs.setdefault("TruthNameSCT", "")

    kwargs.setdefault("TRTDriftCircleContainerName", "")
    kwargs.setdefault("SimDataMapNameTRT", "")
    kwargs.setdefault("TruthNameTRT", "")

    kwargs.setdefault("PixelDetEleCollKey", "ITkPixelDetectorElementCollection")
    kwargs.setdefault("SCTDetEleCollKey", "ITkStripDetectorElementCollection")

    InDetPRD_MultiTruthMakerSi = CompFactory.InDet.PRD_MultiTruthMaker(name, **kwargs)
    acc.addEventAlgo(InDetPRD_MultiTruthMakerSi)
    return acc

def ITkPRD_MultiTruthMakerSiPUCfg(flags, name="ITkPRD_MultiTruthMakerSiPU", **kwargs):

    if flags.Detector.EnableITkPixel:
        kwargs.setdefault("PixelClusterContainerName", 'ITkPixelPUClusters')
        kwargs.setdefault("SimDataMapNamePixel", 'ITkPixel_PU_SDO_Map')
        kwargs.setdefault("TruthNamePixel", 'PRD_PU_MultiTruthITkPixel')
    if flags.Detector.EnableITkStrip:
        kwargs.setdefault("SCTClusterContainerName", 'ITkStripPUClusters')
        kwargs.setdefault("SimDataMapNameSCT", 'ITkStrip_PU_SDO_Map')
        kwargs.setdefault("TruthNameSCT", 'PRD_PU_MultiTruthITkStrip')

    return InDetPRD_MultiTruthMakerSiCfg(flags, name, **kwargs)

def InDetDetailedTrackTruthMakerCfg(flags, Tracks, DetailedTruth, name='Maker',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackCollectionName", Tracks)
    kwargs.setdefault("DetailedTrackTruthName", DetailedTruth)
    kwargs.setdefault("TruthNamePixel", 'PRD_MultiTruthPixel' if flags.Detector.EnablePixel else "")
    kwargs.setdefault("TruthNameSCT", 'PRD_MultiTruthSCT' if flags.Detector.EnableSCT else "")
    # for cosmics, at the stage of SiPatternRecognition, the TRT truth information is not yet available
    useTRT = flags.Detector.EnableTRT and not(flags.Beam.Type is BeamType.Cosmics and (DetailedTruth == "SiSPSeededTracksDetailedTruth" or DetailedTruth == "ResolvedTracksDetailedTruth"))
    kwargs.setdefault("TruthNameTRT", 'PRD_MultiTruthTRT' if useTRT else "")

    acc.addEventAlgo(CompFactory.InDet.InDetDetailedTrackTruthMaker(name = DetailedTruth+name, **kwargs))
    return acc

def ITkDetailedTrackTruthMakerCfg(flags, Tracks, DetailedTruth, name='Maker',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackCollectionName", Tracks)
    kwargs.setdefault("DetailedTrackTruthName", DetailedTruth)
    kwargs.setdefault("TruthNamePixel", 'PRD_MultiTruthITkPixel' if flags.Detector.EnableITkPixel else "")
    kwargs.setdefault("TruthNameSCT", 'PRD_MultiTruthITkStrip' if flags.Detector.EnableITkStrip else "")
    kwargs.setdefault("TruthNameTRT", "")

    acc.addEventAlgo(CompFactory.InDet.InDetDetailedTrackTruthMaker(name = DetailedTruth+name, **kwargs))
    return acc

def InDetTruthTrackCreationCfg(flags, name='InDetTruthTrackCreation', **kwargs):
    acc = ComponentAccumulator()

    from InDetConfig.InDetTruthToolsConfig import InDetPRD_TruthTrajectoryBuilderCfg,InDetTruthTrackBuilderCfg
    InDetPRD_TruthTrajectoryBuilder = acc.popToolsAndMerge(InDetPRD_TruthTrajectoryBuilderCfg(flags))
    acc.addPublicTool(InDetPRD_TruthTrajectoryBuilder)
    kwargs.setdefault('PRD_TruthTrajectoryBuilder', InDetPRD_TruthTrajectoryBuilder)

    InDetTruthTrackBuilder = acc.popToolsAndMerge(InDetTruthTrackBuilderCfg(flags))
    acc.addPublicTool(InDetTruthTrackBuilder)
    kwargs.setdefault('TruthTrackBuilder', InDetTruthTrackBuilder)
    kwargs.setdefault('OutputTrackCollection', 'InDetPseudoTracks')
    
    from InDetConfig.InDetAssociationToolsConfig import InDetPRDtoTrackMapToolGangedPixelsCfg
    InDetPRDtoTrackMapToolGangedPixels = acc.popToolsAndMerge(InDetPRDtoTrackMapToolGangedPixelsCfg(flags))
    kwargs.setdefault('AssociationTool', InDetPRDtoTrackMapToolGangedPixels)

    from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolSharedHitsCfg
    TrackSummaryTool = acc.popToolsAndMerge(InDetTrackSummaryToolSharedHitsCfg(flags, name='InDetTruthTrackCreationSummaryToolSharedHits'))
    acc.addPublicTool(TrackSummaryTool)
    kwargs.setdefault('TrackSummaryTool', TrackSummaryTool)

    trajectoryselectors = []
    if not flags.InDet.Tracking.doIdealPseudoTracking:
        from InDetConfig.InDetTruthToolsConfig import InDetPRD_TruthTrajectorySelectorCfg
        TruthTrajectorySelector = acc.popToolsAndMerge(InDetPRD_TruthTrajectorySelectorCfg())
        trajectoryselectors.append(TruthTrajectorySelector)
    kwargs.setdefault('PRD_TruthTrajectorySelectors', trajectoryselectors)

    acc.addEventAlgo(CompFactory.Trk.TruthTrackCreation(name, **kwargs))
    return acc
