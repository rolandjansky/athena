# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def RecoSteering(flags, tryConfiguringAll=False):
    """
    Generates configuration of the reconstructions

    This driver configures all reconstruction steps unconditionally.
    The selftest available below can be used for simple jobs, 
    yet full functionality is achieved with transforms that set many flags.
    """
    from AthenaCommon.Logging import logging
    log = logging.getLogger("RecoSteering")
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg
    acc = MainServicesCfg(flags)

    # setup input
    if flags.Input.Format == 'BS':
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        acc.merge(ByteStreamReadCfg(flags))
        log.info("---------- Configured BS reading")
    else:
        from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
        acc.merge(PoolReadCfg(flags))
        log.info("---------- Configured POOL reading")

    # calorimeter
    if flags.Reco.EnableCalo:
        from CaloRec.CaloRecoConfig import CaloRecoCfg
        acc.merge(CaloRecoCfg(flags, doLCCalib=True))
        log.info("---------- Configured calorimeter reconstruction")

    # ID / ITk
    if flags.Reco.EnableTracking:
        if flags.Detector.GeometryID:
            from InDetConfig.TrackRecoConfig import TrackRecoCfg
            acc.merge(TrackRecoCfg(flags))
        elif flags.Detector.GeometryITk:
            from InDetConfig.ITkTrackRecoConfig import ITkTrackRecoCfg
            acc.merge(ITkTrackRecoCfg(flags))
            return acc  # stop here for now with ITk
        log.info("---------- Configured tracking")

    # muons
    if flags.Reco.EnableMuon:
        from MuonConfig.MuonReconstructionConfig import MuonReconstructionCfg
        acc.merge(MuonReconstructionCfg(flags))
        log.info("---------- Configured muon tracking")

    if flags.Reco.EnableCombinedMuon and tryConfiguringAll:
        from MuonCombinedConfig.MuonCombinedReconstructionConfig import MuonCombinedReconstructionCfg
        acc.merge(MuonCombinedReconstructionCfg(flags))
        log.info("---------- Configured combined muon reconstruction")

    # Caching of CaloExtension for downstream Combined Performance algorithms.
    if flags.Detector.GeometryID:
        from TrackToCalo.CaloExtensionBuilderAlgCfg import CaloExtensionBuilderAlgCfg
        acc.merge(CaloExtensionBuilderAlgCfg(flags))
    elif flags.Detector.GeometryITk:
        from TrackToCalo.ITkCaloExtensionBuilderAlgCfg import ITkCaloExtensionBuilderAlgCfg
        acc.merge(ITkCaloExtensionBuilderAlgCfg(flags))
    log.info("---------- Configured track calorimeter extension builder")

    if flags.Reco.EnableEgamma:
        from egammaConfig.egammaSteeringConfig import EGammaSteeringCfg
        acc.merge(EGammaSteeringCfg(flags))
        log.info("---------- Configured egamma")

    if flags.Reco.EnablePFlow:
        from eflowRec.PFRun3Config import PFCfg
        acc.merge(PFCfg(flags))
        log.info("---------- Configured particle flow")

    # jets
    # btagging
    if flags.Reco.EnableBTagging and tryConfiguringAll:
        # hack to prevent btagging fragments to rename top sequence
        from AthenaCommon.ConcurrencyFlags import jobproperties
        jobproperties.ConcurrencyFlags.NumThreads = flags.Concurrency.NumThreads
        from BTagging.BTagRun3Config import BTagRecoSplitCfg
        acc.merge(BTagRecoSplitCfg(flags))
        log.info("---------- Configured btagging")

    # setup output
    if any((flags.Output.doWriteESD, flags.Output.doWriteAOD, flags.Output.doWriteRDO)):
        from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
        acc.merge(PoolWriteCfg(flags))
        log.info("setup POOL format writing")

    if flags.Output.doWriteESD:
        log.info("ESD ItemList: %s", acc.getEventAlgo(
            "OutputStreamESD").ItemList)
        log.info("---------- Configured ESD writing")

    if flags.Output.doWriteAOD:
        log.info("ESD ItemList: %s", acc.getEventAlgo(
            "OutputStreamAOD").ItemList)
        log.info("---------- Configured AOD writing")

    return acc
