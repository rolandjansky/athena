# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory     import CompFactory
# -------------------------------------------------------------------------
#
# ------- fragment to handle track truth association
#
# -------------------------------------------------------------------------

def ITkDetailedTrackTruthMakerCfg(flags, Tracks, DetailedTruth, name='Maker',**kwargs) :
    acc = ComponentAccumulator()
    kwargs.setdefault("TrackCollectionName", Tracks)
    kwargs.setdefault("DetailedTrackTruthName", DetailedTruth)
    kwargs.setdefault("TruthNamePixel", 'PRD_MultiTruthITkPixel')
    kwargs.setdefault("TruthNameSCT", 'PRD_MultiTruthITkStrip')
    kwargs.setdefault("TruthNameTRT", '')

    # this is how the truth maker gets to know which detector is on ...
    if (not flags.Detector.EnableITkPixel):
        kwargs.setdefault("TruthNamePixel", "")
    if (not flags.Detector.EnableITkStrip):
        kwargs.setdefault("TruthNameSCT", "")

    acc.addEventAlgo(CompFactory.InDet.InDetDetailedTrackTruthMaker(name = DetailedTruth+name, **kwargs))
    return acc

def ITkTruthMatchToolCfg(flags, name='ITkTruthMatchTool', **kwargs) :
    acc = ComponentAccumulator()

    kwargs.setdefault("WeightPixel", 10.)
    kwargs.setdefault("WeightSCT", 5.)

    ITkTruthMatchSimilarityTool = CompFactory.Trk.TruthMatchRatio(name = name, **kwargs)
    acc.setPrivateTools(ITkTruthMatchSimilarityTool)
    return acc

def ITkTrackTruthSimilaritySelectorCfg(flags, DetailedTruth, TracksTruth, name='Selector', **kwargs) :
    acc = ComponentAccumulator()

    ITkTruthMatchSimilarityTool = acc.popToolsAndMerge(ITkTruthMatchToolCfg(flags))

    kwargs.setdefault("DetailedTrackTruthName", DetailedTruth)
    kwargs.setdefault("OutputName", TracksTruth)
    kwargs.setdefault("TrackTruthSimilarityTool", ITkTruthMatchSimilarityTool)

    acc.addEventAlgo(CompFactory.TrackTruthSimilaritySelector(name = TracksTruth+name, **kwargs))
    return acc

def ITkTrackTruthCfg(flags, Tracks = "CombinedITkTracks", DetailedTruth = "CombinedITkTracksDetailedTruth", TracksTruth = "CombinedITkTracksTruthCollection"):
    acc = ComponentAccumulator()
    #
    # --- Enable the detailed track truth
    #
    acc.merge(ITkDetailedTrackTruthMakerCfg(flags, Tracks, DetailedTruth))
    #
    # --- Detailed to old TrackTruth
    #
    acc.merge(ITkTrackTruthSimilaritySelectorCfg(flags, DetailedTruth, TracksTruth))

    return acc


if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    numThreads=1
    ConfigFlags.Concurrency.NumThreads=numThreads
    ConfigFlags.Concurrency.NumConcurrentEvents=numThreads # Might change this later, but good enough for the moment.

    ConfigFlags.Detector.GeometryITkPixel = True
    ConfigFlags.Detector.GeometryITkStrip = True

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.lock()
    ConfigFlags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    ################## SiliconPreProcessing Configurations ###################
    from InDetConfig.ITkSiliconPreProcessing import ITkRecPreProcessingSiliconCfg
    top_acc.merge(ITkRecPreProcessingSiliconCfg(ConfigFlags))
    
    #//// TrackingSiPatternConfig configurations from Temporary location /////
    ################# SiSPSeededTrackFinder Configurations ###################

    InputCollections = []

    SiSPSeededTrackCollectionKey = 'SiSPSeededPixelTracks'
    ResolvedTrackCollectionKey = 'ResolvedPixelTracks'
    from InDetConfig.ITkTrackingSiPatternConfig import ITkSiSPSeededTrackFinderCfg
    top_acc.merge(ITkSiSPSeededTrackFinderCfg( ConfigFlags,
                                               InputCollections = InputCollections, 
                                               SiSPSeededTrackCollectionKey = SiSPSeededTrackCollectionKey))
    ##########################################################################
    #################### InDetTrackTruth Configurations ######################

    InputTrackCollection = 'SiSPSeededPixelTracks'
    InputDetailedTrackTruth = 'DetailedTrackTruth'
    InputTrackCollectionTruth = 'TrackTruthCollection'
    
    top_acc.merge(ITkTrackTruthCfg(flags=ConfigFlags,
                                   Tracks = InputTrackCollection,
                                   DetailedTruth = InputDetailedTrackTruth,
                                   TracksTruth = InputTrackCollectionTruth))
    #################################################################
    top_acc.printConfig()
    top_acc.run(25)
    top_acc.store(open("test_TrackTruthConfig.pkl", "wb"))
