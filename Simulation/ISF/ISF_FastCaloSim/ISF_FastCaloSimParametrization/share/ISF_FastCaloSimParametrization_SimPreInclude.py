from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::FastCaloSimParamActionTool',['Event','Run','Step'])
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"shift_lar_subhit",True)
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"shorten_lar_step",True)
# optimised merging scheme parameters
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxEtaPS",1)
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxPhiPS",5)
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxrPS",0)

simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxEtaEM1",1)
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxPhiEM1",5)
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxrEM1",15)

simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxEtaEM2",1)
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxPhiEM2",5)
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxrEM2",60)

simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxEtaEM3",1)
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxPhiEM3",5)
simFlags.UserActionConfig.addConfig('G4UA::FastCaloSimParamActionTool',"maxrEM3",8)
