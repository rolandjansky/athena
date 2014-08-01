# Modify the hit wrapping action to wrap on 1600 ns

def cbwrapper_setup():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    myAction = PyG4Atlas.UserAction('G4UserActions', 'HitWrapper', ['EndOfEvent'])
    myAction.set_Properties( {"WrapTime" : "1600"} )
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myAction)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("postInit", cbwrapper_setup)
