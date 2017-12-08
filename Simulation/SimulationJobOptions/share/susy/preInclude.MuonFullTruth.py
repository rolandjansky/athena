#################################################
#  preInclude.MuonFullTruth.py
#
# preInclude fragment for simulation of LLP that
# decay in the muon spectrometer.  This fragment
# will enable the standard truth strategies in 
# the spectrometer, adding the full showers to 
# the truth record.  Because the muon system is
# substantial in the forward region, it should
# NOT be used for any standard production.
#################################################

def muon_llp_modify_truth_strategies():
    ## Modifying truth strategies as requested by e/gamma group
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.listStrategies()
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Bremsstrahlung', 'MUON::MUON', 1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Conversion', 'MUON::MUON', 1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Decay', 'MUON::MUON', 1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Ionization', 'MUON::MUON', 1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('HadronicInteraction', 'MUON::MUON',1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Compton', 'MUON::MUON', 1)

    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Bremsstrahlung', 'MUONQ02::MUONQ02', 1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Conversion', 'MUONQ02::MUONQ02', 1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Decay', 'MUONQ02::MUONQ02', 1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Ionization', 'MUONQ02::MUONQ02', 1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('HadronicInteraction', 'MUONQ02::MUONQ02',1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('Compton', 'MUONQ02::MUONQ02', 1)

## Register the callback
simFlags.InitFunctions.add_function("postInit", muon_llp_modify_truth_strategies)
