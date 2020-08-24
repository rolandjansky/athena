#!/usr/bin/env python
"""Main steering for MC+MC and MC+data overlay

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg

from InDetOverlay.BCMOverlayConfig import BCMOverlayCfg
from InDetOverlay.PixelOverlayConfig import PixelOverlayCfg
from InDetOverlay.SCTOverlayConfig import SCTOverlayCfg
from InDetOverlay.TRTOverlayConfig import TRTOverlayCfg
from LArDigitization.LArDigitizationConfigNew import LArOverlayCfg
from MuonConfig.CscOverlayConfig import CscOverlayCfg
from MuonConfig.MdtOverlayConfig import MdtOverlayCfg
from MuonConfig.RpcOverlayConfig import RpcOverlayCfg
from MuonConfig.TgcOverlayConfig import TgcOverlayCfg
from OverlayCopyAlgs.OverlayCopyAlgsConfig import \
    CopyCaloCalibrationHitContainersCfg, CopyJetTruthInfoCfg, CopyMcEventCollectionCfg, \
    CopyTimingsCfg, CopyTrackRecordCollectionsCfg
from TileSimAlgs.TileDigitizationConfig import TileDigitizationCfg, TileOverlayTriggerDigitizationCfg
from TrigT1CaloSim.OverlayTTL1Config import OverlayTTL1Cfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoOverlayCfg


def OverlayMainServicesCfg(flags):
    """Configure event loop for overlay"""
    acc = MainServicesCfg(flags)
    if not flags.Overlay.DataOverlay:
        if flags.Concurrency.NumThreads > 0:
            AthenaHiveEventLoopMgr = CompFactory.AthenaHiveEventLoopMgr
            elmgr = AthenaHiveEventLoopMgr()
        else:
            AthenaEventLoopMgr = CompFactory.AthenaEventLoopMgr
            elmgr = AthenaEventLoopMgr()
        elmgr.UseSecondaryEventNumber = True
        acc.addService(elmgr)
    return acc


def OverlayMainCfg(configFlags):
    """Main overlay steering configuration"""

    # Construct our accumulator to run
    acc = OverlayMainServicesCfg(configFlags)
    acc.merge(PoolReadCfg(configFlags))
    acc.merge(PoolWriteCfg(configFlags))

    # Add event info overlay
    acc.merge(EventInfoOverlayCfg(configFlags))

    # Add truth overlay (needed downstream)
    acc.merge(CopyMcEventCollectionCfg(configFlags))
    acc.merge(CopyJetTruthInfoCfg(configFlags))
    acc.merge(CopyTimingsCfg(configFlags))
    acc.merge(CopyCaloCalibrationHitContainersCfg(configFlags))
    acc.merge(CopyTrackRecordCollectionsCfg(configFlags))

    # Inner detector
    if configFlags.Detector.OverlayBCM:
        acc.merge(BCMOverlayCfg(configFlags))
    if configFlags.Detector.OverlayPixel:
        acc.merge(PixelOverlayCfg(configFlags))
    if configFlags.Detector.OverlaySCT:
        acc.merge(SCTOverlayCfg(configFlags))
    if configFlags.Detector.OverlayTRT:
        acc.merge(TRTOverlayCfg(configFlags))

    # Calorimeters
    if configFlags.Detector.OverlayLAr:
        acc.merge(LArOverlayCfg(configFlags))
    if configFlags.Detector.OverlayTile:
        acc.merge(TileDigitizationCfg(configFlags))
    if configFlags.Detector.OverlayL1Calo:
        acc.merge(OverlayTTL1Cfg(configFlags))
        acc.merge(TileOverlayTriggerDigitizationCfg(configFlags))

    # Muon system
    if configFlags.Detector.OverlayCSC:
        acc.merge(CscOverlayCfg(configFlags))
    if configFlags.Detector.OverlayMDT:
        acc.merge(MdtOverlayCfg(configFlags))
    if configFlags.Detector.OverlayRPC:
        acc.merge(RpcOverlayCfg(configFlags))
    if configFlags.Detector.OverlayTGC:
        acc.merge(TgcOverlayCfg(configFlags))

    return acc
