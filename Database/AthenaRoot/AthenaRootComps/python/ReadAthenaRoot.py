# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# @file AthenaRootComps/python/ReadAthenaRoot.py
# @purpose make the Athena framework read a set of ROOT files to emulate the
#          usual TTree event loop
# @author Sebastien Binet <binet@cern.ch>
#
# Original code from Tadashi Maeno (AANTEventSelector)

def _configure():
    """Install the Athena-based TTree event selector and correctly configure
    a few other services.
    """
    from AthenaCommon import CfgMgr
    from AthenaCommon.AppMgr import theApp
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'ReadAthenaRoot' )
    msg.debug("Configuring Athena for reading ROOT files (via TChain)...")

    if not hasattr(svcMgr, 'THistSvc'):
        svcMgr += CfgMgr.THistSvc()

    if hasattr(svcMgr, 'EventSelector'):
        err = "svcMgr already configured with another EventSelector: [%s]"%\
            svcMgr.EventSelector.getFullJobOptName()
        msg.error( err )
        raise RuntimeError( err )

    # disable the event loop heartbeat as it is somewhat I/O hungry for
    # no real gain in n-tuple reading/writing scenarii
    if not hasattr(svcMgr, theApp.EventLoop):
        svcMgr += getattr(CfgMgr, theApp.EventLoop)()
    evtloop = getattr(svcMgr, theApp.EventLoop)
    try:
        msg.info('disabling event loop heartbeat...')
        evtloop.EventPrintoutInterval = 0
        msg.info('disabling event loop heartbeat... [done]')
    except Exception, err:
        msg.info('disabling event loop heartbeat... [failed]')
        msg.info('performances might be sub-par... sorry.')
        pass
        
    # Load ProxyProviderSvc
    if not hasattr (svcMgr, 'ProxyProviderSvc'):
        svcMgr += CfgMgr.ProxyProviderSvc()
        pass

    # from Configurables import Athena__RootNtupleEventSelector
    evtsel = CfgMgr.Athena__RootNtupleEventSelector( "EventSelector" )
    svcMgr += evtsel
    theApp.ExtSvc += [ evtsel.getFullName() ]
    theApp.EvtSel = "EventSelector"
    svcMgr.ProxyProviderSvc.ProviderNames += [evtsel.getFullName()]
    del evtsel
    
    # configure the cnvsvc
    svcMgr += CfgMgr.Athena__NtupleCnvSvc()
    if not hasattr(svcMgr, 'EventPersistencySvc'):
        svcMgr += CfgMgr.EvtPersistencySvc( "EventPersistencySvc" )
    svcMgr.EventPersistencySvc.CnvServices += [ "Athena::NtupleCnvSvc" ]
    
    msg.debug("Configuring Athena for reading ROOT files (via TChain)... [OK]")
    return

# execute at module import
_configure()

# clean-up namespace
del _configure
