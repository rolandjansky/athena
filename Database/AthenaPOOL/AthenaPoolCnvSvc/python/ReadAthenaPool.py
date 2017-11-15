# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @file ReadAthena.py
## @brief AthenaPool python module for reading event objects.
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: ReadAthenaPool.py,v 1.11 2008-11-18 22:44:00 binet Exp $
###############################################################
"""
 This is a module for reading event objects in AthenaPool.
 It loads the conversion service shared lib, and the EventInfo converter.

 When using Explicit Collection, a query can be applied to the collection attributes
 through the EventSelector for preselecting events. Note only attributes are (run,event) for now.
 For example:

 EventSelector.query = 'event > 10 && event < 100';

 will select events that have event number in the specified range.
"""

def _configureReadAthenaPool():
    """ Helper method to configure Athena to read back POOL files """

    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'configureReadAthenaPool' )
    msg.debug( "Configuring Athena for reading POOL files..." )

    from AthenaCommon import CfgMgr
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.AppMgr import theApp

    # Load the basic services
    import AthenaPoolCnvSvc.AthenaPool

    # Load ProxyProviderSvc
    if not hasattr (svcMgr, 'ProxyProviderSvc'):
        svcMgr += CfgMgr.ProxyProviderSvc()

    # Add in MetaDataSvc
    if not hasattr (svcMgr, 'MetaDataSvc'):
        svcMgr += CfgMgr.MetaDataSvc ("MetaDataSvc")
    svcMgr.ProxyProviderSvc.ProviderNames += [ "MetaDataSvc" ]

    # Add in MetaData Stores
    if not hasattr (svcMgr, 'InputMetaDataStore'):
        svcMgr += CfgMgr.StoreGateSvc ("InputMetaDataStore")
    if not hasattr (svcMgr, 'MetaDataStore'):
        svcMgr += CfgMgr.StoreGateSvc ("MetaDataStore")

    # Enable IOVDbSvc to read MetaData
    svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
    svcMgr.MetaDataSvc.MetaDataTools += [ "IOVDbMetaDataTool" ]
    if not hasattr (svcMgr.ToolSvc, 'IOVDbMetaDataTool'):
        svcMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()

    # Add in EventSelector
    svcMgr += CfgMgr.EventSelectorAthenaPool ("EventSelector")
    #default InputCollections to FilesInput value of AthenaCommonFlags
    from AthenaCommon.JobProperties import jobproperties as jps
    svcMgr.EventSelector.InputCollections = jps.AthenaCommonFlags.FilesInput()
    
    _n = svcMgr.EventSelector.getFullJobOptName()
    theApp.EvtSel = _n
    del _n

    # For Analysis release use DataHeader satellite and lower heartbeat
    import os 
    if "AthAnalysisBase" in os.environ.get('CMTEXTRATAGS',""): 
        #Sep2016: disabled satellite for now, until fully debugged
        #svcMgr.EventSelector.CollectionTree = "POOLContainer/basic"
        # From Will Buttinger to suppress the event loop heartbeat as it is somewhat I/O hungry for 
        # no real gain in analysis scenarii 
        if not hasattr(svcMgr, theApp.EventLoop): 
            svcMgr += getattr(CfgMgr, theApp.EventLoop)() 
        evtloop = getattr(svcMgr, theApp.EventLoop) 
        try: 
            evtloop.EventPrintoutInterval = 10000 
        except Exception, err: 
            msg.info('failed suppressing event loop heartbeat. performances might be sub-par... sorry.') 
            pass 

    # Add in AthenaPoolAddressProviderSvc
    if not hasattr (svcMgr, 'AthenaPoolAddressProviderSvc'):
        svcMgr += CfgMgr.AthenaPoolAddressProviderSvc ("AthenaPoolAddressProviderSvc")
    svcMgr.ProxyProviderSvc.ProviderNames += [ "AthenaPoolAddressProviderSvc" ]

    # Set up DataVector/DataProxyStorage backwards compatibility.
    #from DataModelAthenaPool import DataModelCompatSvc
    svcMgr += CfgMgr.DataModelCompatSvc()

    # Always want AddressRemappingSvc, since that's responsible for suppressing
    # the read of objects that are produced by a WriteHandle.
    from SGComps.AddressRemappingSvc import getAddressRemappingSvc
    getAddressRemappingSvc()

    msg.debug( "Configuring Athena for reading POOL files... [DONE]" )
    return

## configuration at module import
_configureReadAthenaPool()

## clean-up: avoid running multiple times this method
del _configureReadAthenaPool
