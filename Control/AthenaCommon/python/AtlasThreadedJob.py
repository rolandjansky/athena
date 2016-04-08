# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file AtlasThreadedJob.py
## @brief py-module to configure the Athena AppMgr for threaded (Hive) jobs
## @author Charles Leggett
###############################################################

def _setupAtlasThreadedJob():
    from AppMgr import theApp
    from AppMgr import ServiceMgr as svcMgr

    import SystemOfUnits as Units
    from Constants import VERBOSE, DEBUG, INFO, ERROR

    from ConcurrencyFlags import jobproperties as jps

    if (jps.ConcurrencyFlags.NumProcs() == 0) :
        theApp.MessageSvcType = "InertMessageSvc"
    else:
        # InertMessageSvc doesn't play nice with MP
        theApp.MessageSvcType = "MessageSvc"

    svcMgr.MessageSvc.defaultLimit = 0
    msgFmt = "% F%40W%S%4W%e%s%7W%R%T %0W%M"
    svcMgr.MessageSvc.Format = msgFmt

    theApp.StatusCodeCheck = False

    from AthenaServices.AthenaServicesConf import AthenaHiveEventLoopMgr

    svcMgr += AthenaHiveEventLoopMgr()
    svcMgr.AthenaHiveEventLoopMgr.WhiteboardSvc = "EventDataSvc"
#    svcMgr.AthenaHiveEventLoopMgr.OutputLevel = INFO

    theApp.EventLoop = "AthenaHiveEventLoopMgr"

    svcMgr.StatusCodeSvc.AbortOnError = False

    nThreads = jps.ConcurrencyFlags.NumThreads()
    numStores = nThreads
    numAlgsInFlight = nThreads
    numThreads = nThreads

    from StoreGate.StoreGateConf import SG__HiveMgrSvc
    svcMgr += SG__HiveMgrSvc("EventDataSvc")
    svcMgr.EventDataSvc.NSlots = numStores
#    svcMgr.EventDataSvc.OutputLevel = INFO

    import StoreGate.StoreGateConf as StoreGateConf
    svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")


    from GaudiHive.GaudiHiveConf import AlgResourcePool
    svcMgr += AlgResourcePool( OutputLevel = INFO );

    from GaudiHive.GaudiHiveConf import ForwardSchedulerSvc
    svcMgr += ForwardSchedulerSvc()
    svcMgr.ForwardSchedulerSvc.OutputLevel = INFO
    svcMgr.ForwardSchedulerSvc.MaxEventsInFlight = numStores
    svcMgr.ForwardSchedulerSvc.MaxAlgosInFlight = numAlgsInFlight
    svcMgr.ForwardSchedulerSvc.ThreadPoolSize = numThreads

    # enable timeline recording
    from GaudiHive.GaudiHiveConf import TimelineSvc
    svcMgr += TimelineSvc( RecordTimeline = True, Partial = False );
    
    #
    ## Setup SGCommitAuditor to sweep new DataObjects at end of Alg execute
    #
    
    theAuditorSvc = svcMgr.AuditorSvc
    theApp.AuditAlgorithms=True 
    from SGComps.SGCompsConf import SGCommitAuditor
    theAuditorSvc += SGCommitAuditor()
    
    
## load basic services configuration at module import
_setupAtlasThreadedJob()

## clean-up: avoid running multiple times this method
del _setupAtlasThreadedJob
