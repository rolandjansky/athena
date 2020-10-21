# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file McAsciiEventSelector/python/ReadMcAscii.py
## @brief: install the McAsciiEventSelector

def _loadReadMcAscii():
    import AthenaCommon.AtlasUnixStandardJob  # noqa: F401
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr

    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'loadReadMcAscii' )
    msg.debug( "Loading services for 'ReadMcAscii'..." )

    ## needed dlls
    if not hasattr(svcMgr, 'PartPropSvc'):
        from PartPropSvc.PartPropSvcConf import PartPropSvc
        svcMgr += PartPropSvc()

    ##

    from AthenaCommon import CfgMgr

    svcMgr += CfgMgr.McAsciiCnvSvc()
    svcMgr += CfgMgr.McAsciiEventSelector("EventSelector")
    theApp.EvtSel = svcMgr.EventSelector.getFullName()

    # Persistency services
    svcMgr.EventPersistencySvc.CnvServices += [ "McAsciiCnvSvc" ]

    return

## load 'ReadMcAscii' configuration at module import
_loadReadMcAscii()

## clean-up: avoid running multiple times the configuration method
del _loadReadMcAscii
