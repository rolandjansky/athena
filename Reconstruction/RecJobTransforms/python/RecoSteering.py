# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.Enums import Format


def RecoSteering(flags):
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
    if flags.Input.Format is Format.BS:
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
        log.info("---------- Configured AODtoxAOD Truth Conversion")

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
        log.info("---------- Configured e/gamma")

    # Caching of CaloExtension for downstream
    # Combined Performance algorithms.
    if flags.Reco.EnableCaloExtension:
        from TrackToCalo.CaloExtensionBuilderAlgCfg import (
            CaloExtensionBuilderCfg)
        acc.merge(CaloExtensionBuilderCfg(flags))
        log.info("---------- Configured track calorimeter extension builder")

    # Muon Combined
    if flags.Reco.EnableCombinedMuon:
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
        acc.merge(IsolationSteeringCfg(flags))
        log.info("---------- Configured isolation")

    # jets
    if flags.Reco.EnableJet:
        from JetRecConfig.JetRecoSteering import JetRecoSteeringCfg
        acc.merge(JetRecoSteeringCfg(flags))
        log.info("---------- Configured jets")

    # btagging
    if flags.Reco.EnableBTagging:
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

    # Setup the final post-processing
    if flags.Reco.EnablePostProcessing:
        acc.merge(RecoPostProcessingCfg(flags))
        log.info("---------- Configured post-processing")

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

    # Set up PerfMon
    if flags.PerfMon.doFastMonMT or flags.PerfMon.doFullMonMT:
        from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
        acc.merge(PerfMonMTSvcCfg(flags))
        log.info("---------- Configured PerfMon")

    return acc


def RecoPostProcessingCfg(flags):
    acc = ComponentAccumulator()
    if flags.Reco.PostProcessing.TRTAloneThinning:
        from ThinningUtils.ThinTRTStandaloneConfig import (
            ThinTRTStandaloneCfg)
        # TODO remove doTau=False when Tau are added
        acc.merge(ThinTRTStandaloneCfg(flags, doTau=False))
    if flags.Reco.PostProcessing.GeantTruthThinning:
        from ThinningUtils.ThinGeantTruthConfig import (
            ThinGeantTruthCfg)
        acc.merge(ThinGeantTruthCfg(flags))
        pass

    return acc
