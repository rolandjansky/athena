from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions():
    # new MT actions. Note that this will only work with one thread.
    simFlags.OptionalUserActionList.addAction('G4UA::G4AtlantisDumperTool',[ 'BeginOfEvent', 'EndOfEvent', 'Step'])
else:
    # V1 userActions
    try:
        from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    except ImportError:
        from G4AtlasServices.UserActionStore import UAStore

    UAStore.addAction('G4AtlantisDumper',[ 'BeginOfEvent', 'EndOfEvent', 'Step'])

