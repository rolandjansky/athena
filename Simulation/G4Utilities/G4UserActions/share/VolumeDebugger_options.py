## Add a volume debug action to the list of user actions
# This should be configured to point at a specific volume
# It will then dump that volume to GDML and run an overlap
# checking routine on that volume

# Register callback functions at various init stages
from G4AtlasApps.SimFlags import simFlags
def vol_debug_Init():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    volDebugAction = PyG4Atlas.UserAction( 'G4UserActions','VolumeDebugger',['EndOfEvent'])
    volDebugAction.set_Properties({"outputPath" : "./test_volume.gdml", "targetVolume" : "CALO:CALO", "resolution" : "1000000", "tolerance" : "0.000001"})
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(volDebugAction)
simFlags.InitFunctions.add_function("postInitG4", vol_debug_Init) # Doesn't need a BeginOfRun

