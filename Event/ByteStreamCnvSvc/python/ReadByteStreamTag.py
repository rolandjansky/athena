# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

###############################################################
#
# module for reading EventStorage BS input file.
#==============================================================

from AthenaCommon.AppMgr import theApp

# get a handle on the ServiceManager
svcMgr = theApp.serviceMgr()

# Need Pool to read TAG in Pool format
import AthenaPoolCnvSvc.ReadAthenaPool

# Services
#from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ROBDataProviderSvc
#svcMgr += ROBDataProviderSvc()

# for EventType
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()
svcMgr.EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

# ByteStreamAddressProviderSvc
from ByteStreamCnvSvcBase. ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc
svcMgr += ByteStreamAddressProviderSvc()
ByteStreamAddressProviderSvc = svcMgr.ByteStreamAddressProviderSvc

# specific for tag
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamNavigationProviderSvc
svcMgr += ByteStreamNavigationProviderSvc( "ByteStreamNavigationProviderSvc" )
svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamNavigationProviderSvc" ]

# Basic metadata
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamMetadataTool
svcMgr.MetaDataSvc.MetaDataTools += [ "ByteStreamMetadataTool" ]

# User metadata in FMD
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamAttListMetadataSvc
svcMgr += ByteStreamAttListMetadataSvc("ByteStreamAttListMetadataSvc")

