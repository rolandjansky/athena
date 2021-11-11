##########################################################
#
# SimulationJobOptions/preInclude.G4GammaGeneralProcess.py
# Marilena Bandieramonte
#
# Activate the G4GammaGeneralProcess and the corresponding
# job options
#  
##########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.G4Commands+=["/process/em/UseGeneralProcess true"];
simFlags.OptionalUserActionList.addAction("G4UA::FixG4CreatorProcessTool")
