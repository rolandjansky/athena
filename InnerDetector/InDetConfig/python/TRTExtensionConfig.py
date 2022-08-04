# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType

#///////////////////////////////////////////////////////////////////////////////////////////////
def TRT_TrackExtensionAlgCfg(flags, name = 'InDetTRT_ExtensionPhase', SiTrackCollection=None, ExtendedTracksMap="ExtendedTracksMap", **kwargs):
    acc = ComponentAccumulator()

    if "TrackExtensionTool" not in kwargs:
        from InDetConfig.TRT_TrackExtensionToolConfig import TRT_TrackExtensionToolCfg
        kwargs.setdefault("TrackExtensionTool", acc.popToolsAndMerge(TRT_TrackExtensionToolCfg(flags)))

    kwargs.setdefault("InputTracksLocation", SiTrackCollection)
    kwargs.setdefault("ExtendedTracksLocation", ExtendedTracksMap)

    acc.addEventAlgo(CompFactory.InDet.TRT_TrackExtensionAlg(name = name, **kwargs))
    return acc

def InDetExtensionProcessorCfg(flags, SiTrackCollection=None, ExtendedTrackCollection = None, ExtendedTracksMap = None, **kwargs):
    acc = ComponentAccumulator()

    if "TrackFitter" not in kwargs:
        if flags.InDet.Tracking.ActivePass.extension != "LowPt":
            from TrkConfig.CommonTrackFitterConfig import InDetTrackFitterHoleSearchCfg
            InDetExtensionFitter = acc.popToolsAndMerge(InDetTrackFitterHoleSearchCfg(flags, 'InDetTrackFitter_TRTExtension'+flags.InDet.Tracking.ActivePass.extension))
        else:
            from TrkConfig.CommonTrackFitterConfig import InDetTrackFitterLowPtHoleSearchCfg
            InDetExtensionFitter = acc.popToolsAndMerge(InDetTrackFitterLowPtHoleSearchCfg(flags, 'InDetTrackFitter_TRTExtension'+flags.InDet.Tracking.ActivePass.extension))
        acc.addPublicTool(InDetExtensionFitter)
        kwargs.setdefault("TrackFitter", InDetExtensionFitter)


    #
    # --- load scoring for extension
    #
    if "ScoringTool" not in kwargs:
        if flags.Beam.Type is BeamType.Cosmics:
            from InDetConfig.InDetTrackScoringToolsConfig import InDetCosmicExtenScoringToolCfg
            InDetExtenScoringTool = acc.popToolsAndMerge(InDetCosmicExtenScoringToolCfg(flags))
        else:
            from InDetConfig.InDetTrackScoringToolsConfig import InDetExtenScoringToolCfg
            InDetExtenScoringTool = acc.popToolsAndMerge(InDetExtenScoringToolCfg(flags))
        acc.addPublicTool(InDetExtenScoringTool)
        kwargs.setdefault("ScoringTool", InDetExtenScoringTool)

    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags)))

    kwargs.setdefault("TrackName", SiTrackCollection)
    kwargs.setdefault("ExtensionMap", ExtendedTracksMap)
    kwargs.setdefault("NewTrackName", ExtendedTrackCollection)
    kwargs.setdefault("suppressHoleSearch", False)
    kwargs.setdefault("tryBremFit", flags.InDet.Tracking.doBremRecovery)
    kwargs.setdefault("caloSeededBrem", flags.InDet.Tracking.doCaloSeededBrem and flags.Detector.EnableCalo)
    kwargs.setdefault("pTminBrem", flags.InDet.Tracking.ActivePass.minPTBrem)
    kwargs.setdefault("RefitPrds", False)
    kwargs.setdefault("matEffects", flags.InDet.Tracking.materialInteractionsType if flags.InDet.Tracking.materialInteractions else 0)
    kwargs.setdefault("Cosmics", flags.Beam.Type is BeamType.Cosmics)

    acc.addEventAlgo(CompFactory.InDet.InDetExtensionProcessor("InDetExtensionProcessor" + flags.InDet.Tracking.ActivePass.extension, **kwargs))

    return acc

