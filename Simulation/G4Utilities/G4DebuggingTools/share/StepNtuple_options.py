from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions():
    # new MT actions. Note that this will only work with one thread.
    simFlags.OptionalUserActionList.addAction('G4UA::StepNtupleTool',['BeginOfEvent','EndOfEvent','BeginOfRun','Step'])
else:
    # V1 userActions
    try:
        from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    except ImportError:
        from G4AtlasServices.UserActionStore import UAStore

    UAStore.addAction('StepNtuple',['BeginOfEvent','Step','EndOfEvent'])

# configure the NTupleSvc
theApp.HistogramPersistency = "ROOT"
svcMgr.NTupleSvc = Service( "NTupleSvc" )
svcMgr.NTupleSvc.Output = [ "FILE1 DATAFILE='stepntuple.root' TYP='ROOT' OPT='NEW'" ]

