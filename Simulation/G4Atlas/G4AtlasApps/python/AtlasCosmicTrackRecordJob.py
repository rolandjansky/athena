# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file AtlasCosmicTrackRecordJob.py
## @brief py-module to configure the Athena AppMgr for cosmic track record reading jobs
## @author Zach Marshall <zmarshal@caltech.edu>
###############################################################

def _setupAtlasCosmicTrackRecordJob():
    import AthenaCommon.AtlasUnixStandardJob  # noqa: F401
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    # General Application Configuration options
    from McEventSelector.McEventSelectorConf import McCnvSvc
    svcMgr += McCnvSvc()

    if hasattr(svcMgr, 'EventSelector'):
        theApp.EvtSel = svcMgr.EventSelector.getFullName()

    # Persistency services
    svcMgr.EventPersistencySvc.CnvServices += [ "McCnvSvc" ]


## Load at module import, and clean-up to avoid running this method multiple times
_setupAtlasCosmicTrackRecordJob()
del _setupAtlasCosmicTrackRecordJob
