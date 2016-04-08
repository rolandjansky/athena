
HistOutFile = "PanTau_RunTimeHists.root"

# Add Histogram service
THistSvc = Service("THistSvc")
THistSvc.Output = ["PanTauHists DATAFILE='" + HistOutFile + "' OPT='RECREATE'"]
THistSvc.PrintAll = 1
ServiceMgr += THistSvc