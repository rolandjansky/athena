from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["simTime DATAFILE='g4profiling.root' OPT='RECREATE'"];

print ServiceMgr.THistSvc

from G4AtlasApps.SimFlags import simFlags
# configuration for MT actions
simFlags.OptionalUserActionList.addAction('G4UA::TestActionTimerTool',['Event','Run','Step'])
simFlags.OptionalUserActionList.addAction('G4UA::TestActionVPTimerTool',['Event','Run','Step'])
simFlags.OptionalUserActionList.addAction('G4UA::TestActionEHistTool',['Run','Step','Tracking'])
