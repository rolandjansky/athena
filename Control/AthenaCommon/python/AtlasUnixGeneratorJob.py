# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file AtlasUnixGeneratorJob.py
## @brief py-module to configure the Athena AppMgr for generator (UNIX) jobs
## @author Sebastien Binet <binet@cern.ch>
## $Id: AtlasUnixGeneratorJob.py,v 1.5 2009-02-10 22:49:02 wlav Exp $
###############################################################

def _setupAtlasUnixGeneratorJob():
    import AtlasUnixStandardJob
    from AppMgr import theApp
    from AppMgr import ServiceMgr as svcMgr

    # General Application Configuration options
    from McEventSelector.McEventSelectorConf import McCnvSvc
    from McEventSelector.McEventSelectorConf import McEventSelector
    svcMgr += McCnvSvc()
    if hasattr(svcMgr, 'EventSelector'): del svcMgr.EventSelector
    svcMgr += McEventSelector("EventSelector")
    theApp.EvtSel = svcMgr.EventSelector.getFullName()

    # Persistency services
    svcMgr.EventPersistencySvc.CnvServices += [ "McCnvSvc" ]
    return

## load basic services configuration at module import
_setupAtlasUnixGeneratorJob()

## clean-up: avoid running multiple times this method
del _setupAtlasUnixGeneratorJob
