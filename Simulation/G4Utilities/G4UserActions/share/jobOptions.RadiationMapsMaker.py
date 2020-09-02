#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
from G4AtlasApps.SimFlags import simFlags
from G4UserActions.G4UserActionsConf import G4UA__RadiationMapsMakerTool
#
# to change configurables modify them like in the next lines; example values reflect defaults
#
# radmaptool = G4UA__RadiationMapsMakerTool("G4UA::RadiationMapsMakerTool")
# radmaptool.RadMapsFileName = "RadMaps.root" 
# radmaptool.Material        = "" # if left empty all materials are used (default)
# radmaptool.PositiveYOnly   = False
# radmaptool.NBinsR          =   120
# radmaptool.NBinsZ          =   240
# radmaptool.NBinsLogEn      =    90
# radmaptool.NBinsLogEo      =    45
# radmaptool.NBinsLogTimeCut =    20
# radmaptool.NBinsDPhi       =    18
# radmaptool.NBinsTheta      =     9
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
# radmaptool.LogEMinn        =  -11.0  # in log10(E/MeV)
# radmaptool.LogEMaxn        =    7.0  # in log10(E/MeV)
# radmaptool.LogEMino        =   -2.0  # in log10(E/MeV)
# radmaptool.LogEMaxo        =    7.0  # in log10(E/MeV)
# radmaptool.LogTMin         =   -9.0  # in log10(t_cut/s)
# radmaptool.LogTMax         =   11.0  # in log10(t_cut/s)
# radmaptool.ThetaMin        =    0.0  # in degrees 
# radmaptool.ThetaMin        =   90.0  # in degrees
# radmaptool.ElemZMin        =    1    # Atomic number
# radmaptool.ElemZMax        =   92    # Atomic number
#
simFlags.OptionalUserActionList.addAction('G4UA::RadiationMapsMakerTool')

