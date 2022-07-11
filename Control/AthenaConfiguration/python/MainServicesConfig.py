# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
AthSequencer=CompFactory.AthSequencer

def MainServicesMiniCfg(loopMgr='AthenaEventLoopMgr', masterSequence='AthAlgSeq'):
    #Mininmal basic config, just good enough for HelloWorld and alike
    cfg=ComponentAccumulator(AthSequencer(masterSequence,Sequential=True))
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


def AthenaEventLoopMgrCfg(cfgFlags):

    cfg = ComponentAccumulator()
    elmgr = CompFactory.AthenaEventLoopMgr()
    if cfgFlags.Input.OverrideRunNumber:
        from AthenaKernel.EventIdOverrideConfig import EvtIdModifierSvcCfg
        elmgr.EvtIdModifierSvc =  cfg.getPrimaryAndMerge( EvtIdModifierSvcCfg(cfgFlags) ).name

    if cfgFlags.Common.isOverlay:
        if not cfgFlags.Overlay.DataOverlay:
            elmgr.RequireInputAttributeList = True
            elmgr.UseSecondaryEventNumber = True

    cfg.addService( elmgr )

    return cfg


def AthenaHiveEventLoopMgrCfg(cfgFlags):

    cfg = ComponentAccumulator()
    hivesvc = CompFactory.SG.HiveMgrSvc("EventDataSvc")
    hivesvc.NSlots = cfgFlags.Concurrency.NumConcurrentEvents
    cfg.addService( hivesvc )

    from AthenaCommon.Constants import INFO
    arp = CompFactory.AlgResourcePool( OutputLevel = INFO )
    arp.TopAlg = ["AthMasterSeq"] #this should enable control flow
    cfg.addService( arp )

    scheduler = CompFactory.AvalancheSchedulerSvc()
    scheduler.CheckDependencies    = cfgFlags.Scheduler.CheckDependencies
    scheduler.ShowDataDependencies = cfgFlags.Scheduler.ShowDataDeps
    scheduler.ShowDataFlow         = cfgFlags.Scheduler.ShowDataFlow
    scheduler.ShowControlFlow      = cfgFlags.Scheduler.ShowControlFlow
    scheduler.VerboseSubSlots      = cfgFlags.Scheduler.EnableVerboseViews
    scheduler.ThreadPoolSize       = cfgFlags.Concurrency.NumThreads
    cfg.addService(scheduler)

    from SGComps.SGInputLoaderConfig import SGInputLoaderCfg
    # FailIfNoProxy=False makes it a warning, not an error, if unmet data
    # dependencies are not found in the store.  It should probably be changed
    # to True eventually.
    inputloader_ca = SGInputLoaderCfg(cfgFlags, FailIfNoProxy=cfgFlags.Input.FailOnUnknownCollections)
    cfg.merge(inputloader_ca, sequenceName="AthAlgSeq")
    # Specifying DataLoaderAlg makes the Scheduler automatically assign
    # all unmet data dependencies to that algorithm
    if cfgFlags.Scheduler.AutoLoadUnmetDependencies:
        scheduler.DataLoaderAlg = inputloader_ca.getPrimary().getName()

    elmgr = CompFactory.AthenaHiveEventLoopMgr()
    elmgr.WhiteboardSvc = "EventDataSvc"
    elmgr.SchedulerSvc = scheduler.getName()
    elmgr.OutStreamType = 'AthenaOutputStream'

    if cfgFlags.Input.OverrideRunNumber:
        from AthenaKernel.EventIdOverrideConfig import EvtIdModifierSvcCfg
        elmgr.EvtIdModifierSvc =  cfg.getPrimaryAndMerge(EvtIdModifierSvcCfg(cfgFlags)).name

    if cfgFlags.Common.isOverlay:
        if not cfgFlags.Overlay.DataOverlay:
            elmgr.RequireInputAttributeList = True
            elmgr.UseSecondaryEventNumber = True

    cfg.addService( elmgr )

    return cfg


