# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

def createJGTowerMaker( SuperCellContainer="SCell", ApplySCQual=True, SCBitMask=0x200, **kwargs ) :

    from TrigT1CaloFexSim.TrigT1CaloFexSimConf import JGTowerMaker

    towerMaker = JGTowerMaker( useSCQuality = ApplySCQual,
                               useAllCalo = False,
                               useAllRun2TT = False,
                               SuperCellType = SuperCellContainer,
                               SuperCellQuality = SCBitMask )

    for arg in kwargs:
        towerMaker.__setattr__(arg,kwargs[arg])
    
    from AthenaCommon.Logging import logging
    log = logging.getLogger("TrigT1CaloFexSimConfig")
    log.info("Enabling JGTowerMaker %s. It is setup to read SuperCellContainer %s and %sapply SCQuality" % 
             (towerMaker.name(), towerMaker.SuperCellType, "" if towerMaker.useSCQuality else "not " ) )
    
    return towerMaker


def createJGTowerReader( SuperCellType = "SCell", **kwargs ) :

    from TrigT1CaloFexSim.TrigT1CaloFexSimConf import JGTowerReader

    towerReader = JGTowerReader( outputNoise = False, 
                                 debugJetAlg = False,
                                 SuperCellType = SuperCellType, 
                                 noise_file = "Run3L1CaloSimulation/Noise/jTowergTowerNoise.20210209.r11881.root",
                                 plotSeeds = False,
                                 saveSeeds = True,
                                 dumpTowerInfo = False,

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
                                 jJetRound_seed_size = 0.31, ## seed square of side this. 0.3 for 3x3 towers, but floating point precision
                                 jJetRound_max_r = 0.26,     ## distance (in eta and phi, not a radius)
                                 ## within which other seeds will be counted in local max. 0.25 for 5x5 towers, 
                                 ## which is what we want, but floating point
                                 jJetRound_r=0.39,           ## jet radius. 0.4, but don't want == 0.4
                                 jJetRound_seed_tower_noise_multiplier = 2.0,
                                 jJetRound_seed_total_noise_multiplier = 0.0,
                                 jJetRound_seed_min_ET_MeV = 500,
                                 jJetRound_jet_tower_noise_multiplier = 2.0,
                                 jJetRound_jet_total_noise_multiplier = 0.0,
                                 jJetRound_jet_min_ET_MeV = 5000,
                                 
                                 makeRoundJetsPUsub=True,
                                 
                                 makeRoundLargeRJets = True,
                                 jJetRound_LargeR_seed_size = 0.31, # seed square of side this. 0.3 for 3x3 towers
                                 jJetRound_LargeR_max_r = 0.26,     # distance (in eta and phi, not a radius) within 
                                 # which other seeds will be counted in local max. 0.25 for 5x5 towers,
                                 jJetRound_LargeR_r=0.80,           # jet radius. 
                                 jJetRound_LargeR_seed_min_ET_MeV = 500,
                                 jJetRound_LargeR_jet_min_ET_MeV = 5000,
                                 
                                 makeJetsFromMap = False,
                                 towerMap = 'Run3L1CaloSimulation/JetMaps/jFexJet_towerMap_2019-02-12_notruncation.txt', 
                                 map_seed_tower_noise_multiplier = 2.0,
                                 map_seed_total_noise_multiplier = 0.0,
                                 map_seed_min_ET_MeV = 500,
                                 map_jet_tower_noise_multiplier = 2.0,
                                 map_jet_total_noise_multiplier = 0.0,
                                 map_jet_min_ET_MeV = 500,
                                 
                                 gJet_r=0.9,
                                 gJet_block_tower_noise_multiplier = 0., 
                                 gJet_block_min_ET_MeV = 0.,
                                 gJet_tower_noise_multiplier = 0.,
                                 gJet_jet_min_ET_MeV = 5000.,

                             )
    for arg in kwargs:
        towerReader.__setattr__(arg,kwargs[arg])

    from AthenaCommon.Logging import logging
    log = logging.getLogger("TrigT1CaloFexSimConfig")
    log.info("Enabling JGTowerReader %s. It is setup to create makeSquareJets: %s, makeRoundJets: %s, makeRoundLargeRJets: %s, makeJetsFromMap: %s" % 
             (towerReader.name(), towerReader.makeSquareJets, towerReader.makeRoundJets, towerReader.makeRoundLargeRJets, towerReader.makeJetsFromMap ) )

    return towerReader


def createSuperCellBCIDAlg(**kwargs):

    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from LArRecUtils.LArRecUtilsConf import LArFlatConditionSvc;
    svcMgr += LArFlatConditionSvc();
    svcMgr.ProxyProviderSvc.ProviderNames += ["LArFlatConditionSvc"];
    svcMgr.LArFlatConditionSvc.DoSuperCells = True;
    svcMgr.LArFlatConditionSvc.DoRegularCells = True;

    from CaloTools.CaloLumiBCIDSCToolDefault import CaloLumiBCIDSCToolDefault
    svcMgr.ToolSvc += CaloLumiBCIDSCToolDefault()

    # create the algorithm
    from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import SuperCellBCIDAlg
    bcidAlg = SuperCellBCIDAlg()
    for arg in kwargs:
        bcidAlg.__setattr__(arg,kwargs[arg])

    return bcidAlg

def createSuperCellBCIDEmAlg(**kwargs):
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from CaloTools.CaloLumiBCIDToolDefault import CaloLumiBCIDToolDefault
    theBCIDTool=CaloLumiBCIDToolDefault()
    svcMgr.ToolSvc += theBCIDTool

    from TrigL1CaloUpgrade.TrigL1CaloUpgradeConf import SuperCellBCIDAlg,SuperCellBCIDEmAlg
    bcidAlg = SuperCellBCIDEmAlg()
    for arg in kwargs:
        bcidAlg.__setattr__(arg,kwargs[arg])
    return bcidAlg 
