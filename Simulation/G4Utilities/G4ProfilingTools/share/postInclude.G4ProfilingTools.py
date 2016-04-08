from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc("THistSvc")
    ServiceMgr.THistSvc.Output  = ["profiling DATAFILE='g4profiling.root' OPT='NEW'"];

from G4AtlasServices.G4AtlasUserActionConfig import UAStore
# add default configurable
#UAStore.addAction('TestActionEHist',['BeginOfRun','EndOfRun','Step','BeginOfTracking','EndOfTracking'])
UAStore.addAction('TestActionTimer',['BeginOfRun','EndOfRun','Step','BeginOfEvent','EndOfEvent'])
UAStore.addAction('TestActionVPTimer',['EndOfRun','Step','BeginOfEvent','EndOfEvent'])
