###############################################################################
#
# Fragment to add calorimeter decays and hadronic interactions to the truth tree
#
# Implemented for the B-physics folks who want to estimate fakes well
# Results in a pretty major blow-up of hit file size - use with care!
#
###############################################################################

def add_calo_decay_truth_strategies():
    ## Modifying truth strategies as requested by e/gamma group
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.listStrategies()
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Decay', 'CALO::CALO', 1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('HadronicInteraction', 'CALO::CALO',1)

## Register the callback
simFlags.InitFunctions.add_function("postInit", add_calo_decay_truth_strategies)
