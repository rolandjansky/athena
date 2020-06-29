# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file AtlasUnixStandardJob.py
## @brief py-module to configure the Athena AppMgr for standard (UNIX) jobs
## @author Sebastien Binet <binet@cern.ch>
###############################################################

from __future__ import print_function

def _setupAtlasUnixStandardJob():
    from .AppMgr import theApp
    from .AppMgr import ServiceMgr as svcMgr

    ## basic Gaudi services
    import GaudiSvc.GaudiSvcConf as GaudiSvcConf
    svcMgr += GaudiSvcConf.IncidentSvc()
    svcMgr += GaudiSvcConf.EvtPersistencySvc( "EventPersistencySvc" )
    svcMgr += GaudiSvcConf.HistogramSvc( "HistogramDataSvc" )
    svcMgr += GaudiSvcConf.NTupleSvc()
    # ToolSvc is already added in AppMgr.py.
    svcMgr += GaudiSvcConf.RndmGenSvc()
    svcMgr += GaudiSvcConf.ChronoStatSvc()

    import GaudiAud.GaudiAudConf as GaudiAudConf
    svcMgr.AuditorSvc += GaudiAudConf.AlgContextAuditor()

    # make the message service available
    # from GaudiSvc.GaudiSvcConf import MessageSvc
    svcMgr.MessageSvc = theApp.service( "MessageSvc" )     # already instantiated

    # StoreGate services configuration
    import StoreGate.StoreGateConf as StoreGateConf
    svcMgr += StoreGateConf.StoreGateSvc()
    svcMgr += StoreGateConf.StoreGateSvc("DetectorStore")
    svcMgr += StoreGateConf.StoreGateSvc("HistoryStore")
    svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

    #ClassIDSvc configuration
    import CLIDComps.CLIDCompsConf as CLIDCompsConf
    svcMgr += CLIDCompsConf.ClassIDSvc()
    svcMgr.ClassIDSvc.CLIDDBFiles += [ "Gaudi_clid.db" ]

    #Ignore certain Service loops
    theApp.InitializationLoopCheck = False

    # dictionary services
    # the dict loader
    import AthenaServices.AthenaServicesConf as AthenaServicesConf
    if not hasattr(svcMgr, 'AthDictLoaderSvc'):
        svcMgr += AthenaServicesConf.AthDictLoaderSvc()
    theApp.CreateSvc += [svcMgr.AthDictLoaderSvc.getFullJobOptName()]

    # add-in a service to handle core-dumps
    if not hasattr(svcMgr, 'CoreDumpSvc'):
        svcMgr += AthenaServicesConf.CoreDumpSvc()
    theApp.CreateSvc += [ svcMgr.CoreDumpSvc.getFullJobOptName() ]

## load basic services configuration at module import
_setupAtlasUnixStandardJob()

## clean-up: avoid running multiple times this method
del _setupAtlasUnixStandardJob
