#########################################################
#
# SimulationJobOptions/postOptions.FastMBKiller.py
# Zach Marshall
#
# Requested for the Atlfast-II and MinBias groups
#########################################################

def fastmbkiller_setup():
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    myAction = PyG4Atlas.UserAction('G4UserActions','FastIDKiller', ['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
    myAction.set_Properties( {"Z":"3600" , "R":"1400"} )
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myAction)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", fastmbkiller_setup)
