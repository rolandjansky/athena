
#  Defines a long lived particle decay strategy strategy
#       (standard simulation uses level 1)
def add_LLP_truth_strategy():
    from G4AtlasApps import PyG4Atlas
    MCTruthMenu=PyG4Atlas.G4AtlasEngine.menu_MCTruth()
    LLPstrg=PyG4Atlas.MCTruthStrg('G4TruthStrategies', 'LLP','Atlas::Atlas',0)
    MCTruthMenu.add_McTruthStrategy(LLPstrg)

    # Overly paranoid addition of regions
    LLPstrg.add_Volumes('BeamPipe::BeamPipe', 1)
    LLPstrg.add_Volumes('IDET::IDET', 1)
    LLPstrg.add_Volumes('CALO::CALO', 1)
    LLPstrg.add_Volumes('Muon::MuonSys', 1)
    LLPstrg.add_Volumes('MUONQ02::MUONQ02', 1)
    
    LLPstrg.add_Volumes('Pixel::Pixel', 1)
    LLPstrg.add_Volumes('SCT::SCT', 1)
    LLPstrg.add_Volumes('TRT::TRT', 1)
    LLPstrg.add_Volumes('Tile::Tile', 1)

    ## Should not need to add additional regions - this works on volume hierarchy
    #LLPstrg.add_Volumes('HEC', 1)
    #LLPstrg.add_Volumes('EMB', 1)
    #LLPstrg.add_Volumes('EMEC', 1)
    #LLPstrg.add_Volumes('FCAL', 1)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitMCTruth", add_LLP_truth_strategy)