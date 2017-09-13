from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::FastCaloSimParamActionTool',['BeginOfEvent','EndOfEvent','BeginOfRun','EndOfRun','Step'])
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool','shift_lar_subhit','1')
