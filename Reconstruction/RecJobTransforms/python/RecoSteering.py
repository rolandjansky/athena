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
        # Check if running on legacy inputs
        if "EventInfo" not in flags.Input.Collections:
            from xAODEventInfoCnv.xAODEventInfoCnvConfig import (
                EventInfoCnvAlgCfg)
            acc.merge(EventInfoCnvAlgCfg(flags))
        log.info("---------- Configured POOL reading")

    # AOD2xAOD Truth conversion
    if flags.Input.isMC:
        from xAODTruthCnv.xAODTruthCnvConfigNew import GEN_AOD2xAODCfg
        acc.merge(GEN_AOD2xAODCfg(flags))
        log.info("---------- Configured xAODTruthCnvAlg")

    # trigger
    if flags.Reco.EnableTrigger:
        from TriggerJobOpts.TriggerRecoConfig import TriggerRecoCfg
        acc.merge(TriggerRecoCfg(flags))
        log.info("---------- Configured trigger data decoding")

    # calorimeter
    if flags.Detector.EnableCalo:
        from CaloRec.CaloRecoConfig import CaloRecoCfg
        acc.merge(CaloRecoCfg(flags))
        log.info("---------- Configured calorimeter reconstruction")

    # ID / ITk
    if flags.Reco.EnableTracking:
        from InDetConfig.TrackRecoConfig import InDetTrackRecoCfg
        acc.merge(InDetTrackRecoCfg(flags))
        log.info("---------- Configured tracking")

    # Muon
    if flags.Detector.EnableMuon:
        from MuonConfig.MuonReconstructionConfig import MuonReconstructionCfg
        acc.merge(MuonReconstructionCfg(flags))
        log.info("---------- Configured muon tracking")

    # EGamma
    if flags.Reco.EnableEgamma:
        from egammaConfig.egammaSteeringConfig import EGammaSteeringCfg
        acc.merge(EGammaSteeringCfg(flags))
        log.info("---------- Configured egamma")

    # Caching of CaloExtension for downstream 
    # Combined Performance algorithms.
    if flags.Reco.EnableCaloExtension:
        from TrackToCalo.CaloExtensionBuilderAlgCfg import (
            CaloExtensionBuilderCfg)
        acc.merge(CaloExtensionBuilderCfg(flags))
        log.info("---------- Configured track calorimeter extension builder")

    # Muon Combined
    if flags.Reco.EnableCombinedMuon and tryConfiguringAll:
        from MuonCombinedConfig.MuonCombinedReconstructionConfig import (
            MuonCombinedReconstructionCfg)
        acc.merge(MuonCombinedReconstructionCfg(flags))
        log.info("---------- Configured combined muon reconstruction")

    # PFlow
    if flags.Reco.EnablePFlow:
        from eflowRec.PFRun3Config import PFCfg
        acc.merge(PFCfg(flags))
        log.info("---------- Configured particle flow")

    # EGamma and CombinedMuon isolation
    if flags.Reco.EnableCombinedMuon or flags.Reco.EnableEgamma:
        from IsolationAlgs.IsolationSteeringConfig import IsolationSteeringCfg
        acc.merge(IsolationSteeringCfg(flags, doIsoMuon=tryConfiguringAll))
        log.info("---------- Configured isolation")

    # jets

    # btagging
    if flags.Reco.EnableBTagging and tryConfiguringAll:
        # hack to prevent btagging fragments to rename top sequence
        from AthenaCommon.ConcurrencyFlags import jobproperties
        jobproperties.ConcurrencyFlags.NumThreads = flags.Concurrency.NumThreads
        from BTagging.BTagRun3Config import BTagRecoSplitCfg
        acc.merge(BTagRecoSplitCfg(flags))
        log.info("---------- Configured btagging")

    # Tau

    # HI
    if flags.Reco.EnableHI:
        from HIRecConfig.HIRecConfig import HIRecCfg
        acc.merge(HIRecCfg(flags))
        log.info("---------- Configured Heavy Ion reconstruction")

    # setup output
    if any((flags.Output.doWriteESD,
            flags.Output.doWriteAOD,
            flags.Output.doWriteRDO)):
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
