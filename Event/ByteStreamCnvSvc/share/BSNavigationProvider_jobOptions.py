###############################################################
#
# Job options file for navigating to BS input file.
#==============================================================

"""
 -+- 19 330 A           svcMgr.ProxyProviderSvc.ProviderNames += [ "ByteStreamNavigationProviderSvc" ]
 -+- 19 331
 -+- 19 332 A           EventPersistencySvc = svcMgr.EventPersistencySvc
 -+- 19 333 A           EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]
 -+- 19 334 A           svcMgr += Service("ByteStreamCnvSvc")
"""
# get a handle on the ServiceManager
svcMgr = theApp.serviceMgr()

# Services

# for EventType
from ByteStreamCnvSvc.ByteStreamCnvSvcConf import ByteStreamCnvSvc
svcMgr += ByteStreamCnvSvc()

# Properties 
EventPersistencySvc = svcMgr.EventPersistencySvc
EventPersistencySvc.CnvServices += [ "ByteStreamCnvSvc" ]

# ByteStreamNavigationProviderSvc
from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamNavigationProviderSvc
svcMgr += ByteStreamNavigationProviderSvc()
ByteStreamNavigationProviderSvc = svcMgr.ByteStreamNavigationProviderSvc

# proxy provider
from SGComps.SGCompsConf import ProxyProviderSvc
svcMgr += ProxyProviderSvc()
ProxyProviderSvc = svcMgr.ProxyProviderSvc
ProxyProviderSvc.ProviderNames += [ "ByteStreamNavigationProviderSvc" ]
