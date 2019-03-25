#########################################################
#
# G4UserActions/preInclude.VolumeDumper.py
# @author Miha Muskinja
#
# Dumps all volume names and classifies them with G4DebuggingHelper
#
#########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::VolumeDumperTool')
