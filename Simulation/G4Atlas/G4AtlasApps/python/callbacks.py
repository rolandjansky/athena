# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

"""
Collection of standard callback functions to be used in the sim initialisation, e.g.

from G4AtlasApps import callbacks
SimFlags.InitFunctions.add_function("postInit", callbacks.use_classicalrk4_stepper)
"""


## Change the field stepper
def use_simplerunge_stepper():
    from G4AtlasApps.SimFlags import simFlags
    simFlags.G4Stepper = 'SimpleRunge'

def use_classicalrk4_stepper():
    from G4AtlasApps.SimFlags import simFlags
    simFlags.G4Stepper = 'ClassicalRK4'

def use_nystromrk4_stepper():
    from G4AtlasApps.SimFlags import simFlags
    simFlags.G4Stepper = 'NystromRK4'

## Use verbose G4 tracking
def use_verbose_tracking():
    from G4AtlasApps.SimFlags import simFlags
    simFlags.G4Commands += ['/tracking/verbose 1']

## Do a recursive geometry test
def do_recursive_geometry_test():
    # For syntax and state requirements see:
    # http://www-geant4.kek.jp/lxr/source//geometry/navigation/src/G4GeometryMessenger.cc
    from G4AtlasApps.SimFlags import simFlags
    simFlags.G4Commands += ["/geometry/test/recursion_start 0"]
    simFlags.G4Commands += ["/geometry/test/recursion_depth 2"]
    simFlags.G4Commands += ["/geometry/test/run"]  # Needs to be run in G4State_Idle!! I.e. After geometry Initialized

# Add a truth catch for LLP decay processes
def add_LLP_truth_strategies():
    print "ERROR add_LLP_truth_strategies is obsolete"
    print "Please request a replacement configuration"
    import sys
    sys.exit(1)
    ## from G4AtlasApps import AtlasG4Eng,PyG4Atlas
    ## astrategy=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'LLP','IDET::IDET',1)
    ## astrategy.add_Volumes('CALO::CALO',1)
    ## astrategy.add_Volumes('BeamPipe::BeamPipe',1)
    ## mcTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
    ## mcTruthMenu.add_McTruthStrategy(astrategy)

def add_EnergyConservationTest():
    from G4AtlasApps.SimFlags import simFlags
    # Enable the energy conservation test action
    simFlags.OptionalUserActionList.addAction(
        'G4UA::EnergyConservationTestTool', ['Event', 'Tracking', 'Step'])

## Range cuts for gamma processes (conv, phot, compt)
## these are off by default in Geant4
def turn_on_gamma_range_cuts():
    from G4AtlasApps.SimFlags import simFlags
    simFlags.G4Commands += ['/process/em/applyCuts true']
