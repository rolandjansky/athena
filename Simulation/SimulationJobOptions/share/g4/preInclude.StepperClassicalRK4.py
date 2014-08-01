#########################################################
##
## Change the G4 magnetic field stepper to G4ClassicalRK4
## This stepper is ~15% slower than the G4AtlasRK4 and
## G4NystromRK4 steppers, but is usable with a generic
## equation of motion, including that for monopoles.
##
#########################################################

def setup_stepper_classicalrk4():
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.fldMenu.UseStepper('ClassicalRK4')

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("postInit", setup_stepper_classicalrk4)
