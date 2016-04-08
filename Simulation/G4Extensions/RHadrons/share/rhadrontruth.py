
def rhadron_modify_truth_strategies():
    from G4AtlasApps import AtlasG4Eng    
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'Muon::MuonSys',1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'MUONQ02::MUONQ02',1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'IDET::IDET',1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'CALO::CALO',1)
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.activateStrategy('LLP', 'BeamPipe::BeamPipe',1)
    #AtlasG4Eng.G4Eng._ctrl.mctruthMenu.disactivateStrategy('Decay') #to test that the LLP strategy gets it
    AtlasG4Eng.G4Eng._ctrl.mctruthMenu.listStrategies()

from G4AtlasApps.SimFlags import simFlags
#from G4AtlasApps import callbacks
#simFlags.InitFunctions.add_function("postInit", callbacks.add_LLP_truth_strategies)
simFlags.InitFunctions.add_function("postInit", rhadron_modify_truth_strategies)
