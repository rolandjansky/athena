from G4AtlasApps.SimFlags import simFlags
from G4UserActions.G4UserActionsConf import G4UA__RadiationMapsMakerTool
#
# to change configurables modify them like in the next lines; example values reflect defaults
#
# radmaptool = G4UA__RadiationMapsMakerTool("G4UA::RadiationMapsMakerTool")
# radmaptool.RadMapsFileName = "RadMaps.root" 
# radmaptool.Material        = "" # if left empty all materials are used (default)
# radmaptool.NBinsR          =   120
# radmaptool.NBinsZ          =   240
# radmaptool.NBinsR3D        =    30
# radmaptool.NBinsZ3D        =    60
# radmaptool.NBinsPhi3D      =    32
# radmaptool.RMinZoom        =     0.0 # in cm 
# radmaptool.RMaxZoom        =   480.0 # in cm  
# radmaptool.ZMinZoom        =     0.0 # in cm 
# radmaptool.ZMaxZoom        =   960.0 # in cm 
# radmaptool.RMinFull        =     0.0 # in cm 
# radmaptool.RMaxFull        =  1200.0 # in cm  
# radmaptool.ZMinFull        =     0.0 # in cm 
# radmaptool.ZMaxFull        =  2400.0 # in cm 
# radmaptool.PhiMinZoom      =  -180.0 # in degrees
# radmaptool.PhiMaxZoom      =   180.0 # in degrees
#
simFlags.OptionalUserActionList.addAction('G4UA::RadiationMapsMakerTool',['Run','Step'])

