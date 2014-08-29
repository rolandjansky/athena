#==============================================================
#
#
#		This job option fragment for the G4 simulation
#		of the ATLAS detector adds the FastIDKiller.
#       You can include it in your job options or as a 
#       preConfig file when running with AtlasG4_trf.py
#
__version__="$Revision: 1.3 $"
#==============================================================

def FastIDKiller_preG4Init():
    from G4AtlasApps import PyG4Atlas,AtlasG4Eng
    # Enable the FastIDKiller
    MyAction = PyG4Atlas.UserAction('G4UserActions','FastIDKiller', ['BeginOfRun','EndOfRun','BeginOfEvent','EndOfEvent','Step'])
    AtlasG4Eng.G4Eng.menu_UserActions.add_UserAction(MyAction)
from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", FastIDKiller_preG4Init)

#--- End jobOptions.FastIDKiller.py file  ------------------------------

