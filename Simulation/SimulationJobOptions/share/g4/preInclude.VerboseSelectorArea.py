# Fragment for setting up the verbose selector based on area.
# Can be used to enable G4 verbose output for a specific event, and for a specific region of the detector if desired.
# This is VERY handy for looking into problems in G4.
# Verbose level is transmitted to several G4 classes to increase their verbosity.
# Note that the event number is the number of events into the G4 run, rather than the ATLAS event number.

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::VerboseSelectorTool',['Event','Tracking','Step'])
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','Xmin',-25000)
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','Xmax',25000)
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','Ymin',-25000)
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','Ymax',25000)
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','Zmin',-30000)
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','Zmax',30000)
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','TargetEvent',1)
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','VerboseLevel',1)

