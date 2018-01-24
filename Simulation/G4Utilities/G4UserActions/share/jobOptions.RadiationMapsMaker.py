from G4AtlasServices import G4AtlasServicesConfig
from G4UserActions.G4UserActionsConf import G4UA__RadiationMapsMakerTool
radmaptool = G4UA__RadiationMapsMakerTool("G4UA::RadiationMapsMakerTool")
G4AtlasServicesConfig.addAction("G4UA::RadiationMapsMakerTool",['Run','Step'],False) 

