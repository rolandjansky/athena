# Include the verbose selector based on track ID
# Very handy for debugging crashes on the grid when you know what track caused the crash
# Most powerful in conjunction with the verbose selector area

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::VerboseSelectorTool',['Event','Tracking','Step'])
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','TargetEvent',1)
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','TargetTrack',15932)
simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','VerboseLevel',2)
#simFlags.UserActionConfig.addConfig('G4UA::VerboseSelectorTool','TargetBarcode',-1)
