def enableJGTowerReader(debug = False):
    algseq = CfgMgr.AthSequencer("AthAlgSeq")

    algseq += CfgMgr.JGTowerReader (
        outputNoise = False,
        debugJetAlg = debug,
        noise_file = "Run3L1CaloSimulation/Noise/noise_r10684.root",
        plotSeeds = True,
        dumpTowerInfo = True,
        
        makeSquareJets = False,
        jJet_seed_size = 0.2,
        jJet_r = 0.4,
        jJet_max_r = 0.4,
        jJet_seed_tower_noise_multiplier = 2.0,
        jJet_seed_total_noise_multiplier = 0.0,
        jJet_seed_min_ET_MeV = 500,
        jJet_jet_tower_noise_multiplier = 2.0,
        jJet_jet_total_noise_multiplier = 0.0,
        jJet_jet_min_ET_MeV = 500,
        
        makeRoundJets = True,
        jJetRound_seed_size = 0.31, # seed square of side this. 0.3 for 3x3 towers, but floating point precision
        jJetRound_max_r = 0.26,     # distance (in eta and phi, not a radius) within which other seeds will be counted in local max. 0.25 for 5x5 towers, which is what we want, but floating point
        jJetRound_r=0.39,           # jet radius. 0.4, but don't want == 0.4
        jJetRound_seed_tower_noise_multiplier = 2.0,
        jJetRound_seed_total_noise_multiplier = 0.0,
        jJetRound_seed_min_ET_MeV = 500,
        jJetRound_jet_tower_noise_multiplier = 2.0,
        jJetRound_jet_total_noise_multiplier = 0.0,
        jJetRound_jet_min_ET_MeV = 5000,

        makeRoundLargeRJets = True,
        jJetRound_LargeR_seed_size = 0.31, # seed square of side this. 0.3 for 3x3 towers
        jJetRound_LargeR_max_r = 0.26,     # distance (in eta and phi, not a radius) within which other seeds will be counted in local max. 0.25 for 5x5 towers,
        jJetRound_LargeR_r=0.80,           # jet radius. 
        jJetRound_LargeR_seed_min_ET_MeV = 500,
        jJetRound_LargeR_jet_min_ET_MeV = 5000,
        
        makeJetsFromMap = True,
        towerMap = 'Run3L1CaloSimulation/JetMaps/jFexJet_towerMap_2019-02-15.txt', # once it's on cvmfs
        map_seed_tower_noise_multiplier = 2.0,
        map_seed_total_noise_multiplier = 0.0,
        map_seed_min_ET_MeV = 500,
        map_jet_tower_noise_multiplier = 2.0,
        map_jet_total_noise_multiplier = 0.0,
        map_jet_min_ET_MeV = 500,
        
        gJet_seed_size=0.4,
        gJet_max_r=1.0,
        gJet_r=1.0,
        gJet_seed_tower_noise_multiplier = 1.0,
        gJet_seed_total_noise_multiplier = 1.0,
        gJet_seed_min_ET_MeV = 500,
        gJet_jet_tower_noise_multiplier = 3.0,
        gJet_jet_total_noise_multiplier = 0.0,
        gJet_jet_min_ET_MeV = 10000.0,
        )

