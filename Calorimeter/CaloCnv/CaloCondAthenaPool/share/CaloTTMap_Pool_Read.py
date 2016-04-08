###############################################################
#
# Job options fragment to read LArTTCell map from POOL
#
#==============================================================

# Calo/LAr related
include( "CaloIdCnv/CaloIdCnv_joboptions.py" )

# this lib
theApp.Dlls += [ "CaloCondAthenaPoolPoolCnv" ]

import AthenaPoolCnvSvc.AthenaPool
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

svcMgr.ProxyProviderSvc.ProviderNames += [ "CondProxyProvider" ]
svcMgr.EventPersistencySvc.CnvServices += [ "AthenaPoolCnvSvc" ]

