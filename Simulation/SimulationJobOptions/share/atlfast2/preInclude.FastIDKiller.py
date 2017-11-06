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

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::FastIDKillerTool', ['Run','Step'])
