# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file TriggerUnixStandardSetup.py
## @brief py-module to configure the Athena AppMgr for trigger
## @author Werner Wiedenmann <Werner.Wiedenmann@cern.ch>
###############################################################

class _Conf:
    """Some configuration flags for this module with defaults"""
    useOnlineTHistSvc = True    # set in athenaHLT.py

def setupCommonServices():
    from AthenaCommon import CfgMgr
    from AthenaCommon.Logging import logging
    from AthenaCommon.Constants import INFO
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr, theApp
    from AthenaCommon.ConcurrencyFlags import jobproperties as jps

    # Setup messaging for Python and C++
    from AthenaCommon.Logging import log
    log.setFormat("%(asctime)s  Py:%(name)-31s %(levelname)7s %(message)s")

    # Create our own logger
    log = logging.getLogger( 'TriggerUnixStandardSetup::setupCommonServices:' )

    from TrigServices.TrigServicesConfig import setupMessageSvc
    setupMessageSvc()
     
    # Do the default Atlas job configuration first
    import AthenaCommon.AtlasUnixStandardJob   # noqa: F401

    # Now do HLT/thread specific configuration (see e.g. AtlasThreadedJob.py)
    from StoreGate.StoreGateConf import SG__HiveMgrSvc
    svcMgr += SG__HiveMgrSvc("EventDataSvc",
                             NSlots = jps.ConcurrencyFlags.NumConcurrentEvents())

    import StoreGate.StoreGateConf as StoreGateConf
    svcMgr += StoreGateConf.StoreGateSvc("ConditionStore")

    # ThreadPoolService thread local initialization
    from GaudiHive.GaudiHiveConf import ThreadPoolSvc
    svcMgr += ThreadPoolSvc("ThreadPoolSvc")
    svcMgr.ThreadPoolSvc.ThreadInitTools = ["ThreadInitTool"]

    from GaudiHive.GaudiHiveConf import AlgResourcePool
    svcMgr += AlgResourcePool( OutputLevel = INFO,
                               TopAlg=["AthMasterSeq"])       # this should enable control flow

    from AthenaCommon.AlgSequence import AlgSequence
    from SGComps.SGCompsConf import SGInputLoader
    topSequence = AlgSequence()
    topSequence += SGInputLoader(FailIfNoProxy = False)  # change to True eventually

    from AthenaCommon.AlgScheduler import AlgScheduler
    AlgScheduler.ShowDataDependencies(False)
    AlgScheduler.ShowControlFlow(False)
    AlgScheduler.setDataLoaderAlg ('SGInputLoader' )

    # Setup SGCommitAuditor to sweep new DataObjects at end of Alg execute
    theApp.AuditAlgorithms = True
    from SGComps.SGCompsConf import SGCommitAuditor
    svcMgr.AuditorSvc += SGCommitAuditor()

    # setup ROOT6
    from PyUtils.Helpers import ROOT6Setup
    ROOT6Setup()

    # Setup online THistSvc unless specifically configured otherwise
    #    setup the THistSvc early and force the creation of the ThistSvc 
    #    so that it can be used by infrastructure services to book histograms  
    #    (to avoid problems e.g. with histograms in ROBDataProviderSvc)
    if _Conf.useOnlineTHistSvc:
        if hasattr(svcMgr, 'THistSvc'):
            log.fatal("The offline histogramming THistSvc is already in place.")
            raise RuntimeError("Cannot setup online histogramming TrigMonTHistSvc")
        log.debug("Using online histogramming service (TrigMonTHistSvc)")
        from TrigServices.TrigServicesConf import TrigMonTHistSvc
        svcMgr += TrigMonTHistSvc("THistSvc")
    else:
        log.debug("Using offline histogramming service (THistSvc)")
        from GaudiSvc.GaudiSvcConf import THistSvc
        svcMgr += THistSvc()
    theApp.CreateSvc += [ svcMgr.THistSvc.getFullName() ]

    # StoreGateSvc
    svcMgr.StoreGateSvc.ActivateHistory = False
    
    # ProxyProviderSvc services configuration
    svcMgr += CfgMgr.ProxyProviderSvc()

    # --- ByteStreamAddressProviderSvc configuration
    svcMgr += CfgMgr.ByteStreamAddressProviderSvc()
    svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]
    theApp.CreateSvc += [ svcMgr.ByteStreamAddressProviderSvc.getFullName() ]

    # Initialization of DetDescrCnvSvc
    svcMgr += CfgMgr.DetDescrCnvSvc(
        # specify primary Identifier dictionary to be used
        IdDictName = "IdDictParser/ATLAS_IDS.xml")

    theApp.CreateSvc += [ svcMgr.DetDescrCnvSvc.getFullName() ]
    svcMgr.EventPersistencySvc.CnvServices += [ "DetDescrCnvSvc" ]

    # Online services for ByteStream input/output
    from TrigByteStreamCnvSvc.TrigByteStreamCnvSvcConf import TrigByteStreamCnvSvc, TrigByteStreamInputSvc, TrigEventSelectorByteStream
    svcMgr += TrigByteStreamCnvSvc("ByteStreamCnvSvc") # this name is hard-coded in some converters
    svcMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]
    svcMgr += TrigByteStreamInputSvc("ByteStreamInputSvc")
    svcMgr += TrigEventSelectorByteStream("EventSelector", ByteStreamInputSvc = svcMgr.ByteStreamInputSvc)
    theApp.EvtSel = "EventSelector"

    # make the HltEventLoopMgr service available
    svcMgr.HltEventLoopMgr = theApp.service( "HltEventLoopMgr" )     # already instantiated
    svcMgr.HltEventLoopMgr.WhiteboardSvc = "EventDataSvc"
    svcMgr.HltEventLoopMgr.SchedulerSvc = AlgScheduler.getScheduler().getName()
    svcMgr.HltEventLoopMgr.EvtSel = svcMgr.EventSelector
    svcMgr.HltEventLoopMgr.OutputCnvSvc = svcMgr.ByteStreamCnvSvc

    from TrigOutputHandling.TrigOutputHandlingConfig import HLTResultMTMakerCfg
    svcMgr.HltEventLoopMgr.ResultMaker = HLTResultMTMakerCfg()

    # Configuration of Interval of Validity Service
    svcMgr += CfgMgr.IOVSvc()
    
    # Configure COOL update helper tool
    from TrigServices.TrigServicesConfig import TrigCOOLUpdateHelper
    svcMgr.HltEventLoopMgr.CoolUpdateTool = TrigCOOLUpdateHelper()
            
    # Configure the online ROB data provider service
    from TrigServices.TrigServicesConfig import HltROBDataProviderSvc
    svcMgr += HltROBDataProviderSvc()  

    # Explicitly set a few OutputLevels (needed because some services are created in
    # different order when running with the PSC)
    svcMgr.StatusCodeSvc.OutputLevel = theApp.OutputLevel
    svcMgr.IncidentSvc.OutputLevel = theApp.OutputLevel
    svcMgr.ProxyProviderSvc.OutputLevel = theApp.OutputLevel
    svcMgr.StoreGateSvc.OutputLevel = theApp.OutputLevel
    
    return