##########################################################################################################################
# ------------------------------------------------------------
#
# ----------- Setup TRT Extension for New tracking
#
# ------------------------------------------------------------
def NewTrackingTRTExtensionCfg(flags, SiTrackCollection = None, ExtendedTrackCollection = None, ExtendedTracksMap = None):
    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    acc = TRTPreProcessingCfg(flags)
    #
    # Track extension to TRT algorithm
    #
    acc.merge(TRT_TrackExtensionAlgCfg(flags,
                                       name = 'InDetTRT_Extension' + flags.InDet.Tracking.ActivePass.extension,
                                       SiTrackCollection = SiTrackCollection,
                                       ExtendedTracksMap = ExtendedTracksMap))

    acc.merge(InDetExtensionProcessorCfg(flags,
                                         SiTrackCollection = SiTrackCollection,
                                         ExtendedTrackCollection = ExtendedTrackCollection,
                                         ExtendedTracksMap = ExtendedTracksMap))

    return acc
##########################################################################################################################

if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads

    ConfigFlags.Detector.GeometryPixel = True 
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.Detector.GeometryTRT = True

    # Disable calo for this test
    ConfigFlags.Detector.EnableCalo = False

    ConfigFlags.InDet.Tracking.doTRTExtension = True
    ConfigFlags.InDet.Tracking.holeSearchInGX2Fit = True

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    top_acc.merge(BeamSpotCondAlgCfg(ConfigFlags))

    if "EventInfo" not in ConfigFlags.Input.Collections:
        from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
        top_acc.merge(EventInfoCnvAlgCfg(ConfigFlags))

    if ConfigFlags.Input.isMC:
        from xAODTruthCnv.xAODTruthCnvConfig import GEN_AOD2xAODCfg
        top_acc.merge(GEN_AOD2xAODCfg(ConfigFlags))

    from PixelGeoModel.PixelGeoModelConfig import PixelReadoutGeometryCfg
    from SCT_GeoModel.SCT_GeoModelConfig import SCT_ReadoutGeometryCfg
    top_acc.merge(PixelReadoutGeometryCfg(ConfigFlags))
    top_acc.merge(SCT_ReadoutGeometryCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_ReadoutGeometryCfg
    top_acc.merge(TRT_ReadoutGeometryCfg( ConfigFlags ))

    from BeamPipeGeoModel.BeamPipeGMConfig import BeamPipeGeometryCfg
    top_acc.merge(BeamPipeGeometryCfg(ConfigFlags))

    InputCollections = []

    ResolvedTracks = 'ResolvedTracks'
    InDetSpSeededTracksKey = 'SiSPSeededTracks'
    ExtendedTrackCollection = 'ExtendedTracks'
    ExtendedTracksMap = 'ExtendedTracksMap'

    #################### Additional Configuration  ########################
    #######################################################################
    ################# TRTPreProcessing Configuration ######################
    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    top_acc.merge(TRTPreProcessingCfg(ConfigFlags))

    ################ TRTSegmentFinding Configuration ######################
    from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
    top_acc.merge(TRTSegmentFindingCfg( ConfigFlags,
                                        extension = "",
                                        InputCollections = InputCollections,
                                        BarrelSegments = 'TRTSegments'))

    ############### SiliconPreProcessing Configuration ####################
    from InDetConfig.SiliconPreProcessing import InDetRecPreProcessingSiliconCfg
    top_acc.merge(InDetRecPreProcessingSiliconCfg(ConfigFlags))

    ####################### TrackingSiPattern #############################
    from InDetConfig.TrackingSiPatternConfig import TrackingSiPatternCfg
    top_acc.merge(TrackingSiPatternCfg( ConfigFlags,
                                        InputCollections = InputCollections,
                                        ResolvedTrackCollectionKey = ResolvedTracks,
                                        SiSPSeededTrackCollectionKey = InDetSpSeededTracksKey))

    ########################### TRTExtension  #############################
    top_acc.merge(NewTrackingTRTExtensionCfg(ConfigFlags,
                                             SiTrackCollection = ResolvedTracks,
                                             ExtendedTrackCollection = ExtendedTrackCollection, 
                                             ExtendedTracksMap = ExtendedTracksMap))
    #######################################################################

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5
    #
    top_acc.printConfig()
    top_acc.run(25)
    top_acc.store(open("test_TRTExtensionConfig.pkl", "wb"))
