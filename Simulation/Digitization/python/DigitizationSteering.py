#!/usr/bin/env python
"""Main steering for the digitization jobs

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from BCM_Digitization.BCM_DigitizationConfigNew import BCM_DigitizationCfg
from Digitization.DigitizationParametersConfig import writeDigitizationMetadata
from LArDigitization.LArDigitizationConfigNew import LArTriggerDigitizationCfg
from MCTruthSimAlgs.RecoTimingConfig import MergeRecoTimingObjCfg
from MuonConfig.CSC_DigitizationConfig import CSC_DigitizationDigitToRDOCfg
from MuonConfig.MDT_DigitizationConfig import MDT_DigitizationDigitToRDOCfg
from MuonConfig.MM_DigitizationConfig import MM_DigitizationDigitToRDOCfg
from MuonConfig.RPC_DigitizationConfig import RPC_DigitizationDigitToRDOCfg
from MuonConfig.TGC_DigitizationConfig import TGC_DigitizationDigitToRDOCfg
from MuonConfig.sTGC_DigitizationConfig import sTGC_DigitizationDigitToRDOCfg
from PixelDigitization.ITkPixelDigitizationConfig import ITkPixelDigitizationCfg
from PixelDigitization.PixelDigitizationConfigNew import PixelDigitizationCfg
from SCT_Digitization.SCT_DigitizationConfigNew import SCT_DigitizationCfg
from StripDigitization.StripDigitizationConfig import ITkStripDigitizationCfg
from TileSimAlgs.TileDigitizationConfig import TileDigitizationCfg, TileTriggerDigitizationCfg
from TRT_Digitization.TRT_DigitizationConfigNew import TRT_DigitizationCfg
from Digitization.PileUpUtils import pileupInputCollections

from AthenaCommon.Logging import logging
logDigiSteering = logging.getLogger('DigitizationSteering')

def DigitizationMainServicesCfg(flags):
    """Configure main digitization services"""
    if flags.Digitization.PileUp:
        if flags.Concurrency.NumThreads > 0:
            logDigiSteering.error("DigitizationMainServicesCfg: Attempting to run pile-up digitization AthenaMT using %s threads!", str(flags.Concurrency.NumThreads))
            logDigiSteering.error("DigitizationMainServicesCfg: Running pile-up digitization with AthenaMT is not supported. Please update your configuration. The job will fail now.")
            raise RuntimeError("DigitizationSteering.DigitizationMainServicesCfg: Running pile-up digitization with AthenaMT is not supported. Please update your configuration.")
        from Digitization.PileUpConfigNew import PileUpEventLoopMgrCfg
        acc = MainServicesCfg(flags, LoopMgr="PileUpEventLoopMgr")
        acc.merge(PileUpEventLoopMgrCfg(flags))
    else:
        acc = MainServicesCfg(flags)

    acc.merge(PoolReadCfg(flags))
    acc.merge(PoolWriteCfg(flags))

    return acc


def DigitizationMainCfg(flags):
    # Construct main services
    acc = DigitizationMainServicesCfg(flags)

    acc.merge(DigitizationMainContentCfg(flags))

    return acc

def DigitizationMainContentCfg(flags):

    acc = ComponentAccumulator()

    acc.merge(writeDigitizationMetadata(flags))

    if not flags.Digitization.PileUp:
        # Old EventInfo conversion
        if "EventInfo" not in flags.Input.Collections:
            from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoCnvAlgCfg
            acc.merge(EventInfoCnvAlgCfg(flags,
                                        inputKey="McEventInfo",
                                        outputKey="EventInfo"))
        # Decorate pile-up values
        from Digitization.PileUpConfigNew import NoPileUpMuWriterCfg
        acc.merge(NoPileUpMuWriterCfg(flags))

    # Signal-only truth information
    # TODO: is another flag needed for this?
    if flags.Digitization.PileUp:
        from MCTruthSimAlgs.MCTruthSimAlgsConfigNew import (
            SignalOnlyMcEventCollCfg,
            MergeAntiKt4TruthJetsCfg,
            MergeAntiKt6TruthJetsCfg,
            MergeTruthParticlesCfg,
            MergeMuonEntryLayerCfg,
            MergeCalibHitsCfg,
        )
        if flags.Common.ProductionStep!=ProductionStep.FastChain:
            acc.merge(SignalOnlyMcEventCollCfg(flags))
        puCollections = pileupInputCollections(flags.Digitization.PU.LowPtMinBiasInputCols)
        if "AntiKt4TruthJets" in puCollections:
            acc.merge(MergeAntiKt4TruthJetsCfg(flags))
        if "AntiKt6TruthJets" in puCollections:
            acc.merge(MergeAntiKt6TruthJetsCfg(flags))
        if "TruthPileupParticles" in puCollections:
            acc.merge(MergeTruthParticlesCfg(flags))
        acc.merge(MergeMuonEntryLayerCfg(flags))
        acc.merge(MergeCalibHitsCfg(flags))

        from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
        acc.merge(TruthDigitizationOutputCfg(flags))

    # Beam spot reweighting
    if flags.Common.ProductionStep != ProductionStep.PileUpPresampling and flags.Digitization.InputBeamSigmaZ > 0:
        from BeamEffects.BeamEffectsAlgConfig import BeamSpotReweightingAlgCfg
        acc.merge(BeamSpotReweightingAlgCfg(flags))

    # Inner Detector
    if flags.Detector.EnableBCM:
        acc.merge(BCM_DigitizationCfg(flags))
    if flags.Detector.EnablePixel:
        acc.merge(PixelDigitizationCfg(flags))
    if flags.Detector.EnableSCT:
        acc.merge(SCT_DigitizationCfg(flags))
    if flags.Detector.EnableTRT:
        acc.merge(TRT_DigitizationCfg(flags))

    # ITk
    if flags.Detector.EnableITkPixel:
        acc.merge(ITkPixelDigitizationCfg(flags))
    if flags.Detector.EnableITkStrip:
        acc.merge(ITkStripDigitizationCfg(flags))

    # Calorimeter
    if flags.Detector.EnableLAr:
        acc.merge(LArTriggerDigitizationCfg(flags))
    if flags.Detector.EnableTile:
        acc.merge(TileDigitizationCfg(flags))
        acc.merge(TileTriggerDigitizationCfg(flags))

    # Muon Spectrometer
    if flags.Detector.EnableMDT:
        acc.merge(MDT_DigitizationDigitToRDOCfg(flags))
    if flags.Detector.EnableTGC:
        acc.merge(TGC_DigitizationDigitToRDOCfg(flags))
    if flags.Detector.EnableRPC:
        acc.merge(RPC_DigitizationDigitToRDOCfg(flags))
    if flags.Detector.EnableCSC:
        acc.merge(CSC_DigitizationDigitToRDOCfg(flags))
    if flags.Detector.EnableMM:
        acc.merge(MM_DigitizationDigitToRDOCfg(flags))
    if flags.Detector.EnablesTGC:
        acc.merge(sTGC_DigitizationDigitToRDOCfg(flags))

    # Add MT-safe PerfMon
    if flags.PerfMon.doFastMonMT or flags.PerfMon.doFullMonMT:
        from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
        acc.merge(PerfMonMTSvcCfg(flags))

    # Timing
    acc.merge(MergeRecoTimingObjCfg(flags))

    return acc


def DigitizationMessageSvcCfg(flags):
    """MessageSvc for digitization and overlay"""
    MessageSvc = CompFactory.MessageSvc
    acc = ComponentAccumulator()
    acc.addService(MessageSvc(setError=["HepMcParticleLink"]))
    return acc


def DigitizationTestingPostInclude(flags, acc):
    """Testing digitization post-include"""
    # dump config
    configName = "DigiPUConfigCA" if flags.Digitization.PileUp else "DigiConfigCA"
    from AthenaConfiguration.JobOptsDumper import JobOptsDumperCfg
    acc.merge(JobOptsDumperCfg(flags, FileName=f"{configName}.txt"))

    # dump pickle
    with open(f"{configName}.pkl", "wb") as f:
        acc.store(f)
