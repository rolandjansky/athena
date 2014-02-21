#
# $Id: LArG4FastSimulation_setupTimer_jobOptions.py 541012 2013-03-14 20:54:37Z zmarshal $
#
# job options for calculating timing information
#

#
# Include this file in your simulation job options to apply the timer
#

def LArG4Timer_preG4Init():
    from G4AtlasApps import PyG4Atlas,AtlasG4Eng
    MyAction = PyG4Atlas.UserAction('LArG4FastSimulationTestActions','TestActionTimer',['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(MyAction)
simFlags.InitFunctions.add_function("preInitG4", LArG4Timer_preG4Init)

