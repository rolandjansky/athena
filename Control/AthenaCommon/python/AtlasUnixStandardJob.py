# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

## @file AtlasUnixStandardJob.py
## @brief py-module to configure the Athena AppMgr for standard (UNIX) jobs
## @author Sebastien Binet <binet@cern.ch>
## $Id: AtlasUnixStandardJob.py,v 1.16 2009-02-10 22:48:51 wlav Exp $
###############################################################

def _setupAtlasUnixStandardJob():
    from AppMgr import theApp
    from AppMgr import ServiceMgr as svcMgr

    from Constants import INFO

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

    # make sure StatusCodeSvc messages are always printed
    svcMgr += GaudiSvcConf.StatusCodeSvc()
    svcMgr.StatusCodeSvc.OutputLevel = INFO
    svcMgr.StatusCodeSvc.Filter += [
              "Reflex::NewDelFunctionsT<StatusCode>::delete_T(void*)",
        "ROOT::Reflex::NewDelFunctionsT<StatusCode>::delete_T(void*)",
        ]
    # make sure StatusCodes are always checked
    svcMgr.StatusCodeSvc.AbortOnError=True


    
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
    try:
        theApp.InitializationLoopCheck = False
##         theApp.LoopCheckIgnore += [
##             "StoreGateSvc", "DetectorStore",
##             "PerfMonSvc", "AuditorSvc", "GeoModelSvc",
##             "IOVSvc"
##             ]
    except AttributeError:
        # gaudi v19r9... were art thou ?
        pass

    # dictionary services
    # the dict loader
    import AthenaServices.AthenaServicesConf as AthenaServicesConf
    if not hasattr(svcMgr, 'AthDictLoaderSvc'):
        svcMgr += AthenaServicesConf.AthDictLoaderSvc()
    theApp.CreateSvc += [svcMgr.AthDictLoaderSvc.getFullJobOptName()]

    # the dict checker
    if not hasattr(svcMgr, 'AthenaSealSvc'):
        svcMgr += AthenaServicesConf.AthenaSealSvc()
    theApp.CreateSvc += [svcMgr.AthenaSealSvc.getFullJobOptName()]
    
    # add-in a service to handle core-dumps
    if not hasattr(svcMgr, 'CoreDumpSvc'):
        svcMgr += AthenaServicesConf.CoreDumpSvc()
    theApp.CreateSvc += [ svcMgr.CoreDumpSvc.getFullJobOptName() ]

## load basic services configuration at module import
_setupAtlasUnixStandardJob()

## clean-up: avoid running multiple times this method
del _setupAtlasUnixStandardJob
