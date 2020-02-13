#!/usr/bin/env python
"""Main steering for MC+MC and MC+data overlay

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.MainServicesConfig import MainServicesThreadedCfg
from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg

from OverlayCopyAlgs.OverlayCopyAlgsConfig import \
    CopyCaloCalibrationHitContainersCfg, CopyJetTruthInfoCfg, CopyMcEventCollectionCfg, \
    CopyTimingsCfg, CopyTrackRecordCollectionsCfg
from xAODEventInfoCnv.xAODEventInfoCnvConfig import EventInfoOverlayCfg

def OverlayMainCfg(configFlags):
    """Main overlay steering configuration"""

    # Construct our accumulator to run
    acc = MainServicesThreadedCfg(configFlags)
    acc.merge(PoolReadCfg(configFlags))

    # Add event info overlay
    acc.merge(EventInfoOverlayCfg(configFlags))

    # Add truth overlay (needed downstream)
    acc.merge(CopyMcEventCollectionCfg(configFlags))
    acc.merge(CopyJetTruthInfoCfg(configFlags))
    acc.merge(CopyTimingsCfg(configFlags))
    acc.merge(CopyCaloCalibrationHitContainersCfg(configFlags))
    acc.merge(CopyTrackRecordCollectionsCfg(configFlags))

    return acc
