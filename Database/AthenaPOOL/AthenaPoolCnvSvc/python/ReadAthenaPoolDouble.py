# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

## @file ReadAthena.py
## @brief AthenaPool python module for reading event objects.
## @author Peter van Gemmeren <gemmeren@bnl.gov>
## $Id: ReadAthenaPool.py,v 1.11 2008-11-18 22:44:00 binet Exp $
###############################################################
"""
    Copy of the one made for standard EventSelector to work with
    the DoubleEventSelector.
"""

def _configureReadAthenaPoolDouble():
    """ Helper method to configure Athena to read back POOL files """

    from AthenaCommon.Logging import logging
    msg = logging.getLogger( 'configureReadAthenaPool' )
    msg.debug( "Configuring Athena for reading POOL files..." )

    from AthenaCommon import CfgMgr
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.AppMgr import theApp

    # Load the basic services
    import AthenaPoolCnvSvc.AthenaPool

    # Switch on TTreeCache for CollectionTree
    svcMgr.AthenaPoolCnvSvc.InputPoolAttributes += [ "DatabaseName = '*'; ContainerName = 'CollectionTree'; TREE_CACHE = '-1'" ]

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
    svcMgr += CfgMgr.EventSelectorAthenaPool("SecondaryEventSelector", IsSecondary=True)
    svcMgr += CfgMgr.DoubleEventSelectorAthenaPool("DoubleEventSelector")    
    theApp.EvtSel = "DoubleEventSelectorAthenaPool/DoubleEventSelector"

    # Add in AthenaPoolAddressProviderSvc
    if not hasattr (svcMgr, 'AthenaPoolAddressProviderSvcPrimary'):
        svcMgr += CfgMgr.AthenaPoolAddressProviderSvc ("AthenaPoolAddressProviderSvcPrimary")
    svcMgr.ProxyProviderSvc.ProviderNames += [ "AthenaPoolAddressProviderSvc/AthenaPoolAddressProviderSvcPrimary" ]
    svcMgr.AthenaPoolAddressProviderSvcPrimary.DataHeaderKey = "EventSelector"
    svcMgr.AthenaPoolAddressProviderSvcPrimary.AttributeListKey = "Input"
    if not hasattr (svcMgr, 'AthenaPoolAddressProviderSvcSecondary'):
        svcMgr += CfgMgr.AthenaPoolAddressProviderSvc ("AthenaPoolAddressProviderSvcSecondary")
    svcMgr.ProxyProviderSvc.ProviderNames += [ "AthenaPoolAddressProviderSvc/AthenaPoolAddressProviderSvcSecondary" ]
    svcMgr.AthenaPoolAddressProviderSvcSecondary.DataHeaderKey = "SecondaryEventSelector"

    # Set up DataVector/DataProxyStorage backwards compatibility.
    #from DataModelAthenaPool import DataModelCompatSvc
    svcMgr += CfgMgr.DataModelCompatSvc()

    # Always want AddressRemappingSvc, since that's responsible for suppressing
    # the read of objects that are produced by a WriteHandle.
    from SGComps.AddressRemappingSvc import getAddressRemappingSvc
    getAddressRemappingSvc()

    msg.debug( "Configuring Athena for reading two POOL files concurrently... [DONE]" )
    return

## configuration at module import
_configureReadAthenaPoolDouble()

## clean-up: avoid running multiple times this method
del _configureReadAthenaPoolDouble
