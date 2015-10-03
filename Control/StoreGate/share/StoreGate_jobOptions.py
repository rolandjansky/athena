from AthenaCommon.AppMgr import theApp, ServiceMgr, theAuditorSvc
theApp.CreateSvc += [ "StoreGateSvc/DetectorStore", "StoreGateSvc" ]
theApp.ExtSvc += [ "StoreGateSvc/HistoryStore" ]

from AthenaCommon.ConfigurableDb import getConfigurable
ServiceMgr += getConfigurable("ClassIDSvc")()
ServiceMgr.ClassIDSvc.CLIDDBFiles += [ "Gaudi_clid.db" ]

from AthenaCommon.AppMgr import theAuditorSvc
from GaudiAud.GaudiAudConf import AlgContextAuditor
theAuditorSvc += AlgContextAuditor()
