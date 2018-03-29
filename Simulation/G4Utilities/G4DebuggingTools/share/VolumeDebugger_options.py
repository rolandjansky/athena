## Add a volume debug action to the list of user actions
# This should be configured to point at a specific volume
# It will then dump that volume to GDML and run an overlap
# checking routine on that volume


from G4AtlasApps.SimFlags import simFlags
# new MT actions. Note that this will only work with one thread.
simFlags.OptionalUserActionList.addAction('G4UA::VolumeDebuggerTool',['Run'])

