from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc()

ServiceMgr.THistSvc.Output  = ["simTime DATAFILE='g4profiling.root' OPT='RECREATE'"];

print ServiceMgr.THistSvc

from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions():
    # configuration for MT actions
    simFlags.OptionalUserActionList.addAction('G4UA::TestActionTimerTool',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
    simFlags.OptionalUserActionList.addAction('G4UA::TestActionVPTimerTool',['BeginOfEvent','EndOfEvent','EndOfRun','BeginOfRun','Step'])
    simFlags.OptionalUserActionList.addAction('G4UA::TestActionEHistTool',['BeginOfRun','EndOfRun','Step','BeginOfTracking','EndOfTracking'])
else:
    # V1 userActions
    try:
        from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    except ImportError:
        from G4AtlasServices.UserActionStore import UAStore
    # add default configurable
    UAStore.addAction('TestActionEHist',['BeginOfRun','EndOfRun','Step','BeginOfTracking','EndOfTracking'])
    UAStore.addAction('TestActionTimer',['BeginOfRun','EndOfRun','Step','BeginOfEvent','EndOfEvent'])
    UAStore.addAction('TestActionVPTimer',['EndOfRun','Step','BeginOfEvent','EndOfEvent'])
