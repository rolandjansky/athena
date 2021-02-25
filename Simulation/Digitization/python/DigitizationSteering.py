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
    if flags.Detector.EnableBCM:
        acc.merge(BCM_DigitizationCfg(flags))
    if flags.Detector.EnablePixel:
        acc.merge(PixelDigitizationCfg(flags))
    if flags.Detector.EnableSCT:
        acc.merge(SCT_DigitizationCfg(flags))
    if flags.Detector.EnableTRT:
        acc.merge(TRT_DigitizationCfg(flags))

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


def setupDigitizationFlags(flags):
    """Setup common digitization flags."""
    # autoconfigure pile-up
    if flags.Digitization.PU.NumberOfLowPtMinBias > 0 \
        or flags.Digitization.PU.NumberOfHighPtMinBias > 0 \
        or flags.Digitization.PU.NumberOfBeamHalo > 0 \
        or flags.Digitization.PU.NumberOfBeamGas > 0 \
        or flags.Digitization.PU.NumberOfCavern > 0:
        flags.Digitization.PileUp = True

    if flags.Digitization.PileUp:
        flags.Input.OverrideRunNumber = True
        # keep this one True by default in CA-based config
        flags.Digitization.DoXingByXingPileUp = True
