# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# ------------------------------------------------------------
#
# ----------- Truth Manipulated Tracking
#
# ------------------------------------------------------------

def TruthTrackingCfg(flags):
    acc = ComponentAccumulator()

    from InDetConfig.InDetTruthAlgsConfig import InDetTruthTrackCreationCfg
    acc.merge(InDetTruthTrackCreationCfg(flags))
    return acc


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    # Disable calo for this test
    ConfigFlags.Detector.EnableCalo = False

    from AthenaConfiguration.TestDefaults import defaultTestFiles
    ConfigFlags.Input.Files = defaultTestFiles.RDO_RUN2
    ConfigFlags.lock()

    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    top_acc = MainServicesCfg(ConfigFlags)

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    top_acc.merge(PoolReadCfg(ConfigFlags))

    ################################ Aditional configurations ################################
    ##
    top_acc.merge(TruthTrackingCfg(ConfigFlags))
    if ConfigFlags.InDet.Tracking.doIdealPseudoTracking:
        from InDetConfig.TrackTruthConfig import InDetTrackTruthCfg
        top_acc.merge(InDetTrackTruthCfg(ConfigFlags, 'InDetPseudoTrackParticles', 'InDetPseudoTrackTruthCollection', 'InDetPseudoTrackTruthCollection'))

#    ConfigFlags.lock()
    ConfigFlags.dump()
    
    ComponentAccumulator.debugMode="trackCA trackEventAlgo ..."


    from AthenaCommon.Constants import DEBUG
    top_acc.foreach_component("AthEventSeq/*").OutputLevel=DEBUG
    top_acc.printConfig(withDetails=True, summariseProps=True)
    top_acc.store(open("TruthTrackingConfig.pkl", "wb"))

    import sys
    if "--norun" not in sys.argv:
        sc = top_acc.run(1)
        if sc.isFailure():
            sys.exit(-1)
