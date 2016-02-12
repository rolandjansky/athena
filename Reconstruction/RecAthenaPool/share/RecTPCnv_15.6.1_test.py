infile = 'aod/AOD-15.6.1/AOD-15.6.1-full.pool.root'
keys = [
    #MissingET_p2
    'MET_RefEle',
    'MET_RefGamma',
    'MET_RefTau',
    'MET_MuonBoy_Spectro',
    'MET_MuonBoy_Track',
    'MET_MuonBoy',
    'MET_Cryo',
    'MET_CryoCone',
    'MET_LocHadTopoObj',
    'MET_Final',
    'MET_TowerObj',
    'AtlfastMissingEt',
    'MET_RefFinal',
    'MET_RefJet',
    'MET_TopoObj',
    'MET_TopoTowerObj',
    'MET_DM_All',
    'MET_DM_Crack2',
    'MET_DM_Cryo',
    'MET_CellOut',
    'MET_DM_Crack1',
    'MET_Muon',

    #MissingEtCalo_p2
    'MET_CorrTower',
    'MET_Base0',
    'MET_Base',
    'MET_Calib',
    'MET_LocHadTopo',
    'MET_Topo',
    'MET_CorrTopo',
    'MET_CorrTopoTower',

    #MissingEtTruth_p2
    'MET_Truth',
    'MET_Truth_PileUp',

    #MuonCaloEnergyContainer_tlp1
    'MuonCaloEnergyCollection',

    #MuonContainer_p5
    'MuGirlLowBetaCollection',
    'AtlfastIsoMuonCollection',
    'CaloMuonCollection',
    'AtlfastMuonCollection',
    'MuidMuonCollection',
    'StacoMuonCollection',

    #MuonSpShowerContainer_p1
    'MuonSpShowers',
         ]

include ('AthenaPoolUtilities/TPCnvTest.py')
