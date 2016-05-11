from AthenaCommon.AppMgr import ServiceMgr
if not hasattr(ServiceMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    ServiceMgr += THistSvc("THistSvc")

ServiceMgr.THistSvc.Output  += ["truth DATAFILE='stepping.root' OPT='NEW'"];

from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions():
    # hive-friendly version
    simFlags.OptionalUserActionList.addAction('G4UA::SteppingValidationTool',['BeginOfRun','EndOfEvent','BeginOfEvent','Step'])
else:
    # V1 userActions
    try:
        from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    except ImportError:
        from G4AtlasServices.UserActionStore import UAStore
    # add default configurable
    UAStore.addAction('SteppingValidation',['Step','BeginOfEvent','EndOfEvent'])

