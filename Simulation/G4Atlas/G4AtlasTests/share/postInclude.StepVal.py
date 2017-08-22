from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc("THistSvc")

ServiceMgr.THistSvc.Output  += ["truth DATAFILE='stepping.root' OPT='NEW'"];

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::SteppingValidationTool',['Run','Event','Step'])

