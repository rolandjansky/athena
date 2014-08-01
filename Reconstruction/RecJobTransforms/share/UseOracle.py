# setup DBReplicaSvc to choose closest Oracle replica, configurables style
from AthenaCommon.AppMgr import ServiceMgr
from PoolSvc.PoolSvcConf import PoolSvc
ServiceMgr+=PoolSvc(SortReplicas=True)
from DBReplicaSvc.DBReplicaSvcConf import DBReplicaSvc
ServiceMgr+=DBReplicaSvc(UseCOOLSQLite=False)
