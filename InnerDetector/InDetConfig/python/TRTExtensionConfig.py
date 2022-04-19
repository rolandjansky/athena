# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import BeamType
import InDetConfig.TrackingCommonConfig as TC

#///////////////////////////////////////////////////////////////////////////////////////////////
def TRT_TrackExtensionAlgCfg(flags, name = 'InDetTRT_ExtensionPhase', SiTrackCollection=None, ExtendedTracksMap="ExtendedTracksMap", TrackExtensionTool=None, **kwargs):
    acc = ComponentAccumulator()
    # set output extension map name
    OutputExtendedTracks = ExtendedTracksMap
    if TrackExtensionTool is None:
        TrackExtensionTool = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolPhaseCfg(flags))
        acc.addPublicTool(TrackExtensionTool)

    kwargs.setdefault("InputTracksLocation", SiTrackCollection)
    kwargs.setdefault("ExtendedTracksLocation", OutputExtendedTracks )
    kwargs.setdefault("TrackExtensionTool", TrackExtensionTool)

    acc.addEventAlgo(CompFactory.InDet.TRT_TrackExtensionAlg(name = name, **kwargs))
    return acc

def CompetingRIOsOnTrackToolCfg(flags, name = 'InDetCompetingRotCreator', **kwargs):
    acc = ComponentAccumulator()
    InDetCompetingTRT_DC_Tool = acc.popToolsAndMerge(TC.InDetCompetingTRT_DC_ToolCfg(flags))
    acc.addPublicTool(InDetCompetingTRT_DC_Tool)

    kwargs.setdefault("ToolForCompPixelClusters", None)
    kwargs.setdefault("ToolForCompSCT_Clusters", None)
    kwargs.setdefault("ToolForCompTRT_DriftCircles", InDetCompetingTRT_DC_Tool)
    acc.setPrivateTools(CompFactory.Trk.CompetingRIOsOnTrackTool(name = name, **kwargs))
    return acc

def DeterministicAnnealingFilterCfg(flags, name = 'InDetDAF', **kwargs):
    acc = ComponentAccumulator()
    InDetCompetingRotCreator = acc.popToolsAndMerge(CompetingRIOsOnTrackToolCfg(flags))
    acc.addPublicTool(InDetCompetingRotCreator)

    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    InDetExtrapolator = acc.getPrimaryAndMerge(InDetExtrapolatorCfg(flags))

    from InDetConfig.TrackingCommonConfig import InDetUpdatorCfg
    InDetUpdator = acc.popToolsAndMerge(InDetUpdatorCfg(flags))

    kwargs.setdefault("ToolForExtrapolation", InDetExtrapolator)
    kwargs.setdefault("ToolForCompetingROTsCreation", InDetCompetingRotCreator)
    kwargs.setdefault("ToolForUpdating", InDetUpdator)
    kwargs.setdefault("AnnealingScheme", [200., 81., 9., 4., 1., 1., 1.])
    kwargs.setdefault("DropOutlierCutValue", 1.E-7)
    kwargs.setdefault("OutlierCutValue", 0.01)
    acc.setPrivateTools(CompFactory.Trk.DeterministicAnnealingFilter(name = name, **kwargs))
    return acc

def InDetExtensionProcessorCfg(flags, SiTrackCollection=None, ExtendedTrackCollection = None, ExtendedTracksMap = None, **kwargs):
    acc = ComponentAccumulator()

    if flags.InDet.Tracking.trtExtensionType == 'DAF' :
        #
        # --- DAF Fitter setup
        #
        InDetExtensionFitter = acc.popToolsAndMerge(DeterministicAnnealingFilterCfg(flags, name = 'InDetDAF'+ flags.InDet.Tracking.ActivePass.extension))
        acc.addPublicTool(InDetExtensionFitter)
    else:
        fitter_args = {}
        if flags.InDet.Tracking.holeSearchInGX2Fit:
            fitter_args.setdefault("DoHoleSearch", True)
            from InDetConfig.InDetBoundaryCheckToolConfig import InDetBoundaryCheckToolCfg
            InDetBoundaryCheckTool = acc.popToolsAndMerge(InDetBoundaryCheckToolCfg(flags))
            fitter_args.setdefault("BoundaryCheckTool", InDetBoundaryCheckTool)

        if flags.InDet.Tracking.ActivePass.extension != "LowPt":
            InDetExtensionFitter = acc.popToolsAndMerge(TC.InDetTrackFitterCfg(flags, 'InDetTrackFitter_TRTExtension'+flags.InDet.Tracking.ActivePass.extension, **fitter_args))
            acc.addPublicTool(InDetExtensionFitter)
        else:
            InDetExtensionFitter = acc.popToolsAndMerge(TC.InDetTrackFitterLowPtCfg(flags, 'InDetTrackFitter_TRTExtension'+flags.InDet.Tracking.ActivePass.extension, **fitter_args))
            acc.addPublicTool(InDetExtensionFitter)
    #
    # --- load scoring for extension
    #
    if flags.Beam.Type is BeamType.Cosmics:
        InDetExtenScoringTool = acc.popToolsAndMerge(TC.InDetCosmicExtenScoringToolCfg(flags))
        acc.addPublicTool(InDetExtenScoringTool)
    else:
        InDetExtenScoringTool = acc.popToolsAndMerge(TC.InDetExtenScoringToolCfg(flags))
        acc.addPublicTool(InDetExtenScoringTool)
    #
    # --- get configured track extension processor
    #
    InDetTrackSummaryTool = acc.getPrimaryAndMerge(TC.InDetTrackSummaryToolCfg(flags))

    kwargs.setdefault("TrackName", SiTrackCollection)
    kwargs.setdefault("ExtensionMap", ExtendedTracksMap)
    kwargs.setdefault("NewTrackName", ExtendedTrackCollection)
    kwargs.setdefault("TrackFitter", InDetExtensionFitter)
    kwargs.setdefault("TrackSummaryTool", InDetTrackSummaryTool)
    kwargs.setdefault("ScoringTool", InDetExtenScoringTool)
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
                                       SiTrackCollection=SiTrackCollection,
                                       ExtendedTracksMap = ExtendedTracksMap,
                                       TrackExtensionTool = acc.popToolsAndMerge(TC.InDetTRT_ExtensionToolCfg(flags))))

    acc.merge(InDetExtensionProcessorCfg(flags,
                                         SiTrackCollection = SiTrackCollection,
                                         ExtendedTrackCollection = ExtendedTrackCollection,
                                         ExtendedTracksMap = ExtendedTracksMap))

    return acc
##########################################################################################################################

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
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
        from xAODTruthCnv.xAODTruthCnvConfigNew import GEN_AOD2xAODCfg
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
