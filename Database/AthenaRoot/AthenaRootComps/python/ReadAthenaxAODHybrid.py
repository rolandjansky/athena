# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file AthenaRootComps/python/ReadAthenaxAODHybrid.py
# @purpose make the Athena framework read a set of xAOD files to emulate the
#          usual TEvent event loop ... BUT READ METADATA WITH POOL!
# @author Will Buttinger and Johannes Elmsheuser
#


def _configure():
    """Install the Athena-based xAOD event selector and correctly configure
    a few other services.
    """
    from AthenaCommon import CfgMgr
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.Logging import logging
    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from AthenaCommon.Constants import ERROR


    msg = logging.getLogger( 'ReadAthenaxAODHybrid' )
    msg.debug("Configuring Athena for reading xAOD files (via TEvent, with POOL for Metadata)...")

    
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
    if not hasattr(svcMgr, 'EventPersistencySvc'): svcMgr += CfgMgr.EvtPersistencySvc( "EventPersistencySvc" )
    if not hasattr (svcMgr, 'MetaDataSvc'): svcMgr += CfgMgr.MetaDataSvc ("MetaDataSvc")
    if not hasattr(svcMgr, 'PoolSvc'): svcMgr += CfgMgr.PoolSvc()

    #Here we set various properties of things 
    theApp.ExtSvc += [ svcMgr.EventSelector.getFullName() ]
    theApp.EvtSel = "EventSelector"
    svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr" #this really should be the default for this property :-(
    svcMgr.PoolSvc.OutputLevel = ERROR
    svcMgr.EventSelector.ReadMetaDataWithPool=True
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


    msg.debug("Configuring Athena for reading ROOT files (via TEvent, with POOL for Metadata)... [OK]")
    return

# execute at module import
_configure()

# clean-up namespace
del _configure
