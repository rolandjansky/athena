# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file AtlasUnixGeneratorJob.py
## @brief py-module to configure the Athena AppMgr for generator (UNIX) jobs
## @author Sebastien Binet <binet@cern.ch>
###############################################################

from __future__ import print_function

def _setupAtlasUnixGeneratorJob():
    from AthenaCommon import AtlasUnixStandardJob    # noqa: F401
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.Logging import logging
    log = logging.getLogger('AtlasUnixGeneratorJob')

    # General Application Configuration options
    from McEventSelector.McEventSelectorConf import McCnvSvc
    svcMgr += McCnvSvc()
    if hasattr(svcMgr, 'EventSelector'):
        log.warning('EventSelector of type %s already exists. Will not add McEventSelector.', svcMgr.EventSelector.getType())
    else:
        from McEventSelector.McEventSelectorConf import McEventSelector
        svcMgr += McEventSelector("EventSelector")
        theApp.EvtSel = svcMgr.EventSelector.getFullName()

    # Persistency services
    svcMgr.EventPersistencySvc.CnvServices += [ "McCnvSvc" ]

    # Temporarily inject the xAOD::EventInfo converter here to allow for adiabatic migration of the clients
    from AthenaCommon.AlgSequence import AthSequencer
    topSequence = AthSequencer("AthAlgSeq")
    if not hasattr(topSequence, "EventInfoCnvAlg"):
        from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoCnvAlg
        topSequence += xAODMaker__EventInfoCnvAlg(AODKey = 'McEventInfo')

    return

## load basic services configuration at module import
_setupAtlasUnixGeneratorJob()

## clean-up: avoid running multiple times this method
del _setupAtlasUnixGeneratorJob
