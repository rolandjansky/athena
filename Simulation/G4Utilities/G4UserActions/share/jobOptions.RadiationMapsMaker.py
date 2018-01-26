from G4AtlasApps.SimFlags import simFlags
from G4UserActions.G4UserActionsConf import G4UA__RadiationMapsMakerTool
radmaptool = G4UA__RadiationMapsMakerTool("G4UA::RadiationMapsMakerTool")
#
# to change configurables modify them like in the next line (default is RadMaps.root):
# radmaptool.RadMapsFileName = "RadiationMaps.root"
#
simFlags.OptionalUserActionList.addAction('G4UA::RadiationMapsMakerTool',['Run','Step'])

