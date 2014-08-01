#########################################################
#
# SimulationJobOptions/postInitOptions.FastIDKiller.py
# Michael Duehrssen
#
# jobOption to call the FastIDKiller while running
# a G4 simulation of the ATLAS detector.
# The FastIDKiller will kill all particles leaving the
# inner detector except all muons or an e/gamma with an
# energy above a certain threshold.
#########################################################

def fastidkiller_setup():
    ## Add the FastIDKiller to the UserActions
    from G4AtlasApps import PyG4Atlas, AtlasG4Eng
    myAction = PyG4Atlas.UserAction('G4UserActions', 'FastIDKiller', ['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(myAction)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", fastidkiller_setup)
