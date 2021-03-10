#!/usr/bin/env python
"""Main steering for MC+MC and MC+data overlay

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
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
from TrigT1CaloSim.TTL1OverlayConfig import LArTTL1OverlayCfg, TileTTL1OverlayCfg
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
        elmgr.RequireInputAttributeList = True
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
    if configFlags.Detector.EnableBCM:
        acc.merge(BCMOverlayCfg(configFlags))
    if configFlags.Detector.EnablePixel:
        acc.merge(PixelOverlayCfg(configFlags))
    if configFlags.Detector.EnableSCT:
        acc.merge(SCTOverlayCfg(configFlags))
    if configFlags.Detector.EnableTRT:
        acc.merge(TRTOverlayCfg(configFlags))

    # Calorimeters
    if configFlags.Detector.EnableLAr:
        acc.merge(LArOverlayCfg(configFlags))
        if configFlags.Detector.EnableL1Calo:
            if configFlags.Overlay.DataOverlay:
                pass  # TODO: not supported for now
            else:
                acc.merge(LArTTL1OverlayCfg(configFlags))
    if configFlags.Detector.EnableTile:
        acc.merge(TileDigitizationCfg(configFlags))
        if configFlags.Detector.EnableL1Calo:
            if configFlags.Overlay.DataOverlay:
                pass  # TODO: not supported for now
            else:
                acc.merge(TileTTL1OverlayCfg(configFlags))
                acc.merge(TileOverlayTriggerDigitizationCfg(configFlags))

    # Muon system
    if configFlags.Detector.EnableCSC:
        acc.merge(CscOverlayCfg(configFlags))
    if configFlags.Detector.EnableMDT:
        acc.merge(MdtOverlayCfg(configFlags))
    if configFlags.Detector.EnableRPC:
        acc.merge(RpcOverlayCfg(configFlags))
    if configFlags.Detector.EnableTGC:
        acc.merge(TgcOverlayCfg(configFlags))

    return acc
