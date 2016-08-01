# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file AthenaRootComps/python/ReadAthenaxAOD.py
# @purpose make the Athena framework read a set of xAOD files to emulate the
#          usual TEvent event loop
# @author Will Buttinger and Johannes Elmsheuser
#

def _configure():
    """Install the Athena-based TTree event selector and correctly configure
    a few other services.
    """
    from AthenaCommon import CfgMgr
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'ReadAthenaxAOD' )
    msg.debug("Configuring Athena for reading xAOD files (via TEvent)...")

    if not hasattr(svcMgr, 'THistSvc'):
        svcMgr += CfgMgr.THistSvc()

    if hasattr(svcMgr, 'EventSelector'):
        err = "svcMgr already configured with another EventSelector: [%s]"%\
            svcMgr.EventSelector.getFullJobOptName()
        msg.error( err )
        raise RuntimeError( err )

    # suppress the event loop heartbeat as it is somewhat I/O hungry for
    # no real gain in n-tuple reading/writing scenarii
    if not hasattr(svcMgr, theApp.EventLoop):
        svcMgr += getattr(CfgMgr, theApp.EventLoop)()
    evtloop = getattr(svcMgr, theApp.EventLoop)
    try:
        evtloop.EventPrintoutInterval = 10000
    except Exception, err:
        msg.info('disabling event loop heartbeat... [failed]')
        msg.info('performances might be sub-par... sorry.')
        pass
        
    # Load ProxyProviderSvc
    if not hasattr (svcMgr, 'ProxyProviderSvc'):
        svcMgr += CfgMgr.ProxyProviderSvc()
        pass
    
    # Load a InputMetaDataStore
    if not hasattr (svcMgr, 'InputMetaDataStore'):
       svcMgr += CfgMgr.StoreGateSvc("InputMetaDataStore")
    
    # from Configurables import Athena__RootNtupleEventSelector
    evtsel = CfgMgr.Athena__xAODEventSelector( "EventSelector" )
    svcMgr += evtsel
    theApp.ExtSvc += [ evtsel.getFullName() ]
    theApp.EvtSel = "EventSelector"
    #svcMgr.ProxyProviderSvc.ProviderNames += [evtsel.getFullName()]
    del evtsel

    #default the input collections to the FilesInput from AthenaCommonFlags
    #this is so that the eventselector picks up input files in grid jobs
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput()
    
    # configure the cnvsvc
    svcMgr += CfgMgr.Athena__xAODCnvSvc()
    if not hasattr(svcMgr, 'EventPersistencySvc'):
        svcMgr += CfgMgr.EvtPersistencySvc( "EventPersistencySvc" )
    svcMgr.EventPersistencySvc.CnvServices += [ "Athena::xAODCnvSvc" ]



    
    msg.debug("Configuring Athena for reading ROOT files (via xAOD)... [OK]")
    return

# execute at module import
_configure()

# clean-up namespace
del _configure
