include.block("LArConditionsCommon/LArIdMap_H8_jobOptions.py")

#####################################################################
# Job options file to read Id mapping for LAr H8 2004 Beam test setup
# Use CondProxyProviderSvc since there is no IOV and mapping is needed
# at initialization phase.


#include ("LArRawConditions/LArRawConditionsDict_joboptions.py")
  
include( "AthenaPoolCnvSvc/AthenaPool_jobOptions.py" )
PoolSvc = Service( "PoolSvc" )
if not (hasattr(PoolSvc.ReadCatalog,'__len__') and "prfile:poolcond/PoolCat_tbcond.xml" in PoolSvc.ReadCatalog):
   PoolSvc.ReadCatalog+= [ "prfile:poolcond/PoolCat_tbcond.xml" ]

from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
ServiceMgr += CondProxyProvider()
ServiceMgr.ProxyProviderSvc.ProviderNames += ["CondProxyProvider"]
ServiceMgr.CondProxyProvider.InputCollections += ["LFN:tbcond.000003.conditions.recon.pool.v0000._0019.pool.root"]

IOVSvc = Service("IOVSvc")
print IOVSvc
IOVSvc.preLoadData = True
print IOVSvc
