#!/usr/bin/env python
"""Main steering for MC+MC and MC+data overlay

Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.MainServicesConfig import MainServicesCfg
from AthenaConfiguration.Enums import LHCPeriod
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg
from OverlayConfiguration.OverlayMetadata import overlayMetadataCheck, overlayMetadataWrite

from InDetOverlay.BCMOverlayConfig import BCMOverlayCfg
from InDetOverlay.ITkPixelOverlayConfig import ITkPixelOverlayCfg
from InDetOverlay.ITkStripOverlayConfig import ITkStripOverlayCfg
from InDetOverlay.PixelOverlayConfig import PixelOverlayCfg
from InDetOverlay.SCTOverlayConfig import SCTOverlayCfg
from InDetOverlay.TRTOverlayConfig import TRTOverlayCfg
from HGTD_Overlay.HGTD_OverlayConfig import HGTD_OverlayCfg
from LArDigitization.LArDigitizationConfig import LArOverlayCfg, LArSuperCellOverlayCfg
from MuonConfig.CSC_OverlayConfig import CSC_OverlayCfg
from MuonConfig.MDT_OverlayConfig import MDT_OverlayCfg
from MuonConfig.MM_OverlayConfig import MM_OverlayCfg
from MuonConfig.RPC_OverlayConfig import RPC_OverlayCfg
from MuonConfig.sTGC_OverlayConfig import sTGC_OverlayCfg
from MuonConfig.TGC_OverlayConfig import TGC_OverlayCfg
from OverlayCopyAlgs.OverlayCopyAlgsConfig import \
    CopyCaloCalibrationHitContainersCfg, CopyJetTruthInfoCfg, CopyPileupParticleTruthInfoCfg, CopyMcEventCollectionCfg, \
    CopyTimingsCfg, CopyTrackRecordCollectionsCfg
from TileSimAlgs.TileDigitizationConfig import TileDigitizationCfg, TileOverlayTriggerDigitizationCfg
from TrigT1CaloSim.TTL1OverlayConfig import LArTTL1OverlayCfg, TileTTL1OverlayCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoOverlayCfg


def OverlayMainCfg(configFlags):
    """Main overlay steering configuration"""

    # Construct our accumulator to run
    acc = MainServicesCfg(configFlags)
    acc.merge(PoolReadCfg(configFlags))
    acc.merge(PoolWriteCfg(configFlags))
    acc.merge(OverlayMainContentCfg(configFlags))
    return acc


def OverlayMainContentCfg(configFlags):
    """Main overlay content"""

    # Handle metadata correctly
    overlayMetadataCheck(configFlags)
    acc = overlayMetadataWrite(configFlags)

    # Add event info overlay
    acc.merge(EventInfoOverlayCfg(configFlags))

    # Add truth overlay (needed downstream)
    acc.merge(CopyMcEventCollectionCfg(configFlags))
    acc.merge(CopyJetTruthInfoCfg(configFlags))
    acc.merge(CopyPileupParticleTruthInfoCfg(configFlags))
    acc.merge(CopyTimingsCfg(configFlags))
    acc.merge(CopyCaloCalibrationHitContainersCfg(configFlags))
    acc.merge(CopyTrackRecordCollectionsCfg(configFlags))

    # Beam spot reweighting
    if configFlags.Digitization.InputBeamSigmaZ > 0:
        from BeamEffects.BeamEffectsAlgConfig import BeamSpotReweightingAlgCfg
        acc.merge(BeamSpotReweightingAlgCfg(configFlags))

    # Inner detector
    if configFlags.Detector.EnableBCM:
        acc.merge(BCMOverlayCfg(configFlags))
    if configFlags.Detector.EnablePixel:
        acc.merge(PixelOverlayCfg(configFlags))
    if configFlags.Detector.EnableSCT:
        acc.merge(SCTOverlayCfg(configFlags))
    if configFlags.Detector.EnableTRT:
        acc.merge(TRTOverlayCfg(configFlags))

    # ITk
    if configFlags.Detector.EnableITkPixel:
        acc.merge(ITkPixelOverlayCfg(configFlags))
    if configFlags.Detector.EnableITkStrip:
        acc.merge(ITkStripOverlayCfg(configFlags))

    # HGTD
    if configFlags.Detector.EnableHGTD:
        acc.merge(HGTD_OverlayCfg(configFlags))

    # Calorimeters
    if configFlags.Detector.EnableLAr:
        acc.merge(LArOverlayCfg(configFlags))
        if configFlags.Detector.EnableL1Calo:
            if configFlags.Overlay.DataOverlay:
                pass  # TODO: not supported for now
            else:
                acc.merge(LArTTL1OverlayCfg(configFlags))
                if configFlags.GeoModel.Run in [LHCPeriod.Run3]:
                    acc.merge(LArSuperCellOverlayCfg(configFlags))

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
        acc.merge(CSC_OverlayCfg(configFlags))
    if configFlags.Detector.EnableMDT:
        acc.merge(MDT_OverlayCfg(configFlags))
    if configFlags.Detector.EnableRPC:
        acc.merge(RPC_OverlayCfg(configFlags))
    if configFlags.Detector.EnableTGC:
        acc.merge(TGC_OverlayCfg(configFlags))
    if configFlags.Detector.EnablesTGC:
        acc.merge(sTGC_OverlayCfg(configFlags))
    if configFlags.Detector.EnableMM:
        acc.merge(MM_OverlayCfg(configFlags))

    # Add MT-safe PerfMon
    if configFlags.PerfMon.doFastMonMT or configFlags.PerfMon.doFullMonMT:
        from PerfMonComps.PerfMonCompsConfig import PerfMonMTSvcCfg
        acc.merge(PerfMonMTSvcCfg(configFlags))

    #track overlay
    if configFlags.Overlay.doTrackOverlay:
        #need this to ensure that the ElementLinks to the PRDs are handled correctly (since the name is hardcoded in the converters)
        from TrkEventCnvTools.TrkEventCnvToolsConfigCA import TrkEventCnvSuperToolCfg
        acc.merge(TrkEventCnvSuperToolCfg(configFlags))
        from OverlayCopyAlgs.OverlayCopyAlgsConfig import CopyTrackCollectionsCfg,CopyPixelClusterContainerCfg, CopySCT_ClusterContainerCfg,\
            CopyTRT_DriftCircleContainerCfg
        acc.merge(CopyTrackCollectionsCfg(configFlags))
        acc.merge(CopyPixelClusterContainerCfg(configFlags))
        acc.merge(CopySCT_ClusterContainerCfg(configFlags))
        acc.merge(CopyTRT_DriftCircleContainerCfg(configFlags))

    return acc
