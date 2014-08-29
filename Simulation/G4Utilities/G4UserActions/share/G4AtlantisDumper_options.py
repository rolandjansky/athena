## Add G4AtlantisDumper to the UserActions

def add_g4atlantisdumper():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    myG4AtlantisDumperAction = PyG4Atlas.UserAction('G4UserActions', 'G4AtlantisDumper', ['BeginOfRun', 'EndOfRun', 'BeginOfEvent', 'EndOfEvent', 'Step'])
    myG4AtlantisDumperAction.set_Properties({"Edep" : "0.1", "Length" : "1.0", "P" : "10.0", "KE" : "10.0"})
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myG4AtlantisDumperAction)

from G4AtlasApps.SimFlags import SimFlags
SimFlags.InitFunctions.add_function("postInitG4", add_g4atlantisdumper)

