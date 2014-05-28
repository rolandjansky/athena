#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool

## get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#Explicitly specify the output file catalog
from PoolSvc.PoolSvcConf import PoolSvc
svcMgr += PoolSvc()
svcMgr.PoolSvc.WriteCatalog = "xmlcatalog_file:Catalog1.xml"
 
from AthenaPoolCnvSvc.AthenaPoolCnvSvcConf import AthenaPoolCnvSvc
svcMgr += AthenaPoolCnvSvc()
svcMgr.AthenaPoolCnvSvc.CommitInterval = 10;
