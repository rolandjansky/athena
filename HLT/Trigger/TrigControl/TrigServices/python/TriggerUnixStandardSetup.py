# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file TriggerUnixStandardSetup.py
## @brief py-module to configure the Athena AppMgr for trigger
## @author Werner Wiedenmann <Werner.Wiedenmann@cern.ch>
###############################################################

class _Conf:
    """Some configuration flags for this module with defaults"""
    useOnlineTHistSvc = True    # set via TrigServices/OfflineTHistSvc.py
    athenaXT = False            # set below in _setupCommonServices


def _setupRoot6IfNeeded():
    import cppyy
    try:
        # try to touch ROOT5-only attribute (success means we have ROOT5)
        cppyy.Cintex.Debug
    except AttributeError:
        # ROOT 6
        from PyUtils.Helpers import ROOT6Setup
        ROOT6Setup()

def _eventLoopMgr(svcMgr):
    if hasattr(svcMgr, 'HltEventLoopMgr'): return svcMgr.HltEventLoopMgr
    return None
    
def _setupCommonServices():
    from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR
    
    # Add timestamp to python logger
    from AthenaCommon.Logging import log
    log.setFormat("%(asctime)s  Py:%(name)-31s %(levelname)7s %(message)s")

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TriggerUnixStandardSetup::setupCommonServices:' )
     
    # Do the default Atlas job configuration first
    import AthenaCommon.AtlasUnixStandardJob

    # Now do HLT specific configuration
    from AthenaCommon import CfgMgr
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.AppMgr import ToolSvc

    # Check whether we are running in athenaXT
    # Only a minimal set of properties should depend on this
    import sys
    if sys.modules.has_key('HLTTestApps'):
        _Conf.athenaXT = True
        log.debug("Configuration for athenaXT running")
    else:
        _Conf.athenaXT = False
        log.debug("Configuration for online running")
        
    # setup ROOT6 if needed
    _setupRoot6IfNeeded()

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
        IdDictName = "IdDictParser/ATLAS_IDS.xml"
        )
    theApp.CreateSvc += [ svcMgr.DetDescrCnvSvc.getFullName() ]
    svcMgr.EventPersistencySvc.CnvServices += [ "DetDescrCnvSvc" ]

    # --- ByteStreamCnvSvc configuration
    svcMgr += CfgMgr.ByteStreamCnvSvc("ByteStreamCnvSvc")
    svcMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]
    
    # Disable history
    svcMgr += CfgMgr.HistorySvc()
    svcMgr.HistorySvc.Activate = False

    # Configuration of Interval of Validity Service
    svcMgr += CfgMgr.IOVSvc()
    
    # Configure TrigISHelper
    from TrigServices.TrigServicesConf import TrigISHelper
    ToolSvc += TrigISHelper("TrigISHelper")

    # Configure TrigPreFlightCheck
    from TrigServices.TrigServicesConf import TrigPreFlightCheck
    ToolSvc += TrigPreFlightCheck("TrigPreFlightCheck",
                                  ReleaseDirs = ["AtlasP1HLT","AtlasHLT"]
                                  )

    # Configure CoreDumpSvc
    if not hasattr(svcMgr,"CoreDumpSvc"):
        from AthenaServices.Configurables import CoreDumpSvc
        svcMgr += CoreDumpSvc()
        
    # Configure COOL update helper tool
    from TrigServices.TrigServicesConfig import TrigCOOLUpdateHelper
    _eventLoopMgr(svcMgr).CoolUpdateTool = TrigCOOLUpdateHelper()
            
    # Setup online THistSvc unless specifically configured otherwise
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

    # Explicitly set a few OutputLevels (needed because some services are created in
    # different order when running with the PSC)
    svcMgr.StatusCodeSvc.OutputLevel = theApp.OutputLevel
    svcMgr.IncidentSvc.OutputLevel = theApp.OutputLevel
    svcMgr.ProxyProviderSvc.OutputLevel = theApp.OutputLevel
    svcMgr.StoreGateSvc.OutputLevel = theApp.OutputLevel
    
    return


def _setupCommonServicesEnd():
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr    
    from AthenaCommon.Logging import logging
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    
    log = logging.getLogger( 'TriggerUnixStandardSetup::setupCommonServicesEnd:' )
    
    # --- create the ByteStreamCnvSvc after the Detector Description otherwise
    # --- the initialization of converters fails
    theApp.CreateSvc += [ svcMgr.ByteStreamCnvSvc.getFullName() ]    

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
        
    # Flag to extract trigger configuration
    if TriggerFlags.Online.doDBConfig():
        from TrigConfigSvc import DoDBConfig
        
    # --- print out configuration details
    _printConfiguration(log.name)
    
    return 
    
def _printConfiguration(loggerName):
    from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR

    from AthenaCommon.Logging import logging
    if loggerName == '':
        loggerName = 'TriggerUnixStandardSetup::_printConfiguration'
        
    log = logging.getLogger( loggerName )
    
    # --- print out configuration details
    # ---
    from AthenaCommon.AppMgr import theApp
    log.debug("---> Application Manager")
    log.debug(theApp)
    
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    log.debug("---> Service Manager")
    log.debug(svcMgr)

    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    log.debug("---> Algorithm Sequence")
    log.debug(topSequence)
    
    return

def setupHLTServicesBegin():
    from AthenaCommon import CfgMgr
    from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TriggerUnixStandardSetup::setupHLTServicesBegin:' )
    log.debug( "---> Start" )

    # --- setup the standard services
    _setupCommonServices()   

    # --- Hlt ROBDataProvider configuration
    svcMgr += CfgMgr.HltROBDataProviderSvc("ROBDataProviderSvc")
    theApp.CreateSvc += [ svcMgr.ROBDataProviderSvc.getFullName() ]

    log.debug( "---> End" )
    return

def setupHLTServicesEnd():
    from AthenaCommon.Constants import VERBOSE, DEBUG, INFO, ERROR

    from AthenaCommon.Logging import logging
    log = logging.getLogger( 'TriggerUnixStandardSetup::setupHLTServicesEnd:' )
    log.debug( "---> Start" )

    # --- final modifications to standard services
    _setupCommonServicesEnd()      

    log.debug( "---> End" )
    return
