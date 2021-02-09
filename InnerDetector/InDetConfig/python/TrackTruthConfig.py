# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
# -------------------------------------------------------------------------
#
# ------- fragment to handle track truth association
#
# -------------------------------------------------------------------------

def InDetDetailedTrackTruthMakerCfg(flags, Tracks, DetailedTruth, name='Maker',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackCollectionName", Tracks)
    kwargs.setdefault("DetailedTrackTruthName", DetailedTruth)
    kwargs.setdefault("TruthNamePixel", 'PRD_MultiTruthPixel')
    kwargs.setdefault("TruthNameSCT", 'PRD_MultiTruthSCT')
    kwargs.setdefault("TruthNameTRT", 'PRD_MultiTruthTRT')

    # this is how the truth maker gets to know which detector is on ...
    if (not flags.Detector.RecoPixel):
        kwargs.setdefault("TruthNamePixel", "")
    if (not flags.Detector.RecoSCT):
        kwargs.setdefault("TruthNameSCT", "")
    # for cosmics, at the stage of SiPatternRecognition, the TRT truth information is not yet available
    if ((not flags.Detector.RecoTRT) or (flags.Beam.Type == 'cosmics' and (DetailedTruth == "SiSPSeededTracksDetailedTruth" or DetailedTruth == "ResolvedTracksDetailedTruth"))):
        kwargs.setdefault("TruthNameTRT", "")

    acc.addEventAlgo(CompFactory.InDet.InDetDetailedTrackTruthMaker(name = DetailedTruth+name, **kwargs))
    return acc

def InDetTruthMatchToolCfg(flags, name='InDetTruthMatchTool', **kwargs) :
    acc = ComponentAccumulator()

    if flags.InDet.truthMatchStrategy == 'TruthMatchRatio':
        InDetTruthMatchTool = CompFactory.Trk.TruthMatchRatio
    elif flags.InDet.truthMatchStrategy == 'TruthMatchTanimoto':
        InDetTruthMatchTool = CompFactory.Trk.TruthMatchTanimoto

    kwargs.setdefault("WeightPixel", 10.)
    kwargs.setdefault("WeightSCT", 5.)
    kwargs.setdefault("WeightTRT", 1.)

    InDetTruthMatchSimilarityTool = InDetTruthMatchTool(name = name, **kwargs)
    acc.setPrivateTools(InDetTruthMatchSimilarityTool)
    return acc

def TrackTruthSimilaritySelectorCfg(flags, DetailedTruth, TracksTruth, name='Selector', **kwargs) :
    acc = ComponentAccumulator()

    InDetTruthMatchSimilarityTool = acc.popToolsAndMerge(InDetTruthMatchToolCfg(flags))
    acc.addPublicTool(InDetTruthMatchSimilarityTool)

    kwargs.setdefault("DetailedTrackTruthName", DetailedTruth)
    kwargs.setdefault("OutputName", TracksTruth)
    kwargs.setdefault("TrackTruthSimilarityTool", InDetTruthMatchSimilarityTool)

    acc.addEventAlgo(CompFactory.TrackTruthSimilaritySelector(name = TracksTruth+name, **kwargs))
    return acc

def  InDetTrackTruthCfg(flags, Tracks = None, DetailedTruth = None, TracksTruth = None):
    acc = ComponentAccumulator()
    #
    # --- Enable the detailed track truth
    #
    acc.merge(InDetDetailedTrackTruthMakerCfg(flags, Tracks, DetailedTruth))
    #
    # --- Detailed to old TrackTruth
    #
    acc.merge(TrackTruthSimilaritySelectorCfg(flags, DetailedTruth, TracksTruth))

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads # Might change this later, but good enough for the moment.

    ConfigFlags.Detector.GeometryPixel = True
    ConfigFlags.Detector.GeometrySCT = True
    ConfigFlags.Detector.GeometryTRT   = True

    ConfigFlags.InDet.doPixelClusterSplitting = True

    ConfigFlags.Detector.RecoIBL = True
    ConfigFlags.Detector.RecoPixel = True
    ConfigFlags.Detector.RecoTRT = True
    ConfigFlags.Detector.RecoSCT = True

    ConfigFlags.addFlag('InDet.useHolesFromPattern', False)

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    ####################### Aditional Configurations #########################

    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    top_acc.merge( PixelGeometryCfg(ConfigFlags) )

    from SCT_GeoModel.SCT_GeoModelConfig import SCT_GeometryCfg
    top_acc.merge(SCT_GeometryCfg(ConfigFlags))

    from TRT_GeoModel.TRT_GeoModelConfig import TRT_GeometryCfg
    top_acc.merge(TRT_GeometryCfg( ConfigFlags ))

    from BeamSpotConditions.BeamSpotConditionsConfig import BeamSpotCondAlgCfg
    top_acc.merge(BeamSpotCondAlgCfg(ConfigFlags))

    from PixelConditionsAlgorithms.PixelConditionsConfig import PixelDistortionAlgCfg, PixelHitDiscCnfgAlgCfg
    top_acc.merge(PixelDistortionAlgCfg(ConfigFlags))
    top_acc.merge(PixelHitDiscCnfgAlgCfg(ConfigFlags))

    from SiLorentzAngleTool.PixelLorentzAngleConfig import PixelLorentzAngleTool, PixelLorentzAngleCfg
    top_acc.addPublicTool(PixelLorentzAngleTool(ConfigFlags))
    top_acc.addPublicTool(top_acc.popToolsAndMerge(PixelLorentzAngleCfg(ConfigFlags)))

    from InDetOverlay.PixelOverlayConfig import PixelRawDataProviderAlgCfg
    top_acc.merge(PixelRawDataProviderAlgCfg(ConfigFlags))

    ################## SiliconPreProcessing Configurations ###################
    from InDetConfig.SiliconPreProcessing import InDetRecPreProcessingSiliconCfg
    top_acc.merge(InDetRecPreProcessingSiliconCfg(ConfigFlags))
    #################### TRTPreProcessing Configurations #####################
    from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
    top_acc.merge(TRTPreProcessingCfg(ConfigFlags,(not ConfigFlags.InDet.doTRTPhaseCalculation or ConfigFlags.Beam.Type =="collisions"),False))
    
    #//// TrackingSiPatternConfig configurations from Temporary location /////
    ################# SiSPSeededTrackFinder Configurations ###################

    InputCollections = []

    SiSPSeededTrackCollectionKey = 'SiSPSeededPixelTracks'
    ResolvedTrackCollectionKey = 'ResolvedPixelTracks'
    from InDetConfig.TRTExtensionConfig import SiSPSeededTrackFinderCfg
    top_acc.merge(SiSPSeededTrackFinderCfg( ConfigFlags,
                                            InputCollections = InputCollections, 
                                            SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))
    ##########################################################################
    #################### InDetTrackTruth Configurations ######################

    InputTrackCollection = 'SiSPSeededPixelTracks'
    InputDetailedTrackTruth = 'DetailedTrackTruth'
    InputTrackCollectionTruth = 'TrackTruthCollection'
    
    top_acc.merge(InDetTrackTruthCfg(flags=ConfigFlags,
                                     Tracks = InputTrackCollection,
                                     DetailedTruth = InputDetailedTrackTruth,
                                     TracksTruth = InputTrackCollectionTruth))
    #################################################################
    top_acc.printConfig()
    top_acc.run(25)
    top_acc.store(open("test_TrackTruthConfig.pkl", "wb"))