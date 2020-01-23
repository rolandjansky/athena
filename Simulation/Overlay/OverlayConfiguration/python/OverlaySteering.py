#!/usr/bin/env python
"""Main steering for MC+MC and MC+data overlay

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
from AthenaPoolCnvSvc.PoolWriteConfig import PoolWriteCfg

from OverlayCopyAlgs.OverlayCopyAlgsConfig import \
    CopyCaloCalibrationHitContainersCfg, CopyJetTruthInfoCfg, CopyMcEventCollectionCfg, \
    CopyTimingsCfg, CopyTrackRecordCollectionsCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoOverlayCfg


def OverlayMainServicesCfg(flags):
    """Configure event loop for overlay"""
    acc = MainServicesThreadedCfg(flags)
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

    return acc
