## autogenerated at [2011-01-15 15:59:23.306304]

## method filling an item-list with some data
def _fill( item ):
    item.add(
{'Analysis::TauDetailsContainer': ['HLT_TrigTauDetailsCalo',
                                   'HLT_TrigTauDetailsMerged',
                                   'TauRecDetailsContainer',
                                   'TauRecExtraDetailsContainer'],
 'Analysis::TauJetContainer': ['HLT_TrigTauRecCalo',
                               'HLT_TrigTauRecMerged',
                               'TauRecContainer'],
 'CTP_Decision': ['CTP_Decision'],
 'CTP_RDO': ['CTP_RDO'],
 'CaloCellContainer': ['AllCalo',
                       'HLT_TrigCaloCellMaker_eGamma',
                       'HLT_TrigCaloCellMaker_tau',
                       'HLT_TrigT2CaloEgammaCells',
                       'HLT_TrigT2CaloTauCells'],
 'CaloCellLinkContainer': ['CaloCalTopoCluster_Link',
                           'EMTopoSW35_Link',
                           'HLT_TrigCaloClusterMaker_topo_Link',
                           'LArClusterEM7_11Nocorr_Link',
                           'LArClusterEMFrwd_Link',
                           'LArClusterEMSofte_Link',
                           'LArClusterEM_Link',
                           'egClusterCollection_Link'],
 'CaloClusterContainer': ['CaloCalTopoCluster',
                          'EMTopoSW35',
                          'HLT',
                          'HLT_TrigCaloClusterMaker',
                          'HLT_TrigCaloClusterMaker_slw',
                          'HLT_TrigCaloClusterMaker_topo',
                          'LArClusterEM',
                          'LArClusterEM7_11Nocorr',
                          'LArClusterEMFrwd',
                          'LArClusterEMSofte',
                          'egClusterCollection'],
 'CaloShowerContainer': ['CaloCalTopoCluster_Data',
                         'EMTopoSW35_Data',
                         'HLT',
                         'LArClusterEM7_11Nocorr_Data',
                         'LArClusterEMSofte_Data',
                         'LArClusterEM_Data',
                         'egClusterCollection_Data'],
 'CaloTopoTowerContainer': ['TopoTower'],
 'CaloTowerContainer': ['CombinedTower'],
 'CombinedMuonFeatureContainer': ['HLT'],
 'CosmicMuonCollection': ['HLT_CosmicMuons'],
 'DataVector<INavigable4Momentum>': ['EMTopoSW35',
                                     'LArClusterEM',
                                     'LArClusterEM7_11Nocorr'],
 'DataVector<LVL1::CMMCPHits>': ['CMMCPHits'],
 'DataVector<LVL1::CMMEtSums>': ['CMMEtSums'],
 'DataVector<LVL1::CMMJetHits>': ['CMMJetHits'],
 'DataVector<LVL1::CPMHits>': ['CPMHits'],
 'DataVector<LVL1::CPMRoI>': ['CPMRoIs'],
 'DataVector<LVL1::CPMTower>': ['CPMTowers', 'CPMTowersOverlap'],
 'DataVector<LVL1::JEMEtSums>': ['JEMEtSums'],
 'DataVector<LVL1::JEMHits>': ['JEMHits'],
 'DataVector<LVL1::JEMRoI>': ['JEMRoIs'],
 'DataVector<LVL1::JetElement>': ['JetElements', 'JetElementsOverlap'],
 'DataVector<LVL1::RODHeader>': ['RODHeaders'],
 'DataVector<LVL1::TriggerTower>': ['TriggerTowers', 'TriggerTowersMuon'],
 'HLT::HLTResult': ['HLTResult_EF', 'HLTResult_L2'],
 'IsoMuonFeatureContainer': ['HLT'],
 'JetCollection': ['AntiKt4LCTopoJets',
                   'AntiKt4TopoEMJets',
                   'AntiKt4TopoJets',
                   'AntiKt4TowerJets',
                   'AntiKt4TruthJets',
                   'AntiKt4TruthPileupJets',
                   'AntiKt6LCTopoJets',
                   'AntiKt6TopoEMJets',
                   'AntiKt6TopoJets',
                   'AntiKt6TowerJets',
                   'AntiKt6TruthJets',
                   'HLT_TrigCosmicJetRec',
                   'HLT_TrigJetRec',
                   'HLT_TrigTauJet'],
 'JetKeyDescriptor': ['JetKeyMap'],
 'JetMomentMap': ['AntiKt4LCTopoJetsMomentMap',
                  'AntiKt4TopoEMJetsMomentMap',
                  'AntiKt4TopoJetsMomentMap',
                  'AntiKt4TowerJetsMomentMap',
                  'AntiKt4TruthJetsMomentMap',
                  'AntiKt6LCTopoJetsMomentMap',
                  'AntiKt6TopoEMJetsMomentMap',
                  'AntiKt6TopoJetsMomentMap',
                  'AntiKt6TowerJetsMomentMap',
                  'AntiKt6TruthJetsMomentMap'],
 'LArNoisyROSummary': ['LArNoisyROSummary'],
 'LVL1::CMMRoI': ['CMMRoIs'],
 'LVL1_ROI': ['LVL1_ROI'],
 'MdtTrackSegmentCollection': ['HLT_MDTsegments'],
 'MuCTPI_RDO': ['MUCTPI_RDO'],
 'MuonFeatureContainer': ['HLT'],
 'MuonFeatureDetailsContainer': ['HLT'],
 'ROIB::RoIBResult': ['RoIBResult'],
 'RawInfoSummaryForTag': ['RawInfoSummaryForTag'],
 'Rec::TrackParticleContainer': ['HLT_InDetTrigParticleCreationCombined_Electron_EFID',
                                 'HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',
                                 'HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',
                                 'HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',
                                 'HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',
                                 'HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',
                                 'HLT_InDetTrigParticleCreation_Bjet_EFID',
                                 'HLT_InDetTrigParticleCreation_Bphysics_EFID',
                                 'HLT_InDetTrigParticleCreation_CosmicsN_EFID',
                                 'HLT_InDetTrigParticleCreation_Electron_EFID',
                                 'HLT_InDetTrigParticleCreation_FullScan_EFID',
                                 'HLT_InDetTrigParticleCreation_Muon_EFID',
                                 'HLT_InDetTrigParticleCreation_Photon_EFID',
                                 'HLT_InDetTrigParticleCreation_Tau_EFID',
                                 'HLT_InDetTrigParticleCreation_minBias_EFID'],
 'RingerRingsContainer': ['HLT_TrigT2CaloEgamma'],
 'TileMuContainer': ['TileMuObj'],
 'TileMuFeatureContainer': ['HLT'],
 'TileTrackMuFeatureContainer': ['HLT'],
 'TrackCollection': ['HLT_InDetTrigTrackSlimmer_Bjet_EFID',
                     'HLT_InDetTrigTrackSlimmer_Bphysics_EFID',
                     'HLT_InDetTrigTrackSlimmer_CosmicsN_EFID',
                     'HLT_InDetTrigTrackSlimmer_Electron_EFID',
                     'HLT_InDetTrigTrackSlimmer_FullScan_EFID',
                     'HLT_InDetTrigTrackSlimmer_Muon_EFID',
                     'HLT_InDetTrigTrackSlimmer_Photon_EFID',
                     'HLT_InDetTrigTrackSlimmer_Tau_EFID'],
 'TrigDec::TrigDecision': ['TrigDecision'],
 'TrigEFBjetContainer': ['HLT_EFBjetFex'],
 'TrigEFBphysContainer': ['HLT_EFBMuMuFex',
                          'HLT_EFDsPhiPiFex',
                          'HLT_EFMuPairs',
                          'HLT_EFTrackMass'],
 'TrigEMClusterContainer': ['HLT',
                            'HLT_T2CaloSwCluster',
                            'HLT_TrigT2CaloCosmic',
                            'HLT_TrigT2CaloEgamma',
                            'HLT_TrigcaloSwCluster'],
 'TrigElectronContainer': ['HLT_L2ElectronFex', 'HLT_L2IDCaloFex'],
 'TrigInDetTrackCollection': ['HLT',
                              'HLT_TRTSegmentFinder',
                              'HLT_TRTxK',
                              'HLT_TrigIDSCAN_Bphysics',
                              'HLT_TrigIDSCAN_Cosmics',
                              'HLT_TrigIDSCAN_FullScan',
                              'HLT_TrigIDSCAN_Jet',
                              'HLT_TrigIDSCAN_Muon',
                              'HLT_TrigIDSCAN_Tau',
                              'HLT_TrigIDSCAN_Tile',
                              'HLT_TrigIDSCAN_eGamma',
                              'HLT_TrigIDSCAN_eGamma_Brem',
                              'HLT_TrigIDSCAN_muonIso',
                              'HLT_TrigSiTrack_Bphysics',
                              'HLT_TrigSiTrack_Cosmics',
                              'HLT_TrigSiTrack_FullScan',
                              'HLT_TrigSiTrack_Jet',
                              'HLT_TrigSiTrack_Jet_robust',
                              'HLT_TrigSiTrack_Muon',
                              'HLT_TrigSiTrack_Muon_robust',
                              'HLT_TrigSiTrack_Tau',
                              'HLT_TrigSiTrack_Tau_robust',
                              'HLT_TrigSiTrack_Tile',
                              'HLT_TrigSiTrack_Tile_robust',
                              'HLT_TrigSiTrack_eGamma',
                              'HLT_TrigSiTrack_eGamma_robust',
                              'HLT_TrigSiTrack_muonIso',
                              'HLT_TrigSiTrack_muonIso_robust'],
 'TrigInDetTrackTruthMap': ['TrigInDetTrackTruthMap'],
 'TrigL2BjetContainer': ['HLT_L2BjetFex'],
 'TrigL2BphysContainer': ['HLT',
                          'HLT_L2BMuMuFex',
                          'HLT_L2BMuMuXFex',
                          'HLT_L2DiMuXFex',
                          'HLT_L2DsPhiPiFexDs',
                          'HLT_L2DsPhiPiFexPhi',
                          'HLT_L2JpsieeFex',
                          'HLT_L2TrackMass',
                          'HLT_TrigDiMuon'],
 'TrigMissingETContainer': ['HLT_L2JetEtSum',
                            'HLT_T2MissingET',
                            'HLT_TrigEFMissingET',
                            'HLT_TrigEFMissingET_FEB',
                            'HLT_TrigEFMissingET_allCells'],
 'TrigMuonClusterFeatureContainer': ['HLT_MuonCluster'],
 'TrigMuonEFInfoContainer': ['HLT_MuTagIMO_EF',
                             'HLT_MuonEFInfo',
                             'HLT_eMuonEFInfo'],
 'TrigMuonEFIsolationContainer': ['HLT_MuonEFIsolation'],
 'TrigPassBitsCollection': ['HLT_passbits'],
 'TrigPhotonContainer': ['HLT_L2PhotonFex'],
 'TrigRNNOutputContainer': ['HLT_TrigRingerNeuralFex'],
 'TrigRoiDescriptorCollection': ['HLT',
                                 'HLT_T2TauFinal',
                                 'HLT_TrigT2CaloEgamma',
                                 'HLT_TrigT2CaloJet',
                                 'HLT_TrigT2CaloTau',
                                 'HLT_TrigT2CosmicJet',
                                 'HLT_forID',
                                 'HLT_forMS',
                                 'HLT_initialRoI',
                                 'HLT_secondaryRoI_EF',
                                 'HLT_secondaryRoI_L2'],
 'TrigSpacePointCountsCollection': ['HLT_spacepoints'],
 'TrigT2JetContainer': ['HLT_TrigT2CaloJet', 'HLT_TrigT2CosmicJet'],
 'TrigT2MbtsBitsContainer': ['HLT_T2Mbts'],
 'TrigTauClusterContainer': ['HLT_TrigT2CaloTau'],
 'TrigTauClusterDetailsContainer': ['HLT_TrigT2CaloTauDetails'],
 'TrigTauContainer': ['HLT'],
 'TrigTauTracksInfoCollection': ['HLT'],
 'TrigTrackCountsCollection': ['HLT_trackcounts'],
 'TrigTrtHitCountsCollection': ['HLT_TrtHitCount'],
 'TrigVertexCollection': ['HLT_T2HistoPrmVtx',
                          'HLT_TrigBeamSpotVertex',
                          'HLT_TrigBeamSpotVertex_SiTrack'],
 'TrigVertexCountsCollection': ['HLT_vertexcounts'],
 'TruthParticleContainer': ['INav4MomTruthEvent'],
 'VxContainer': ['AllPhotonsVxCandidates', 'HLT_ConversionVx', 'HLT_PrimVx'],
 'egDetailContainer': ['HLT_egamma',
                       'HLT_egamma_Electrons',
                       'HLT_egamma_Photons',
                       'SofteDetailContainer',
                       'egDetailAOD',
                       'egDetailContainer'],
 'egammaContainer': ['ElectronAODCollection',
                     'ElectronCollection',
                     'HLT_egamma',
                     'HLT_egamma_Electrons',
                     'HLT_egamma_Photons',
                     'PhotonAODCollection',
                     'PhotonCollection',
                     'egammaForwardCollection',
                     'softeCollection']}
    )
    return

## EOF ##