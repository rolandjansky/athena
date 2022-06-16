# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# ------------------------------------------------------------
#
# ----------- 2nd iteration, outside in tracking
#
# ------------------------------------------------------------

def BackTrackingCfg(flags, InputCollections = None, TrackCollectionKeys=[] , TrackCollectionTruthKeys=[], ClusterSplitProbContainer=''):
    acc = ComponentAccumulator()
    # ------------------------------------------------------------
    #
    # ---------- TRT Seeded Tracking
    #
    # ------------------------------------------------------------

    from InDetConfig.TRT_SeededTrackFinderConfig import TRT_SeededTrackFinderCfg
    acc.merge(TRT_SeededTrackFinderCfg(flags, InputCollections = InputCollections))
    # ------------------------------------------------------------
    #
    # --- Resolve back tracking tracks
    #
    # ------------------------------------------------------------
    from TrkConfig.TrkAmbiguitySolverConfig import TrkAmbiguityScore_TRT_Cfg, TrkAmbiguitySolver_TRT_Cfg
    acc.merge(TrkAmbiguityScore_TRT_Cfg(flags))
    acc.merge(TrkAmbiguitySolver_TRT_Cfg(flags, ClusterSplitProbContainer = ClusterSplitProbContainer))

    return acc


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags as flags
    from AthenaConfiguration.TestDefaults import defaultTestFiles
    flags.Input.Files=defaultTestFiles.RDO_RUN2

    # disable calo for this test
    flags.Detector.EnableCalo = False

    numThreads=1
    flags.Concurrency.NumThreads=numThreads
    flags.Concurrency.NumConcurrentEvents=numThreads

    flags = flags.cloneAndReplace("InDet.Tracking.ActivePass","InDet.Tracking.MainPass")

    flags.lock()
    flags.dump()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(flags)

    ################################ Aditional configurations ################################
    ##
    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(flags))

    ##
    from InDetConfig.SiliconPreProcessing import InDetRecPreProcessingSiliconCfg
    top_acc.merge(InDetRecPreProcessingSiliconCfg(flags))

    ########################## TRTPreProcessing Configuration ################################

    if not flags.InDet.Tracking.doDBMstandalone:
        from InDetConfig.TRTPreProcessing import TRTPreProcessingCfg
        top_acc.merge(TRTPreProcessingCfg(flags))

    ######################################## TRTSegmentFinding Configuration ###########################################
    InputCollections = []

    from InDetConfig.TRTSegmentFindingConfig import TRTSegmentFindingCfg
    top_acc.merge(TRTSegmentFindingCfg( flags,
                                        extension = "",
                                        InputCollections = InputCollections,
                                        BarrelSegments = 'TRTSegments'))

    ########################################## BackTracking Configuration ##############################################
    TrackCollectionKeys = []

    top_acc.merge(BackTrackingCfg(  flags,
                                    InputCollections = InputCollections,
                                    TrackCollectionKeys=TrackCollectionKeys,
                                    TrackCollectionTruthKeys=[],
                                    ClusterSplitProbContainer=''))

    ####################################################################################################################

    iovsvc = top_acc.getService('IOVDbSvc')
    iovsvc.OutputLevel=5

    top_acc.printConfig()
    top_acc.store(open("test_BackTrackingConfig.pkl", "wb"))
    import sys
    if "--norun" not in sys.argv:
        sc = top_acc.run(5)
        sys.exit(not sc.isSuccess())
