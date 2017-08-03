## Add RadLengthIntegrator to the UserActions, and produce a histogram
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::LengthIntegratorTool',['BeginOfEvent','EndOfEvent','Step'])


from AthenaCommon.AppMgr import ServiceMgr
from GaudiSvc.GaudiSvcConf import THistSvc
ServiceMgr += THistSvc("THistSvc")
ServiceMgr.THistSvc.Output  = ["lengths DATAFILE='rad_intLength.histo.root' OPT='RECREATE'"]
