#########################################################
##
## Change the G4 magnetic field stepper to G4NystromRK4
## This stepper is a G4-rewrite of G4AtlasRK4 and is
## NOT usable with a generic equation of motion, 
## including that for monopoles.
##
#########################################################

def setup_stepper_nystromrk4():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.fldMenu.UseStepper('NystromRK4')

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("postInit", setup_stepper_nystromrk4)
