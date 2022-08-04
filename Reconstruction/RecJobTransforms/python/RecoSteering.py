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
    acc.flagPerfmonDomain('IO')
    if flags.Input.Format is Format.BS:
        from ByteStreamCnvSvc.ByteStreamConfig import ByteStreamReadCfg
        acc.merge(ByteStreamReadCfg(flags))
        # Decorate EventInfo obj with Beam Spot information
        from xAODEventInfoCnv.EventInfoBeamSpotDecoratorAlgConfig import (
            EventInfoBeamSpotDecoratorAlgCfg)
        acc.merge(EventInfoBeamSpotDecoratorAlgCfg(flags))
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
    acc.flagPerfmonDomain('Truth')
    if flags.Input.isMC:
        from xAODTruthCnv.xAODTruthCnvConfig import GEN_AOD2xAODCfg
        acc.merge(GEN_AOD2xAODCfg(flags))
        log.info("---------- Configured AODtoxAOD Truth Conversion")

    # trigger
    acc.flagPerfmonDomain('Trigger')
    if flags.Reco.EnableTrigger:
        from TriggerJobOpts.TriggerRecoConfig import TriggerRecoCfg
        acc.merge(TriggerRecoCfg(flags))
        log.info("---------- Configured trigger data decoding")

    # calorimeter
    acc.flagPerfmonDomain('Calo')
    if flags.Detector.EnableCalo:
        from CaloRec.CaloRecoConfig import CaloRecoCfg
        acc.merge(CaloRecoCfg(flags))
        # configure xAOD thinning
        if (flags.Output.doWriteAOD and
                flags.Calo.Thin.NegativeEnergyCaloClusters):
            from ThinningUtils.ThinNegativeEnergyCaloClustersConfig import (
                ThinNegativeEnergyCaloClustersCfg)
            acc.merge(ThinNegativeEnergyCaloClustersCfg(flags))
        log.info("---------- Configured calorimeter reconstruction")

    # ID / ITk
    acc.flagPerfmonDomain('ID')
    if flags.Reco.EnableTracking:
        from InDetConfig.TrackRecoConfig import InDetTrackRecoCfg
        acc.merge(InDetTrackRecoCfg(flags))
        log.info("---------- Configured tracking")

    # HGTD
    acc.flagPerfmonDomain('HGTD')
    if flags.Reco.EnableHGTDExtension:
        from HGTD_Config.HGTD_RecoConfig import HGTD_RecoCfg
        acc.merge(HGTD_RecoCfg(flags))
        log.info("---------- Configured HGTD track extension")

    # Muon
    acc.flagPerfmonDomain('Muon')
    if flags.Detector.EnableMuon:
        from MuonConfig.MuonReconstructionConfig import MuonReconstructionCfg
        acc.merge(MuonReconstructionCfg(flags))
        log.info("---------- Configured muon tracking")

    # EGamma
    acc.flagPerfmonDomain('EGamma')
    if flags.Reco.EnableEgamma:
        from egammaConfig.egammaSteeringConfig import EGammaSteeringCfg
        acc.merge(EGammaSteeringCfg(flags))
        log.info("---------- Configured e/gamma")

    # Caching of CaloExtension for downstream
    # Combined Performance algorithms.
    acc.flagPerfmonDomain('CaloExtension')
    if flags.Reco.EnableCaloExtension:
        from TrackToCalo.CaloExtensionBuilderAlgCfg import (
            CaloExtensionBuilderCfg)
        acc.merge(CaloExtensionBuilderCfg(flags))
        log.info("---------- Configured track calorimeter extension builder")

    # Muon Combined
    acc.flagPerfmonDomain('CombinedMuon')
    if flags.Reco.EnableCombinedMuon:
        from MuonCombinedConfig.MuonCombinedReconstructionConfig import (
            MuonCombinedReconstructionCfg)
        acc.merge(MuonCombinedReconstructionCfg(flags))
        log.info("---------- Configured combined muon reconstruction")

    # TrackParticleCellAssociation = add cells crossed by high pt ID tracks
    acc.flagPerfmonDomain('TrackCellAssociation')
    if flags.Reco.EnableTrackCellAssociation:
        from TrackParticleAssociationAlgs.TrackParticleAssociationAlgsConfig import (
            TrackParticleCellAssociationAlgCfg)
        acc.merge(TrackParticleCellAssociationAlgCfg(flags))
        log.info("---------- Configured track particle-cell association")

    # PFlow
    acc.flagPerfmonDomain('PFlow')
    if flags.Reco.EnablePFlow:
        from eflowRec.PFRun3Config import PFCfg
        acc.merge(PFCfg(flags))
        log.info("---------- Configured particle flow")

    # EGamma and CombinedMuon isolation
    acc.flagPerfmonDomain('Isolation')
    if flags.Reco.EnableCombinedMuon or flags.Reco.EnableEgamma:
        from IsolationAlgs.IsolationSteeringConfig import IsolationSteeringCfg
        acc.merge(IsolationSteeringCfg(flags))
        log.info("---------- Configured isolation")

    # jets
    acc.flagPerfmonDomain('Jets')
    if flags.Reco.EnableJet:
        from JetRecConfig.JetRecoSteering import JetRecoSteeringCfg
        acc.merge(JetRecoSteeringCfg(flags))
        log.info("---------- Configured jets")

    # btagging
    acc.flagPerfmonDomain('FTag')
    if flags.Reco.EnableBTagging:
        from BTagging.BTagRun3Config import BTagRecoSplitCfg
        acc.merge(BTagRecoSplitCfg(flags))
        log.info("---------- Configured btagging")

    # Tau
    acc.flagPerfmonDomain('Tau')
    if flags.Reco.EnableTau:
        from tauRec.TauConfig import TauReconstructionCfg
        acc.merge(TauReconstructionCfg(flags))
        log.info("---------- Configured tau reconstruction")
        # PFlow tau links
        if flags.Reco.EnablePFlow:
            from eflowRec.PFRun3Config import PFTauFELinkCfg
            acc.merge(PFTauFELinkCfg(flags))
            log.info("---------- Configured particle flow tau FE linking")

    # MET
    acc.flagPerfmonDomain('MET')
    if flags.Reco.EnableMet:
        from METReconstruction.METRecCfg import METCfg
        acc.merge(METCfg(flags))
        log.info("---------- Configured MET")

    # Calo Rings
    acc.flagPerfmonDomain('CaloRings')
    if flags.Reco.EnableCaloRinger:
        from CaloRingerAlgs.CaloRingerAlgsConfig import CaloRingerSteeringCfg
        acc.merge(CaloRingerSteeringCfg(flags))
        log.info("---------- Configured Calo Rings")

    # HI
    acc.flagPerfmonDomain('HI')
    if flags.Reco.EnableHI:
        from HIRecConfig.HIRecConfig import HIRecCfg
        acc.merge(HIRecCfg(flags))
        log.info("---------- Configured Heavy Ion reconstruction")

    # AFP
    acc.flagPerfmonDomain('AFP')
    if flags.Reco.EnableAFP:
        from ForwardRec.AFPRecConfig import AFPRecCfg
        acc.merge(AFPRecCfg(flags))
        log.info("---------- Configured AFP reconstruction")

    # Monitoring
    acc.flagPerfmonDomain('DQM')
    if flags.DQ.doMonitoring:
        from AthenaMonitoring.AthenaMonitoringCfg import (
            AthenaMonitoringCfg, AthenaMonitoringPostprocessingCfg)
        acc.merge(AthenaMonitoringCfg(flags))
        if flags.DQ.doPostProcessing:
            acc.merge(AthenaMonitoringPostprocessingCfg(flags))
        log.info("---------- Configured DQ monitoring")

    # Setup the final post-processing
    acc.flagPerfmonDomain('PostProcessing')
    if flags.Reco.EnablePostProcessing:
        acc.merge(RecoPostProcessingCfg(flags))
        log.info("---------- Configured post-processing")

    # setup output
    acc.flagPerfmonDomain('IO')
    if any((flags.Output.doWriteESD,
            flags.Output.doWriteAOD,
            flags.Output.doWriteRDO)):
        from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
        acc.merge(PoolWriteCfg(flags))
        log.info("setup POOL format writing")

    if flags.Output.doWriteESD:
        # Needed for Trk::Tracks TPCnv/
        # Assumes we write Trk::Track on ESD
        from TrkEventCnvTools.TrkEventCnvToolsConfigCA import (
            TrkEventCnvSuperToolCfg)
        acc.merge(TrkEventCnvSuperToolCfg(flags))
        log.info("ESD ItemList: %s", acc.getEventAlgo(
            "OutputStreamESD").ItemList)
        log.info("---------- Configured ESD writing")

    if flags.Output.doWriteAOD:
        log.info("AOD ItemList: %s", acc.getEventAlgo(
            "OutputStreamAOD").ItemList)
        log.info("---------- Configured AOD writing")

    # Set up PerfMon
    acc.flagPerfmonDomain('PerfMon')
    if flags.PerfMon.doFastMonMT or flags.PerfMon.doFullMonMT:
        from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
        acc.merge(PerfMonMTSvcCfg(flags))
        log.info("---------- Configured PerfMon")
        acc.printPerfmonDomains()

    return acc


def RecoPostProcessingCfg(flags):
    acc = ComponentAccumulator()
    if flags.Reco.PostProcessing.TRTAloneThinning:
        from ThinningUtils.ThinTRTStandaloneConfig import (
            ThinTRTStandaloneCfg)
        acc.merge(ThinTRTStandaloneCfg(flags))
    if flags.Reco.PostProcessing.InDetForwardTrackParticleThinning:
        from ThinningUtils.ThinInDetForwardTrackParticlesConfig import (
            ThinInDetForwardTrackParticlesCfg)
        acc.merge(ThinInDetForwardTrackParticlesCfg(flags))
    if flags.Reco.PostProcessing.GeantTruthThinning:
        from ThinningUtils.ThinGeantTruthConfig import (
            ThinGeantTruthCfg)
        acc.merge(ThinGeantTruthCfg(flags))
        pass

    return acc
