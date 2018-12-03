# Job options to configure the Geant4 setup checker tool
from G4AtlasApps.SimFlags import simFlags
simFlags.OptionalUserActionList.addAction('G4UA::Geant4SetupCheckerTool',['Run'])

# This flag disables checking so that you can write your own reference file
#simFlags.UserActionConfig.addConfig('G4UA::Geant4SetupCheckerTool','RunTest',False)