def setupCommonServicesEnd():
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr    
    from AthenaCommon.Logging import logging

    log = logging.getLogger( 'TriggerUnixStandardSetup::setupCommonServicesEnd:' )
    
    # --- create the ByteStreamCnvSvc after the Detector Description otherwise
    # --- the initialization of converters fails
    #from AthenaCommon.AppMgr import theApp
    #theApp.CreateSvc += [ svcMgr.ByteStreamCnvSvc.getFullName() ]

    # Make sure no THistSvc output/input stream is defined for online running
    if _Conf.useOnlineTHistSvc:
        svcMgr.THistSvc.Output = []
        if len(svcMgr.THistSvc.Input)>0:
            log.error('THistSvc.Input = %s. Input not allowed for online running. Disabling input.' % svcMgr.THistSvc.Input)
            svcMgr.THistSvc.Input = []

    # For offline running make sure at least the EXPERT stream is defined
    else:
        if 1 not in [ o.count('EXPERT') for o in svcMgr.THistSvc.Output ]:
            svcMgr.THistSvc.Output += ["EXPERT DATAFILE='expert-monitoring.root' OPT='RECREATE'"]

    # Set default properties for some important services after all user job options
    log.info('Configure core services for online runnig')

    svcMgr.CoreDumpSvc.CoreDumpStream = "stdout"
    svcMgr.CoreDumpSvc.CallOldHandler = True
    svcMgr.CoreDumpSvc.FatalHandler = 0   # no extra fatal handler
    svcMgr.CoreDumpSvc.TimeOut = 60000000000        # no timeout for stack trace generation -> changed to 60s (ATR17112)

    # Disable StatusCodeSvc (causes problems with shutting down children at stop in HLTPU)
    svcMgr.StatusCodeSvc.SuppressCheck = True
    svcMgr.StatusCodeSvc.AbortOnError = False
        
    svcMgr.IOVSvc.updateInterval = "RUN"
    svcMgr.IOVSvc.preLoadData = True  
    svcMgr.IOVSvc.forceResetAtBeginRun = False 

    if hasattr(svcMgr,'IOVDbSvc'):
        svcMgr.IOVDbSvc.CacheAlign = 0  # VERY IMPORTANT to get unique queries for folder udpates (see Savannah #81092)
        svcMgr.IOVDbSvc.CacheRun = 0
        svcMgr.IOVDbSvc.CacheTime = 0

    return
