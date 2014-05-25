include.block ( "CaloConditions/LArTTCellMap_H8_jobOptions.py" )

###################################################################
#
# Job options file for setting up TT-Cell Map in H8
#
#==================================================================

from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr += PoolSvc()
if not ( hasattr(PoolSvc.ReadCatalog,'__len__') and "prfile:poolcond/PoolCat_tbcond.xml" in PoolSvc.ReadCatalog ):
    ServiceMgr.PoolSvc.ReadCatalog+= [ "prfile:poolcond/PoolCat_tbcond.xml" ]

if not hasattr(ServiceMgr, "CondProxyProvider"):
    from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import CondProxyProvider
    ServiceMgr += CondProxyProvider()

# LArTTCellMap
ServiceMgr.CondProxyProvider.InputCollections +=["LFN:tbcond.000002.conditions.recon.pool.v0000._0004.pool.root"]


