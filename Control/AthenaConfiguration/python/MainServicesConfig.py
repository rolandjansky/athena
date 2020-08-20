# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
AthSequencer=CompFactory.AthSequencer

def MainServicesMiniCfg(loopMgr='AthenaEventLoopMgr', masterSequence='AthAlgSeq'):
    #Mininmal basic config, just good enough for HelloWorld and alike
    cfg=ComponentAccumulator(masterSequence)
    cfg.setAsTopLevel()
    cfg.setAppProperty('TopAlg',['AthSequencer/'+masterSequence])
    cfg.setAppProperty('MessageSvcType', 'MessageSvc')
    cfg.setAppProperty('EventLoop', loopMgr)
    cfg.setAppProperty('ExtSvcCreates', 'False')
    cfg.setAppProperty('JobOptionsSvcType', 'JobOptionsSvc')
    cfg.setAppProperty('JobOptionsType', 'NONE')
    cfg.setAppProperty('JobOptionsPostAction', '')
    cfg.setAppProperty('JobOptionsPreAction', '')
    cfg.setAppProperty('PrintAlgsSequence', True)
    return cfg


def MainServicesCfg(cfgFlags):
    # Run a serial job for threads=0
    LoopMgr = 'AthenaEventLoopMgr'
    if cfgFlags.Concurrency.NumThreads>0:
        if cfgFlags.Concurrency.NumConcurrentEvents==0:
            # In a threaded job this will mess you up because no events will be processed
            raise Exception("Requested Concurrency.NumThreads>0 and Concurrency.NumConcurrentEvents==0, which will not process events!")
        LoopMgr = "AthenaHiveEventLoopMgr"

    ########################################################################
    # Core components needed for serial and threaded jobs
    cfg=MainServicesMiniCfg(loopMgr=LoopMgr, masterSequence='AthMasterSeq')
    cfg.setAppProperty('OutStreamType', 'AthenaOutputStream')

    #Build standard sequences:
    cfg.addSequence(AthSequencer('AthAlgEvtSeq',Sequential=True, StopOverride=True),parentName="AthMasterSeq") 
    cfg.addSequence(AthSequencer('AthOutSeq',StopOverride=True),parentName="AthMasterSeq")

    cfg.addSequence(AthSequencer('AthBeginSeq',Sequential=True),parentName='AthAlgEvtSeq')
    cfg.addSequence(AthSequencer('AthAllAlgSeq',StopOverride=True),parentName='AthAlgEvtSeq') 

    if cfgFlags.Concurrency.NumThreads==0:
        # For serial execution, we need the CondAlgs to execute first.
        cfg.addSequence(AthSequencer('AthCondSeq',StopOverride=True),parentName='AthAllAlgSeq')
        cfg.addSequence(AthSequencer('AthAlgSeq',IgnoreFilterPassed=True,StopOverride=True),parentName='AthAllAlgSeq')
    else:
        # In MT, the order of execution is irrelevant (determined by data deps).
        # We add the conditions sequence later such that the CondInputLoader gets
        # initialized after all other user Algorithms for MT, so the base classes
        # of data deps can be correctly determined. 
        cfg.addSequence(AthSequencer('AthAlgSeq',IgnoreFilterPassed=True,StopOverride=True),parentName='AthAllAlgSeq')
        cfg.addSequence(AthSequencer('AthCondSeq',StopOverride=True),parentName='AthAllAlgSeq')

    cfg.addSequence(AthSequencer('AthEndSeq',Sequential=True),parentName='AthAlgEvtSeq') 
    cfg.setAppProperty('PrintAlgsSequence', True)
    
    #Set up incident firing:
    AthIncFirerAlg=CompFactory.AthIncFirerAlg
    IncidentProcAlg=CompFactory.IncidentProcAlg

    cfg.addEventAlgo(AthIncFirerAlg("BeginIncFiringAlg",FireSerial=False,Incidents=['BeginEvent']),sequenceName='AthBeginSeq')
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg1'),sequenceName='AthBeginSeq')

    cfg.addEventAlgo(AthIncFirerAlg('EndIncFiringAlg',FireSerial=False,Incidents=['EndEvent']), sequenceName="AthEndSeq")
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg2'),sequenceName="AthEndSeq")

    #Basic services:
    ClassIDSvc=CompFactory.ClassIDSvc
    cfg.addService(ClassIDSvc(CLIDDBFiles= ['clid.db',"Gaudi_clid.db" ]))

    StoreGateSvc=CompFactory.StoreGateSvc
    cfg.addService(StoreGateSvc())
    cfg.addService(StoreGateSvc("DetectorStore"))
    cfg.addService(StoreGateSvc("HistoryStore"))
    cfg.addService(StoreGateSvc("ConditionStore"))
    
    cfg.addService(CompFactory.CoreDumpSvc())

    cfg.setAppProperty('InitializationLoopCheck',False)

    cfg.setAppProperty('EvtMax',cfgFlags.Exec.MaxEvents)

    msgsvc=CompFactory.MessageSvc()
    msgsvc.OutputLevel=cfgFlags.Exec.OutputLevel
    cfg.addService(msgsvc)

    if cfgFlags.Exec.DebugStage != "":
        cfg.setDebugStage(cfgFlags.Exec.DebugStage)


    ########################################################################
    # Additional components needed for threaded jobs only
    if cfgFlags.Concurrency.NumThreads>0:

        # Migrated code from AtlasThreadedJob.py
        AuditorSvc=CompFactory.AuditorSvc
        msgsvc.defaultLimit = 0
        msgsvc.Format = "% F%40W%S%4W%R%e%s%8W%R%T %0W%M"

        SG__HiveMgrSvc=CompFactory.SG.HiveMgrSvc
        hivesvc = SG__HiveMgrSvc("EventDataSvc")
        hivesvc.NSlots = cfgFlags.Concurrency.NumConcurrentEvents
        cfg.addService( hivesvc )

        AlgResourcePool=CompFactory.AlgResourcePool
        from AthenaCommon.Constants import INFO
        arp=AlgResourcePool( OutputLevel = INFO )
        arp.TopAlg=["AthMasterSeq"] #this should enable control flow
        cfg.addService( arp )

        AvalancheSchedulerSvc=CompFactory.AvalancheSchedulerSvc
        scheduler = AvalancheSchedulerSvc()
        scheduler.CheckDependencies    = cfgFlags.Scheduler.CheckDependencies
        scheduler.ShowDataDependencies = cfgFlags.Scheduler.ShowDataDeps
        scheduler.ShowDataFlow         = cfgFlags.Scheduler.ShowDataFlow
        scheduler.ShowControlFlow      = cfgFlags.Scheduler.ShowControlFlow
        scheduler.ThreadPoolSize       = cfgFlags.Concurrency.NumThreads
        cfg.addService(scheduler)

        SGInputLoader=CompFactory.SGInputLoader
        # FailIfNoProxy=False makes it a warning, not an error, if unmet data
        # dependencies are not found in the store.  It should probably be changed
        # to True eventually.
        inputloader = SGInputLoader (FailIfNoProxy = False)
        cfg.addEventAlgo( inputloader, "AthAlgSeq" )
        scheduler.DataLoaderAlg = inputloader.getName()

        AthenaHiveEventLoopMgr=CompFactory.AthenaHiveEventLoopMgr

        elmgr = AthenaHiveEventLoopMgr()
        elmgr.WhiteboardSvc = "EventDataSvc"
        elmgr.SchedulerSvc = scheduler.getName()
        cfg.addService( elmgr )

        # enable timeline recording
        TimelineSvc=CompFactory.TimelineSvc
        cfg.addService( TimelineSvc( RecordTimeline = True, Partial = False ) )

        #
        ## Setup SGCommitAuditor to sweep new DataObjects at end of Alg execute
        #
        SGCommitAuditor=CompFactory.SGCommitAuditor
        cfg.addService( AuditorSvc(Auditors=[SGCommitAuditor().getFullJobOptName(),]))
        cfg.setAppProperty("AuditAlgorithms", True)

    return cfg
    
