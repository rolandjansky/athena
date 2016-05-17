## Add a volume debug action to the list of user actions
# This should be configured to point at a specific volume
# It will then dump that volume to GDML and run an overlap
# checking routine on that volume


from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions():
    # new MT actions. Note that this will only work with one thread.
    simFlags.OptionalUserActionList.addAction('G4UA::VolumeDebuggerTool',['BeginOfRun'])
else:
    # V1 userActions
    try:
        from G4AtlasServices.G4AtlasUserActionConfig import UAStore
    except ImportError:
        from G4AtlasServices.UserActionStore import UAStore

    # property setting can be done using ConfiguredFactory, i.e.
    # with something like
    #
    # from AthenaCommon.CfgGetter import getPublicTool
    # getPublicTool('VolumeDebugger').property=value

    UAStore.addAction('VolumeDebugger',['BeginOfRun'])

    # optional config
    #from AthenaCommon.CfgGetter import getPublicTool
    #getPublicTool('VolumeDebugger',tryDefaultConfigurable=True).RunGeoTest=True
    #getPublicTool('VolumeDebugger').TargetVolume='Muon::Muon'
    #getPublicTool('VolumeDebugger').OutputPath='./test.gdml'
