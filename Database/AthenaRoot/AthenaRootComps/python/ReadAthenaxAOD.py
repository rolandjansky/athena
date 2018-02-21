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
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    msg = logging.getLogger( 'ReadAthenaxAOD' )
    msg.debug("Configuring Athena for reading xAOD files (via TEvent)...")

    #check if we already have a selector set up
    if hasattr(svcMgr, 'EventSelector'):
        err = "svcMgr already configured with another EventSelector: [%s]"%\
            svcMgr.EventSelector.getFullJobOptName()
        msg.error( err )
        raise RuntimeError( err )

   
        
    #Setup our EventSelector
    svcMgr += CfgMgr.Athena__xAODEventSelector( "EventSelector" )
    
    #for historical reasons, we now add configurables of a bunch of services
    if not hasattr(svcMgr, 'THistSvc'): svcMgr += CfgMgr.THistSvc()
    if not hasattr (svcMgr, 'ProxyProviderSvc'): svcMgr += CfgMgr.ProxyProviderSvc()
    if not hasattr (svcMgr, 'InputMetaDataStore'): svcMgr += CfgMgr.StoreGateSvc("InputMetaDataStore")
    if not hasattr (svcMgr, 'Athena::xAODCnvSvc'): svcMgr += CfgMgr.Athena__xAODCnvSvc()

    #Here we set various properties of things 
    theApp.ExtSvc += [ svcMgr.EventSelector.getFullName() ]
    theApp.EvtSel = "EventSelector"
    #default the input collections to the FilesInput from AthenaCommonFlags
    #this is so that the eventselector picks up input files in grid jobs
    svcMgr.EventSelector.InputCollections = athenaCommonFlags.FilesInput() 
    

    # suppress the event loop heartbeat as it is somewhat I/O hungry for
    # no real gain in n-tuple reading/writing scenarii
    if not hasattr(svcMgr, theApp.EventLoop): svcMgr += getattr(CfgMgr, theApp.EventLoop)()
    evtloop = getattr(svcMgr, theApp.EventLoop)
    try:
        evtloop.EventPrintoutInterval = 10000
    except Exception, err:
        msg.info('disabling event loop heartbeat... [failed]')
        msg.info('performances might be sub-par... sorry.')
        pass


    
    msg.debug("Configuring Athena for reading ROOT files (via xAOD)... [OK]")
    return

# execute at module import
_configure()

# clean-up namespace
del _configure
