#!/usr/bin/env python
"""Main steering for the digitization jobs

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from BCM_Digitization.BCM_DigitizationConfigNew import BCM_DigitizationCfg
from Digitization.DigitizationParametersConfig import writeDigitizationMetadata
from LArDigitization.LArDigitizationConfigNew import LArTriggerDigitizationCfg
from MCTruthSimAlgs.RecoTimingConfig import MergeRecoTimingObjCfg
from MuonConfig.CSC_DigitizationConfig import CSC_DigitizationDigitToRDOCfg
from MuonConfig.MDT_DigitizationConfig import MDT_DigitizationDigitToRDOCfg
from MuonConfig.RPC_DigitizationConfig import RPC_DigitizationDigitToRDOCfg
from MuonConfig.TGC_DigitizationConfig import TGC_DigitizationDigitToRDOCfg
from PixelDigitization.PixelDigitizationConfigNew import PixelDigitizationCfg
from SCT_Digitization.SCT_DigitizationConfigNew import SCT_DigitizationCfg
from TileSimAlgs.TileDigitizationConfig import TileDigitizationCfg, TileTriggerDigitizationCfg
from TRT_Digitization.TRT_DigitizationConfigNew import TRT_DigitizationCfg


def DigitizationMainServicesCfg(flags):
    """Configure main digitization services"""
    if flags.Digitization.PileUp:
        from Digitization.PileUpConfigNew import PileUpEventLoopMgrCfg
        acc = MainServicesCfg(flags, LoopMgr="PileUpEventLoopMgr")
        acc.merge(PileUpEventLoopMgrCfg(flags))
    else:
        acc = MainServicesCfg(flags)

    acc.merge(PoolReadCfg(flags))
    acc.merge(PoolWriteCfg(flags))

    acc.merge(writeDigitizationMetadata(flags))

    return acc


def DigitizationMainCfg(flags):
    # Construct main services
    acc = DigitizationMainServicesCfg(flags)

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
            MergeTruthJetsCfg,
            MergeMuonEntryLayerCfg,
            MergeCalibHitsCfg,
        )

        acc.merge(SignalOnlyMcEventCollCfg(flags))
        acc.merge(MergeTruthJetsCfg(flags))
        acc.merge(MergeMuonEntryLayerCfg(flags))
        acc.merge(MergeCalibHitsCfg(flags))

        from Digitization.TruthDigitizationOutputConfig import TruthDigitizationOutputCfg
        acc.merge(TruthDigitizationOutputCfg(flags))

    # Inner Detector
    if flags.Detector.DigitizeBCM:
        acc.merge(BCM_DigitizationCfg(flags))
    if flags.Detector.DigitizePixel:
        acc.merge(PixelDigitizationCfg(flags))
    if flags.Detector.DigitizeSCT:
        acc.merge(SCT_DigitizationCfg(flags))
    if flags.Detector.DigitizeTRT:
        acc.merge(TRT_DigitizationCfg(flags))

    # Calorimeter
    if flags.Detector.DigitizeLAr:
        acc.merge(LArTriggerDigitizationCfg(flags))
    if flags.Detector.DigitizeTile:
        acc.merge(TileDigitizationCfg(flags))
        acc.merge(TileTriggerDigitizationCfg(flags))

    # Muon Spectrometer
    if flags.Detector.DigitizeMDT:
        acc.merge(MDT_DigitizationDigitToRDOCfg(flags))
    if flags.Detector.DigitizeTGC:
        acc.merge(TGC_DigitizationDigitToRDOCfg(flags))
    if flags.Detector.DigitizeRPC:
        acc.merge(RPC_DigitizationDigitToRDOCfg(flags))
    if flags.Detector.DigitizeCSC:
        acc.merge(CSC_DigitizationDigitToRDOCfg(flags))

    # Timing
    acc.merge(MergeRecoTimingObjCfg(flags))

    return acc


def DigitizationMessageSvcCfg(flags):
    """MessageSvc for digitization and overlay"""
    MessageSvc = CompFactory.MessageSvc
    acc = ComponentAccumulator()
    acc.addService(MessageSvc(setError=["HepMcParticleLink"]))
    return acc


def setupDigitizationDetectorFlags(configFlags, detectors):
    """Setup digitization detector flags"""
    if not detectors or 'BCM' in detectors or 'ID' in detectors:
        configFlags.Detector.DigitizeBCM = True
    if not detectors or 'DBM' in detectors or 'ID' in detectors:
        configFlags.Detector.DigitizeDBM = True
    if not detectors or 'Pixel' in detectors or 'ID' in detectors:
        configFlags.Detector.DigitizePixel = True
    if not detectors or 'SCT' in detectors or 'ID' in detectors:
        configFlags.Detector.DigitizeSCT = True
    if not detectors or 'TRT' in detectors or 'ID' in detectors:
        configFlags.Detector.DigitizeTRT = True
    if not detectors or 'LAr' in detectors or 'Calo' in detectors or 'L1Calo' in detectors:
        configFlags.Detector.DigitizeLAr = True
    if not detectors or 'Tile' in detectors or 'Calo' in detectors or 'L1Calo' in detectors:
        configFlags.Detector.DigitizeTile = True
    if not detectors or 'L1Calo' in detectors:
        configFlags.Detector.DigitizeL1Calo = True
    if not detectors or 'CSC' in detectors or 'Muon' in detectors:
        configFlags.Detector.DigitizeCSC = True
    if not detectors or 'MDT' in detectors or 'Muon' in detectors:
        configFlags.Detector.DigitizeMDT = True
    if not detectors or 'RPC' in detectors or 'Muon' in detectors:
        configFlags.Detector.DigitizeRPC = True
    if not detectors or 'TGC' in detectors or 'Muon' in detectors:
        configFlags.Detector.DigitizeTGC = True
    if not detectors or 'sTGC' in detectors or 'Muon' in detectors:
        configFlags.Detector.DigitizesTGC = True
    if not detectors or 'MM' in detectors or 'Muon' in detectors:
        configFlags.Detector.DigitizeMM = True
