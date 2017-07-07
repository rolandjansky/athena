###############################################################################
#
# Fragment to add calorimeter decays to the truth tree
#
# Implemented for the B-physics folks who want to estimate fakes well
#
###############################################################################

def add_calo_decay_truth_strategies():
    print "ERROR add_calo_decay_truth_strategies is obsolete"
    ## ## Modifying truth strategies as requested by e/gamma group
    ## from G4AtlasApps import AtlasG4Eng
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.listStrategies()
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Decay', 'CALO::CALO', 1)


## Register the callback
simFlags.InitFunctions.add_function("postInit", add_calo_decay_truth_strategies)
