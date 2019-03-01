# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# module for reading EventStorage BS input file.
#==============================================================

from AthenaCommon import CfgMgr
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AppMgr import theApp

# Load ByteStreamEventStorageInputSvc
if not hasattr (svcMgr, 'ByteStreamInputSvc'):
    svcMgr += CfgMgr.ByteStreamEventStorageInputSvc ("ByteStreamInputSvc")

# Load ROBDataProviderSvc
if not hasattr (svcMgr, 'ROBDataProviderSvc'):
    svcMgr += CfgMgr.ROBDataProviderSvc ("ROBDataProviderSvc")

# Load EventSelectorByteStream
if not hasattr (svcMgr, 'EventSelector'):
    svcMgr += CfgMgr.EventSelectorByteStream ("EventSelector")
theApp.EvtSel = "EventSelector"

#from xAODEventInfoCnv.xAODEventInfoCnvConf import xAODMaker__EventInfoSelectorTool 
#xconv = xAODMaker__EventInfoSelectorTool()
#svcMgr.EventSelector.HelperTools += [xconv]

# Load ByteStreamCnvSvc
if not hasattr (svcMgr, 'ByteStreamCnvSvc'):
    svcMgr += CfgMgr.ByteStreamCnvSvc ("ByteStreamCnvSvc")

# Properties
svcMgr.EventSelector.ByteStreamInputSvc = "ByteStreamInputSvc";

svcMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

# Load ProxyProviderSvc
if not hasattr (svcMgr, 'ProxyProviderSvc'):
    svcMgr += CfgMgr.ProxyProviderSvc()

# Add in ByteStreamAddressProviderSvc
if not hasattr (svcMgr, 'ByteStreamAddressProviderSvc'):
    svcMgr += CfgMgr.ByteStreamAddressProviderSvc ("ByteStreamAddressProviderSvc")
svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamAddressProviderSvc" ]

# Add in MetaDataSvc
if not hasattr (svcMgr, 'MetaDataSvc'):
    svcMgr += CfgMgr.MetaDataSvc ("MetaDataSvc")
svcMgr.ProxyProviderSvc.ProviderNames += [ "MetaDataSvc" ]

# Add in MetaData Stores
from StoreGate.StoreGateConf import StoreGateSvc
if not hasattr (svcMgr, 'InputMetaDataStore'):
    svcMgr += StoreGateSvc( "MetaDataStore" )
if not hasattr (svcMgr, 'MetaDataStore'):
    svcMgr += StoreGateSvc( "InputMetaDataStore" )

# enable IOVDbSvc to read metadata
svcMgr.MetaDataSvc.MetaDataContainer = "MetaDataHdr"
svcMgr.MetaDataSvc.MetaDataTools += [ "IOVDbMetaDataTool" ]

if not hasattr (svcMgr.ToolSvc, 'IOVDbMetaDataTool'):
    svcMgr.ToolSvc += CfgMgr.IOVDbMetaDataTool()

# Enable ByteStream to read MetaData
svcMgr.MetaDataSvc.MetaDataTools += [ "ByteStreamMetadataTool" ]
if not hasattr (svcMgr.ToolSvc, 'ByteStreamMetadataTool'):
    svcMgr.ToolSvc += CfgMgr.ByteStreamMetadataTool()
    svcMgr.ToolSvc.ByteStreamMetadataTool.InputCollName = "ByteStreamMetadata"
    svcMgr.ToolSvc.ByteStreamMetadataTool.OutputCollName = "ByteStreamMetadata"

# User metadata in FMD
if not hasattr (svcMgr, 'ByteStreamAttListMetadataSvc'):
    svcMgr += CfgMgr.ByteStreamAttListMetadataSvc ("ByteStreamAttListMetadataSvc")
