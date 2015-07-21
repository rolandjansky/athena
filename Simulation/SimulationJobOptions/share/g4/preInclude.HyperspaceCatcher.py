# Fragment for setting up the hyperspace catcher
# This code watches out for particles that have entered hyperspace and kill them
# This is VERY handy for looking into problems in G4 - it should NOT be used for standard production.
# Set "killAfter" in order to crash the job after a certain number of hyperspace particles

def hyperspaceCatcher_setup():
    print 'Firing up the HYPERSPACE CATCHER'
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    myAction = PyG4Atlas.UserAction('G4UserActions', 'HyperspaceCatcher', ['Step'])
    myAction.set_Properties( {"treatmentLevel":"1","killAfter":"-1"} )
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myAction)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", hyperspaceCatcher_setup)

