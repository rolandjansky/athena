forwardTransportSvc.FillRootTree = True

from GaudiSvc.GaudiSvcConf import THistSvc
THistSvc = THistSvc()
THistSvc.Output = ["AANT DATAFILE='fwdTransport.root' OPT='RECREATE'"]
svcMgr += THistSvc
