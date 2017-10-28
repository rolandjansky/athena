include.block('SimulationJobOptions/preInclude.Cosmics.py')
##########################################################
##
## SimulationJobOptions/preInclude.Cosmics.py
## Zach Marshall
##
## Set up the environment for cosmics simulation
##########################################################

## Use beamType job property to indicate this is a cosmics job
from AthenaCommon.BeamFlags import jobproperties
if jobproperties.Beam.beamType.get_Value() != 'cosmics':
    jobproperties.Beam.beamType = "cosmics"
print "Cosmics-specific configuration of Simulation and Digitization requested"

## Switch off EventFilter
from G4AtlasApps.SimFlags import simFlags
simFlags.EventFilter.set_Off()

## Ensure that at least the first filter volume is switched on.
if not simFlags.CosmicFilterVolumeName.statusOn:
    print "CosmicFilterVolumeName not set. Using default = %s" % simFlags.CosmicFilterVolumeName.get_Value()
    simFlags.CosmicFilterVolumeName.set_On()


## Now setup cosmics truth strategies via callback functions

def cosmics_modify_truth_strategies():
    print "ERROR cosmics_modify_truth_strategies is obsolete"
    print "Please request a replacement configuration"
    import sys
    sys.exit(1)
    ## ## Modifying truth strategies as requested by e/gamma group
    ## from G4AtlasApps import AtlasG4Eng
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.listStrategies()
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Bremsstrahlung', 'CALO::CALO', 2)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Conversion', 'CALO::CALO', 2)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Decay', 'CALO::CALO', 2)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Ionization', 'CALO::CALO', 2)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('HadronicInteraction', 'CALO::CALO',2)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Compton', 'CALO::CALO', 2)


## Register the callbacks (at two different levels)
simFlags.InitFunctions.add_function("postInit", cosmics_modify_truth_strategies)
