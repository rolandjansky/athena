#########################################################
#
# SimulationJobOptions/preInclude.FastMBKiller.py
# Zach Marshall
#
# Requested for the Atlfast-II and MinBias groups
#########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::FastMBKillerTool', ['Run','Step'])
