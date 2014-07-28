############ FastOR Root ntuple #############
# save ROOT histograms and Tuple
from AthenaCommon.AppMgr import ServiceMgr
from AthenaCommon.AppMgr import theApp
theApp.HistogramPersistency = "ROOT"
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc()
ServiceMgr.THistSvc.Output = ["AANT DATAFILE='FastOR.root' OPT='RECREATE'"]
