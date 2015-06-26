# Turn on check mode for the G4Navigator at BeginOfEvent

def navigatorCheckMode_setup():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    myAction = PyG4Atlas.UserAction('G4UserActions', 'CheckActivation', ['BeginOfEvent'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myAction)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", navigatorCheckMode_setup)

