
def rhadron_modify_truth_strategies():
    print "ERROR rhadron_modify_truth_strategies is obsolete"
    ## from G4AtlasApps import AtlasG4Eng
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'Muon::MuonSys',1)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'MUONQ02::MUONQ02',1)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'IDET::IDET',1)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'CALO::CALO',1)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'BeamPipe::BeamPipe',1)
    ## AtlasG4Eng.G4Eng._ctrl.mctruthMenu.listStrategies()


from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("postInit", rhadron_modify_truth_strategies)