def MainServicesCfg(cfgFlags, LoopMgr='AthenaEventLoopMgr'):
    # Run a serial job for threads=0
    if cfgFlags.Concurrency.NumThreads>0:
        if cfgFlags.Concurrency.NumConcurrentEvents==0:
            # In a threaded job this will mess you up because no events will be processed
            raise Exception("Requested Concurrency.NumThreads>0 and Concurrency.NumConcurrentEvents==0, which will not process events!")
        LoopMgr = "AthenaHiveEventLoopMgr"

    if cfgFlags.Concurrency.NumProcs>0:
        LoopMgr = "AthMpEvtLoopMgr"

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
        cfg.addSequence(AthSequencer('AthAlgSeq',IgnoreFilterPassed=True,StopOverride=True, ProcessDynamicDataDependencies=True, ExtraDataForDynamicConsumers=[] ),parentName='AthAllAlgSeq')
    else:
        # In MT, the order of execution is irrelevant (determined by data deps).
        # We add the conditions sequence later such that the CondInputLoader gets
        # initialized after all other user Algorithms for MT, so the base classes
        # of data deps can be correctly determined. 
        cfg.addSequence(AthSequencer('AthAlgSeq', IgnoreFilterPassed=True, StopOverride=True, ProcessDynamicDataDependencies=True, ExtraDataForDynamicConsumers=[]), parentName='AthAllAlgSeq')
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

    # Should be after all other algorithms.
    cfg.addEventAlgo(AthIncFirerAlg('EndAlgorithmsFiringAlg',FireSerial=False,Incidents=['EndAlgorithms']), sequenceName="AthMasterSeq")
    cfg.addEventAlgo(IncidentProcAlg('IncidentProcAlg3'),sequenceName="AthMasterSeq")

    #Basic services:
    ClassIDSvc=CompFactory.ClassIDSvc
    cfg.addService(ClassIDSvc(CLIDDBFiles= ['clid.db',"Gaudi_clid.db" ]))

    AlgContextSvc=CompFactory.AlgContextSvc
    cfg.addService(AlgContextSvc(BypassIncidents=True))
    cfg.addAuditor(CompFactory.AlgContextAuditor())

    StoreGateSvc=CompFactory.StoreGateSvc
    cfg.addService(StoreGateSvc())
    cfg.addService(StoreGateSvc("DetectorStore"))
    cfg.addService(StoreGateSvc("HistoryStore"))
    cfg.addService(StoreGateSvc("ConditionStore"))
    cfg.addService(CompFactory.CoreDumpSvc(FastStackTrace=True), create=True)

    cfg.setAppProperty('InitializationLoopCheck',False)

    cfg.setAppProperty('EvtMax',cfgFlags.Exec.MaxEvents)

    msgsvc=CompFactory.MessageSvc()
    msgsvc.OutputLevel=cfgFlags.Exec.OutputLevel
    msgsvc.Format = "% F%{:d}W%C%7W%R%T %0W%M".format(cfgFlags.Common.MsgSourceLength)
    cfg.addService(msgsvc)

    if cfgFlags.Exec.DebugStage != "":
        cfg.setDebugStage(cfgFlags.Exec.DebugStage)

    if cfgFlags.Concurrency.NumProcs>0:
        from AthenaMP.AthenaMPConfig import AthenaMPCfg
        mploop = AthenaMPCfg(cfgFlags)
        cfg.merge(mploop)

    ########################################################################
    # Additional components needed for threaded jobs only
    if cfgFlags.Concurrency.NumThreads>0:

        # Migrated code from AtlasThreadedJob.py
        msgsvc.defaultLimit = 0
        msgsvc.Format = "% F%{:d}W%C%6W%R%e%s%8W%R%T %0W%M".format(cfgFlags.Common.MsgSourceLength)

        cfg.merge(AthenaHiveEventLoopMgrCfg(cfgFlags))

        #
        ## Setup SGCommitAuditor to sweep new DataObjects at end of Alg execute
        #
        cfg.addAuditor( CompFactory.SGCommitAuditor() )
        cfg.setAppProperty("AuditAlgorithms", True)
    elif LoopMgr == 'AthenaEventLoopMgr':
        cfg.merge(AthenaEventLoopMgrCfg(cfgFlags))

    return cfg

def MainEvgenServicesCfg(cfgFlags, LoopMgr='AthenaEventLoopMgr',seqName="AthAlgSeq"):
    """ComponentAccumulator-based equivalent of:
    import AthenaCommon.AtlasUnixGeneratorJob

    NB Must have set ConfigFlags.Input.RunNumber and
    ConfigFlags.Input.TimeStamp before calling to avoid
    attempted auto-configuration from an input file.
    """
    cfg = MainServicesCfg(cfgFlags,LoopMgr)
    from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
    cfg.merge (McEventSelectorCfg (cfgFlags))
    # Temporarily inject the xAOD::EventInfo converter here to allow for adiabatic migration of the clients
    cfg.addEventAlgo(CompFactory.xAODMaker.EventInfoCnvAlg(AODKey = 'McEventInfo'),sequenceName=seqName)
    return cfg

if __name__=="__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    try:
        ConfigFlags.Input.RunNumber = 284500 # Set to either MC DSID or MC Run Number
        ConfigFlags.Input.TimeStamp = 1 # dummy value
        cfg = MainEvgenServicesCfg(ConfigFlags)
    except ModuleNotFoundError:
        #  The McEventSelector package required by MainEvgenServicesCfg is not part of the AthAnalysis project
        cfg = MainServicesCfg(ConfigFlags)
    cfg._wasMerged = True   # to avoid errror that CA was not merged
