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
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::FastIDKillerTool',['Run','Step'])

#--- End jobOptions.FastIDKiller.py file  ------------------------------

