# Fragment for setting up the hyperspace catcher
# This code watches out for particles that have entered hyperspace and kill them
# This is VERY handy for looking into problems in G4 - it should NOT be used for standard production.
# Set "killAfter" in order to crash the job after a certain number of hyperspace particles

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::HyperspaceCatcherTool', ['Run','Step'])
simFlags.UserActionConfig.addConfig('G4UA::HyperspaceCatcherTool', 'TreatmentLevel',1)
simFlags.UserActionConfig.addConfig('G4UA::HyperspaceCatcherTool', 'KillAfter',-1)
