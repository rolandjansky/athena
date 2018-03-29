from G4AtlasApps.SimFlags import simFlags
# new MT actions. Note that this will only work with one thread.
simFlags.OptionalUserActionList.addAction('G4UA::G4AtlantisDumperTool', ['Event', 'Step'])

