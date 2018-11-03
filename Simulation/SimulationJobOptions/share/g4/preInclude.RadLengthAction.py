# Include the RadLengthAction UserAction
# Used for production of X0 maps in the muon system

from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::RadLengthActionTool',['Event','Step','Run'])
#simFlags.UserActionConfig.addConfig('G4UA::RadLengthActionTool','VolumeDepthLevel',1)
