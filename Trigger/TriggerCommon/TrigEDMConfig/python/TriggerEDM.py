# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# ********************* WARNING **********************
#
# In order to preserve backward compatibility, please 
#                  !!!!DO NOT!!!
# modify an existing key, simply add the new
# one after the old one.
# If old one is not going to be needed leave the destionations tag list empty.
# (there are examples below)
# 
# This is to ensure we will be able to read datasets
# produced with older versions of the reconstruction program.
#
# ****************************************************

#type/key destination tag (this tag is configure in JO and selects the objects whcih go to ESD AOD (prefix)
# (note that for L2 order matters, there might be not enough space for this at the end of the list so add there low priority objects)

# In Trigger*List, 3rd field holds the category of the trigger container
# Current categories
#	Bjet
#	Bphys
#	Config
#	Cosmics
#	Egamma
#	ID
#	Jet
#	L1
#	MET
#	MinBias
#	Muon
#	Steer
#	Tau
#	Calo       <-- new
#	Tracking   <-- new
#	Combined   <-- new
#	Unknown    <-- should be temporary until you figure out what to classify it as
#	Other?...maybe inDet(ID?), Truth

TriggerL2List = [
    # rois    
    ('TrigRoiDescriptor#HLT',                                             'BS ESD',                 'Steer'),
    ('TrigRoiDescriptor#HLT_forID',                                       'BS ESD AODFULL AODSLIM', 'Muon'),
    ('TrigRoiDescriptor#HLT_forMS',                                       'BS ESD AODFULL AODSLIM', 'Muon'),
    ('TrigRoiDescriptor#HLT_initialRoI',                                  'BS ESD AODFULL AODSLIM', 'Steer'),
    ('TrigRoiDescriptor#HLT_secondaryRoI_L2',                             'BS ESD AODFULL AODSLIM', 'Steer'),
    ('TrigRoiDescriptor#HLT_T2TauFinal',                                  'BS ESD AODFULL AODSLIM', 'Tau'),
    ('TrigRoiDescriptor#HLT_TrigT2CaloEgamma',                            'BS ESD',                 'Egamma'),
    ('TrigRoiDescriptor#HLT_TrigT2CaloJet',                               'BS ESD',                 'Jet'),
    ('TrigRoiDescriptor#HLT_TrigT2CosmicJet',                             'BS ESD',                 'Cosmics'),
    ('TrigRoiDescriptor#HLT_TrigT2CaloTau',                               'BS ESD',                 'Tau'),
    ('TrigRoiDescriptor#HLT_TrigCaloRinger',                              '',                       'Egamma'),
    ('TrigRoiDescriptor#HLT_ReverseRoI',                                  '',                       'Steer'),        # maybe L1?
    ('TrigPassBits#HLT_passbits',                                         'BS ESD AODFULL AODSLIM', 'Steer'),
    ('TrigPassFlags#HLT_passflags',                                       'BS ESD AODFULL AODSLIM', 'Steer'),
    
    # particles
    ('MuonFeature#HLT',                                                   'BS ESD AODFULL AODSLIM', 'Muon'),
    ('IsoMuonFeature#HLT',                                                'BS ESD',                 'Muon'),
    ('TrigMuonClusterFeature#HLT_MuonCluster',                            'BS ESD AODFULL AODSLIM', 'Muon'),
    ('CombinedMuonFeature#HLT',                                           'BS ESD AODFULL AODSLIM', 'Muon'),
    ('TrigPhotonContainer#HLT_L2PhotonFex',                               'BS ESD AODFULL AODSLIM', 'Egamma'),
    ('TrigT2Jet#HLT_TrigT2CaloJet',                                       'BS ESD AODFULL AODSLIM', 'Jet'),
    ('TrigT2Jet#HLT_TrigT2CosmicJet',                                     'BS ESD AODFULL AODSLIM', 'Cosmics'), # ?slim
    ('TrigTau#HLT',                                                       'BS ESD',                 'Tau'),
    ('TileMuFeature#HLT',                                                 'BS ESD',                 'Muon'),
    ('TileTrackMuFeature#HLT',                                            'BS ESD',                 'Muon'),
    ('TrigElectronContainer#HLT_L2IDCaloFex',                             'BS ESD AODFULL AODSLIM', 'Egamma'),  # ? is this still in use?? 
    ('TrigElectronContainer#HLT_L2ElectronFex',                           'BS ESD AODFULL AODSLIM', 'Egamma'),
    ('TrigL2BjetContainer#HLT_L2BjetFex',                                 'BS ESD AODFULL AODSLIM', 'Bjet'),
    ('TrigT2MbtsBits#HLT_T2Mbts',                                         'BS ESD AODFULL AODSLIM', 'MinBias'),
    ('TrigSpacePointCounts#HLT_spacepoints',                              'BS ESD AODFULL AODSLIM', 'MinBias'), # ? do we realy need it!
    ('TrigTrtHitCounts#HLT_TrtHitCount',                                  'BS ESD AODFULL AODSLIM', 'Tracking'),        # category???
    ('TrigMissingET#HLT_T2MissingET',                                     'BS ESD AODFULL AODSLIM', 'MET'),
    ('TrigMissingET#HLT_L2JetEtSum',                                      'BS ESD AODFULL AODSLIM', 'MET'),
    ('TrigMissingET#HLT_L2MissingET_FEB',                                 'BS ESD AODFULL AODSLIM', 'MET'),
    ('ElectronMuonTopoInfo#HLT_EgMuTopoFEX',                              'BS ESD AODFULL AODSLIM', 'Combined'),        # Muon? Egamma? Combined?
    ('ElectronMuonTopoInfo#HLT_L2_PhotonMuonTopoFEX',                     'BS ESD AODFULL AODSLIM', 'Combined'),        # Muon? Egamma? Combined?

    #clusters
    ('TrigEMCluster#HLT',                                                 'BS ESD',                 'Egamma'),
    ('TrigEMCluster#HLT_TrigT2CaloEgamma',                                'BS ESD AODFULL',         'Egamma'),
    ('TrigEMCluster#HLT_TrigCaloRinger',                                  '',                       'Egamma'),
    ('TrigEMCluster#HLT_TrigT2CaloCosmic',                                'BS ESD AODFULL',         'Cosmics'),
    ('TrigEMCluster#HLT_T2CaloSwCluster',                                 'BS ESD AODFULL',         'Egamma'),
    ('TrigEMCluster#HLT_TrigcaloSwCluster',                               'BS ESD AODFULL',         'Egamma'),
    ('TrigCaloCluster#HLT_TrigT2Calo',                                    'BS ESD AODFULL',         'Egamma'),
    ('RingerRings#HLT_TrigT2CaloEgamma',                                  'BS ESD',                 'Egamma'),
    ('RingerRings#HLT_TrigCaloRinger',                                    '',                       'Egamma'),
    ('TrigRNNOutput#HLT_TrigRingerNeuralFex',                             'BS ESD AODFULL AODSLIM', 'Egamma'),
    ('TrigTauCluster#HLT_TrigT2CaloTau',                                  'BS ESD AODFULL AODSLIM', 'Tau'),
    ('TrigTauClusterDetails#HLT_TrigT2CaloTauDetails',                    'BS ESD AODFULL AODSLIM', 'Tau'),
    ('TrigTauTracksInfo#HLT',                                             'BS ESD',                 'Tau'),
    ('CosmicMuonCollection#HLT_CosmicMuons',                              'BS ESD AODFULL',         'Cosmics'),
    ('MdtTrackSegmentCollection#HLT_MDTsegments',                         'BS ESD AODFULL',         'Cosmics'),
    ('TrigRNNOutput#HLT_TrigTRTHTCounts',                                 'BS ESD AODFULL AODSLIM', 'Tracking'),

    # bphys
    ('TrigL2BphysContainer#HLT',                                          'BS ESD',                 'Bphys'),
    ('TrigL2BphysContainer#HLT_L2BMuMuFex',                               'BS ESD AODFULL AODSLIM', 'Bphys'),
    ('TrigL2BphysContainer#HLT_L2BMuMuXFex',                              'BS ESD AODFULL AODSLIM', 'Bphys'),
    ('TrigL2BphysContainer#HLT_L2DiMuXFex',                               'BS ESD AODFULL AODSLIM', 'Bphys'),
    ('TrigL2BphysContainer#HLT_L2DsPhiPiFexDs',                           'BS ESD AODFULL AODSLIM', 'Bphys'),
    ('TrigL2BphysContainer#HLT_L2DsPhiPiFexPhi',                          'BS ESD AODFULL AODSLIM', 'Bphys'),
    ('TrigL2BphysContainer#HLT_L2JpsieeFex',                              'BS ESD AODFULL AODSLIM', 'Bphys'),
    ('TrigL2BphysContainer#HLT_TrigDiMuon',                               'BS ESD AODFULL AODSLIM', 'Bphys'),
    ('TrigL2BphysContainer#HLT_L2TrackMass',                              'BS ESD AODFULL AODSLIM', 'Bphys'),
    ('TrigL2BphysContainer#HLT_L2MultiMuFex',                             'BS ESD AODFULL AODSLIM', 'Bphys'),

    # tracking
    ('TrigInDetTrackCollection#HLT',                                      'BS ESD',                  'ID'),      # category?
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_Jet',                       'BS ESD AODFULL',          'Bjet'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_Tau',                       'BS ESD AODFULL',          'Tau'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma',                    'BS ESD AODFULL',          'Egamma'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_Muon',                      'BS ESD AODFULL AODSLIM',  'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_muonIso',                   'BS ESD AODFULL AODSLIM',  'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_Bphysics',                  'BS ESD AODFULL',          'Bphys'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_FullScan',                  'BS ESD AODFULL',          'Bphys'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_Cosmics',                   'BS ESD AODFULL',          'Cosmics'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_eGamma_Brem',               'BS ESD AODFULL',          'Egamma'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_Tile',                      'BS ESD AODFULL',          'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma',                   'BS ESD AODFULL',          'Egamma'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Muon',                     'BS ESD AODFULL',          'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_muonIso',                  'BS ESD AODFULL',          'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Tau',                      'BS ESD AODFULL',          'Tau'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Jet',                      'BS ESD AODFULL',          'Bjet'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Bphysics',                 'BS ESD AODFULL',          'Bphys'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_FullScan',                 'BS ESD AODFULL',          'Bphys'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Cosmics',                  'BS ESD AODFULL',          'Cosmics'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Tile',                     'BS ESD AODFULL',          'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_eGamma_robust',            'BS ESD AODFULL',          'Egamma'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Muon_robust',              'BS ESD AODFULL',          'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_muonIso_robust',           'BS ESD AODFULL',          'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Tau_robust',               'BS ESD AODFULL',          'Tau'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Jet_robust',               'BS ESD AODFULL',          'Jet'),
    ('TrigInDetTrackCollection#HLT_TrigIDSCAN_Tau_IDOnly',                '',                        'Tau'),
    ('TrigInDetTrackCollection#HLT_TrigSiTrack_Tile_robust',              'BS ESD AODFULL',          'Muon'),
    ('TrigInDetTrackCollection#HLT_TRTSegmentFinder',                     'BS ESD AODFULL',          'ID'),      # category?
    ('TrigInDetTrackCollection#HLT_TRTxK',                                'BS ESD AODFULL',          'ID'),      # category?
    ('TrigVertexCollection#HLT_T2HistoPrmVtx',                            'BS ESD AODFULL AODSLIM',  'Bjet'),
    ('TrigVertexCollection#HLT_TrigBeamSpotVertex',                       'BS ESD AODFULL AODSLIM',  'Bjet'),
    ('TrigVertexCollection#HLT_TrigBeamSpotVertex_SiTrack',               'BS ESD AODFULL AODSLIM',  'Bjet'),
    ('TrigOperationalInfo#HLT_OPI_L2',                                    'BS',                      'Steer'),
    ('TrigMonConfig#HLT_OPI_L2_monitoring_config',                        'BS',                      'Steer'),
    ('TrigMonEvent#HLT_OPI_L2_monitoring_event',                          'BS',                      'Steer'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Muon' ,            'BS ESD AODFULL AODSLIM',  'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_eGamma' ,          'BS ESD AODFULL',          'Egamma'),
#    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_eGamma' ,          'BS ESD AODFULL',          'Egamma'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_muonIso' ,         'BS ESD AODFULL AODSLIM',  'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Tau' ,             'BS ESD AODFULL',          'Tau'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_TauCore' ,         'BS ESD AODFULL',          'Tau'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_TauIso' ,          'BS ESD AODFULL',          'Tau'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Jet' ,             'BS ESD AODFULL',          'Bjet'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Bphysics' ,        'BS ESD AODFULL',          'Bphys'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_FullScan' ,        'BS ESD AODFULL',          'Bphys'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder_Tile' ,            'BS ESD AODFULL',          'Muon'),
    ('TrigInDetTrackCollection#HLT_TrigL2SiTrackFinder' ,                 'BS ESD AODFULL',          'Tracking'),
    ('TrigInDetTrackCollection#HLT_TrigFastTrackFinder_TrigInDetTrack_Muon', 'BS ESD AODFULL',          'Muon'),

    # calocell
    ('CaloCellContainer#HLT_TrigT2CaloEgammaCells',                       'BS ESD AODFULL',          'Egamma'),
    ('CaloCellContainer#HLT_TrigT2CaloTauCells',                          'BS ESD',                  'Tau'),

    ('MuonFeatureDetails#HLT',                                            'BS ESD AODFULL AODSLIM',  'Muon')
    ]


TriggerEFList = [
    ('TrigRoiDescriptor#HLT_secondaryRoI_EF',                             'BS ESD AODFULL AODSLIM',                'Steer'),
    ('TrigRoiDescriptor#HLT_TrigJetRec',                                  'BS ESD',                                'Steer'),
    ('TrigPassBits#HLT_passbits',                                         'BS ESD AODFULL AODSLIM',                'Steer'),
    ('TrigPassFlags#HLT_passflags',                                       'BS ESD AODFULL AODSLIM',                'Steer'),
    ('TrigPassFlags#HLT_isEM',                                            'BS ESD AODFULL AODSLIM',                'Steer'),
    ('CaloClusterContainer#HLT',                                          '',                                       'Calo'),       # Egamma?
    ('CaloClusterContainer#HLT_TrigCaloClusterMaker',                     '',                                       'Calo'),       # Egamma?
    ('CaloClusterContainer#HLT_TrigCaloClusterMaker_slw',                 '',                                       'Calo'),       # Egamma?
    ('CaloClusterContainer#HLT_TrigCaloClusterMaker_topo',                '',                                       'Calo'),       # Egamma?
    ('CaloCellLinkContainer#HLT_TrigCaloClusterMaker_topo_Link',          'BS ESD',                                'Calo'),       # Egamma?
    ('TrigMuonEFContainer#HLT_MuonEF',                                    '',                                      'Muon'),
    ('TrigMuonEFInfoContainer#HLT_MuonEFInfo',                            'BS ESD AODFULL AODSLIM',                'Muon'),
    ('TrigMuonEFInfoContainer#HLT_eMuonEFInfo',                           'BS ESD AODFULL AODSLIM',                'Muon'),
    ('TrigMuonEFInfoContainer#HLT_MuTagIMO_EF',                           'BS ESD AODFULL AODSLIM',                'Muon'),
    ('TrigMuonEFIsolationContainer#HLT_MuonEFIsolation',                  'BS ESD AODFULL AODSLIM',                'Muon'),
    ('CaloShowerContainer#HLT',                                           'BS ESD',                                'Calo'),       # Egamma?
    ('egammaContainer#HLT_egamma_Electrons',                              'BS ESD AODFULL AODSLIM',                'Egamma'),
    ('egammaContainer#HLT_egamma',                                        'BS ESD AODFULL AODSLIM',                'Egamma'),
    ('egammaContainer#HLT_egamma_Photons',                                'BS ESD AODFULL AODSLIM',                'Egamma'),
    ('TrigMissingET#HLT_TrigEFMissingET',                                 'BS ESD AODFULL AODSLIM',                'MET'),
    ('TrigMissingET#HLT_TrigEFMissingET_noiseSupp',                       '',                                      'MET'),
    ('TrigMissingET#HLT_TrigEFMissingET_FEB',                             'BS ESD AODFULL AODSLIM',                'MET'),
    ('TrigMissingET#HLT_TrigEFMissingET_topocl',                          'BS ESD AODFULL AODSLIM',                'MET'),
    ('TrigMissingET#HLT_EFJetEtSum',                                      'BS ESD AODFULL AODSLIM',                'MET'),
    ('ElectronMuonTopoInfo#HLT_EF_PhotonMuonTopoFEX',                                   'BS ESD AODFULL AODSLIM',  'Combined'),       # Combined?
    ('ElectronMuonTopoInfo#HLT_EF_EgMuTopoFEX',                                         'BS ESD AODFULL AODSLIM',  'Combined'),       # Combined?
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bjet_EFID',             'BS ESD AODFULL',          'Bjet'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bphysics_EFID',         'BS ESD AODFULL',          'Bphys'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',    'BS ESD AODFULL',          'Cosmics'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_CosmicsN_EFID',         'BS ESD AODFULL',          'Cosmics'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Electron_EFID',         'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationCombined_Electron_EFID', 'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_FullScan_EFID',         'BS ESD AODFULL',          'Bphys'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_minBias_EFID',          'BS ESD',                  'MinBias'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Muon_EFID',             'BS ESD AODFULL',          'Muon'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Photon_EFID',           'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_EFID',              'BS ESD AODFULL',          'Tau'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_IDOnly_EFID',       '',                        'Tau'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_SiTrk_EFID',        '',                        'Tau'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',       'BS ESD AODFULL',          'Tau'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',  'BS ESD AODFULL',          'Egamma'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',      'BS ESD AODFULL',          'Muon'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',  'BS ESD AODFULL',          'Bphys'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_MuonIso_EFID',          'BS ESD AODFULL',          'Muon'),

    ('Analysis::TauDetailsContainer#HLT_TrigTauDetailsMerged',            'BS ESD AODFULL AODSLIM',  'Tau'),
    ('Analysis::TauDetailsContainer#HLT_TrigTauExtraDetailsMerged',       '',                        'Tau'),
    ('Analysis::TauDetailsContainer#HLT_TrigTauDetailsCalo',              'BS ESD AODFULL AODSLIM',  'Tau'),
    
    ('TrigTrackCounts#HLT',                                               '',                        'MinBias'),
    ('TrigTrackCounts#HLT_trackcounts',                                   'BS ESD AODFULL AODSLIM',  'MinBias'), #? this is the CMS trigger obj
    ('TrigVertexCounts#HLT_vertexcounts',                                 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('TrigEFBjetContainer#HLT_EFBjetFex',                                 'BS ESD AODFULL AODSLIM',  'Bjet'),
    ('TrigEFBphysContainer#HLT_EFBMuMuFex',                               'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('TrigEFBphysContainer#HLT_EFBMuMuXFex',                              'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('TrigEFBphysContainer#HLT_EFDsPhiPiFex',                             'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('TrigEFBphysContainer#HLT_EFMuPairs',                                'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('TrigEFBphysContainer#HLT_EFTrackMass',                              'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('TrigEFBphysContainer#HLT_EFMultiMuFex',                             'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('Analysis::TauJetContainer#HLT_TrigTauRecMerged',                    'BS ESD AODFULL AODSLIM',  'Tau'),
    ('Analysis::TauJetContainer#HLT_TrigTauRecCalo',                      'BS ESD AODFULL AODSLIM',  'Tau'),
    ('JetCollection#HLT',                                                 '',                        'Jet'),
    ('JetKeyDescriptor#HLT_TrigJetKeyMap',                                'BS',                      'Jet'),        #  # ESD AODFULL AODSLIM'),
    ('JetMomentMap#HLT_TrigJetRecMomentMap',                              'BS',                      'Jet'),        #  # ESD AODFULL AODSLIM'),
    ('JetCollection#HLT_TrigJetRec',                                      'BS ESD AODFULL AODSLIM',  'Jet'),
    ('JetCollection#HLT_TrigCosmicJetRec',                                'BS ESD AODFULL AODSLIM',  'Cosmics'),
    ('JetCollection#HLT_TrigTauJet',                                      'BS ESD',                  'Tau'),
    ('CaloTowerContainer#HLT_TrigCaloTowerMaker',                         '',                        'Calo'),       # Egamma?
    ('egDetailContainer#HLT_egamma_Electrons',                            'BS ESD',                  'Egamma'),
    ('egDetailContainer#HLT_egamma',                                      'BS ESD',                  'Egamma'),
    ('egDetailContainer#HLT_egamma_Photons',                              'BS ESD',                  'Egamma'),
    ('TrackCollection#TrigFastTrackFinder_Tau',                           'BS ESD AODFULL', 'Tau'), 
    ('TrackCollection#HLT_InDetTrigTrackSlimmer_Bjet_EFID',               'ESD',                     'Bjet'),
    ('TrackCollection#HLT_InDetTrigTrackSlimmer_Bphysics_EFID',           'ESD',                     'Bphys'),
    ('TrackCollection#HLT_InDetTrigTrackSlimmer_CosmicsN_EFID',           'ESD',                     'Cosmics'),
    ('TrackCollection#HLT_InDetTrigTrackSlimmer_Electron_EFID',           'ESD',                     'Egamma'),
    ('TrackCollection#HLT_InDetTrigTrackSlimmer_FullScan_EFID',           'ESD',                     'Bphys'),
    ('TrackCollection#HLT_InDetTrigTrackSlimmer_Muon_EFID',               'ESD',                     'Muon'),
    ('TrackCollection#HLT_InDetTrigTrackSlimmer_Photon_EFID',             'ESD',                     'Egamma'),
    ('TrackCollection#HLT_InDetTrigTrackSlimmer_Tau_EFID',                'ESD',                     'Tau'),
    ('TrigVertexCollection#HLT_EFHistoPrmVtx',                            'BS ESD AODFULL AODSLIM',  'Unknown'),        # category?
    ('VxContainer#HLT_PrimVx',                                            'BS ESD AODFULL AODSLIM',  'Bjet'),
    ('VxContainer#HLT_ConversionVx',                                      'ESD AODFULL AODSLIM',     'Bjet'),
    ('TrigOperationalInfo#HLT_OPI_EF',                                    'BS',                      'Steer'),
    ('TrigOperationalInfo#HLT_EXPRESS_OPI_EF',                            'BS',                      'Steer'),
    ('TrigMonConfig#HLT_OPI_EF_monitoring_config',                        'BS',                      'Steer'),
    ('TrigMonEvent#HLT_OPI_EF_monitoring_event',                          'BS',                      'Steer'),
    ('CaloCellContainer#HLT_TrigCaloCellMaker_eGamma',                    'BS ESD',                  'Calo'),
    ('CaloCellContainer#HLT_TrigCaloCellMaker_tau',                       'BS ESD',                  'Calo'),
    ('CaloCellContainer#HLT_TrigCaloCellLoF',                             'BS ESD',                  'Calo'),         # category?
    #

		# examples for the configuration of a collection that goes *only* into a data-scouting streams
		# but not into the full event (flag with 'DS')
    # ('TrackCollection#HLT_InDetTrigTracksIDCalib',                      'DS ESD AODFULL AODSLIM',  'EgammaDS'), 
    # ('TrigRoiDescriptor#HLT_secondaryRoI_EF',                            'DS ESD AODFULL AODSLIM',  'EgammaDS'),
    ]

TriggerHLTList = [
    ('TrigRoiDescriptor#HLT_secondaryRoI_HLT',                             'BS ESD AODFULL AODSLIM',                'Steer'),
    ('TrigOperationalInfo#HLT_OPI_HLT',                                    'BS',                      'Steer'),
    ('TrigOperationalInfo#HLT_EXPRESS_OPI_HLT',                            'BS',                      'Steer'),
    ('TrigMonConfig#HLT_OPI_HLT_monitoring_config',                        'BS',                      'Steer'),
    ('TrigMonEvent#HLT_OPI_HLT_monitoring_event',                          'BS',                      'Steer'), 

    #calo
    ('xAOD::CaloClusterContainer_v1#HLT_TrigCaloClusterMaker', 'BS ESD AODFULL AODSLIM', 'Calo'), 
    ('xAOD::CaloClusterAuxContainer_v1#HLT_TrigCaloClusterMakerAux.', 'BS ESD AODFULL AODSLIM', 'Calo'), 
    ('xAOD::CaloClusterContainer_v1#HLT_TrigCaloClusterMaker_slw', 'BS ESD AODFULL', 'Calo'), 
    ('xAOD::CaloClusterAuxContainer_v1#HLT_TrigCaloClusterMaker_slwAux.', 'BS ESD AODFULL', 'Calo'), 
    ('xAOD::CaloClusterContainer_v1#HLT_TrigCaloClusterMaker_topo', 'BS ESD', 'Calo'), 
    ('xAOD::CaloClusterAuxContainer_v1#HLT_TrigCaloClusterMaker_topoAux.','BS ESD', 'Calo'), 
    
    #egamma
    ('xAOD::ElectronContainer#HLT_egamma_Electrons', 'BS ESD AODFULL AODSLIM', 'Electron'),
    ('xAOD::ElectronAuxContainer#HLT_egamma_ElectronsAux.', 'BS ESD AODFULL AODSLIM', 'Electron'), 
    ('xAOD::ElectronContainer#HLT_egamma_electron', 'BS ESD AODFULL AODSLIM', 'Electron'),
    ('xAOD::ElectronAuxContainer#HLT_egamma_electronAux.', 'BS ESD AODFULL AODSLIM', 'Electron'), 
    ('xAOD::PhotonContainer#HLT_egamma_Photons', 'BS ESD AODFULL AODSLIM', 'Photon'),
    ('xAOD::PhotonAuxContainer#HLT_egamma_PhotonsAux.', 'BS ESD AODFULL AODSLIM', 'Photon'), 
    ('xAOD::PhotonContainer#HLT_egamma_photon', 'BS ESD AODFULL AODSLIM', 'Photon'),
    ('xAOD::PhotonAuxContainer#HLT_egamma_photonAux.', 'BS ESD AODFULL AODSLIM', 'Photon'), 

    ('xAOD::TrigElectronContainer_v1#HLT_L2ElectronFex', 'BS ESD AODFULL AODSLIM', 'Electron'),
    ('xAOD::TrigElectronAuxContainer_v1#HLT_L2ElectronFexAux.', 'BS ESD AODFULL AODSLIM', 'Electron'), 
    ('xAOD::TrigElectronContainer_v1#HLT_L2IDCaloFex', 'BS ESD AODFULL AODSLIM', 'Electron'),
    ('xAOD::TrigElectronAuxContainer_v1#HLT_L2IDCaloFexAux.', 'BS ESD AODFULL AODSLIM', 'Electron'), 
    ('xAOD::TrigPhotonContainer_v1#HLT_L2PhotonFex', 'BS ESD AODFULL AODSLIM', 'Photon'),
    ('xAOD::TrigPhotonAuxContainer_v1#HLT_L2PhotonFexAux.', 'BS ESD AODFULL AODSLIM', 'Photon'), 

    ('xAOD::TrigEMCluster_v1#HLT_TrigT2CaloEgamma', 'BS ESD AODFULL AODSLIM', 'Egamma'), 
    ('xAOD::TrigEMClusterAuxContainer_v1#HLT_TrigT2CaloEgammaAux.', 'BS ESD AODFULL AODSLIM', 'Egamma'), 
        
    #bphys
    ('xAOD::TrigBphysContainer_v1#HLT_EFBMuMuFex',                               'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer_v1#HLT_EFBMuMuFexAux.',                        'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysContainer_v1#HLT_EFBMuMuXFex',                              'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer_v1#HLT_EFBMuMuXFexAux.',                       'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysContainer_v1#HLT_EFDsPhiPiFex',                             'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer_v1#HLT_EFDsPhiPiFexAux.',                      'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysContainer_v1#HLT_EFMuPairs',                                'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer_v1#HLT_EFMuPairsAux.',                         'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysContainer_v1#HLT_EFTrackMass',                              'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer_v1#HLT_EFTrackMassAux.',                       'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysContainer_v1#HLT_EFMultiMuFex',                             'BS ESD AODFULL AODSLIM',  'Bphys'),
    ('xAOD::TrigBphysAuxContainer_v1#HLT_EFMultiMuFexAux.',                      'BS ESD AODFULL AODSLIM',  'Bphys'),
    
    #minbias
    # MinBias algs attach single objects therefore the pattern is slightly different, note also changes in the EDMList which define feature -> collection mapping
    ('xAOD::TrigSpacePointCounts_v1#HLT_spacepoints',			         'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigSpacePointCountsAuxContainer_v1#HLT_spacepointsAux.',		 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigT2MbtsBits_v1#HLT_T2Mbts',				         'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigT2MbtsBitsAuxContainer_v1#HLT_T2MbtsAux.',			 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigVertexCounts_v1#HLT_vertexcounts',			         'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigVertexCountsAuxContainer_v1#HLT_vertexcountsAux.',		 'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigTrackCounts_v1#HLT_trackcounts',			         'BS ESD AODFULL AODSLIM',  'MinBias'),
    ('xAOD::TrigTrackCountsAuxContainer_v1#HLT_trackcountsAux.',		 'BS ESD AODFULL AODSLIM',  'MinBias'),
    #taus
    ('xAOD::TauJetContainer_v1#HLT_TrigTauRecMerged', 'BS ESD AODFULL AODSLIM', 'Tau'), 
    ('xAOD::TauJetAuxContainer_v1#HLT_TrigTauRecMergedAux.', 'BS ESD AODFULL AODSLIM', 'Tau'), 
    ('xAOD::JetContainer_v1#HLT_TrigTauJet', 'BS ESD AODFULL AODSLIM', 'Tau'), 
    ('xAOD::JetTrigAuxContainer_v1#HLT_TrigTauJetAux.', 'BS ESD AODFULL AODSLIM', 'Tau'), 

    #muons
    #L2 Muons
    ('xAOD::L2StandAloneMuonContainer_v1#HLT_MuonL2SAInfo',                        'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::L2StandAloneMuonAuxContainer_v1#HLT_MuonL2SAInfoAux.',                 'BS ESD AODFULL AODSLIM',                'Muon'),

    #MuonEFInfo
    ('xAOD::MuonContainer_v1#HLT_MuonEFInfo',                                      'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::MuonAuxContainer_v1#HLT_MuonEFInfoAux.',                               'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleContainer_v1#HLT_MuonEFInfo_CombTrackParticles',          'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_MuonEFInfo_CombTrackParticlesAux.',   'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleContainer_v1#HLT_MuonEFInfo_ExtrapTrackParticles',        'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_MuonEFInfo_ExtrapTrackParticlesAux.', 'BS ESD AODFULL AODSLIM',                'Muon'),

    #eMuonEFInfo
    ('xAOD::MuonContainer_v1#HLT_eMuonEFInfo',                                     'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::MuonAuxContainer_v1#HLT_eMuonEFInfoAux.',                              'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleContainer_v1#HLT_eMuonEFInfo_CombTrackParticles',         'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_eMuonEFInfo_CombTrackParticlesAux.',  'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleContainer_v1#HLT_eMuonEFInfo_ExtrapTrackParticles',       'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_eMuonEFInfo_ExtrapTrackParticlesAux.','BS ESD AODFULL AODSLIM',                'Muon'),
    
    #MuTagIMO_EF
    ('xAOD::MuonContainer_v1#HLT_MuTagIMO_EF',                                     'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::MuonAuxContainer_v1#HLT_MuTagIMO_EFAux.',                              'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleContainer_v1#HLT_MuTagIMO_EF_CombTrackParticles',         'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_MuTagIMO_EF_CombTrackParticlesAux.',  'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleContainer_v1#HLT_MuTagIMO_EF_ExtrapTrackParticles',       'BS ESD AODFULL AODSLIM',                'Muon'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_MuTagIMO_EF_ExtrapTrackParticlesAux.','BS ESD AODFULL AODSLIM',                'Muon'),

    #MET
    ('xAOD::TrigMissingETContainer_v1#HLT_TrigEFMissingET',                                 'BS ESD AODFULL AODSLIM',                'MET'),
    ('xAOD::TrigMissingETAuxContainer_v1#HLT_TrigEFMissingETAux.',                          'BS ESD AODFULL AODSLIM',                'MET'),
    ('xAOD::TrigMissingETContainer_v1#HLT_TrigEFMissingET_noiseSupp',                       '',                                      'MET'),
    ('xAOD::TrigMissingETAuxContainer_v1#HLT_TrigEFMissingET_noiseSuppAux.',                '',                                      'MET'),
    ('xAOD::TrigMissingETContainer_v1#HLT_TrigEFMissingET_FEB',                             'BS ESD AODFULL AODSLIM',                'MET'),
    ('xAOD::TrigMissingETAuxContainer_v1#HLT_TrigEFMissingET_FEBAux.',                      'BS ESD AODFULL AODSLIM',                'MET'),
    ('xAOD::TrigMissingETContainer_v1#HLT_TrigEFMissingET_topocl',                          'BS ESD AODFULL AODSLIM',                'MET'),
    ('xAOD::TrigMissingETAuxContainer_v1#HLT_TrigEFMissingET_topoclAux.',                   'BS ESD AODFULL AODSLIM',                'MET'),
    ('xAOD::TrigMissingETContainer_v1#HLT_EFJetEtSum',                                      'BS ESD AODFULL AODSLIM',                'MET'),
    ('xAOD::TrigMissingETAuxContainer_v1#HLT_EFJetEtSumAux.',                               'BS ESD AODFULL AODSLIM',                'MET'),

    #tracking
    #
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Bjet_EFID',             'BS ESD AODFULL',          'Bjet'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Bphysics_EFID',         'BS ESD AODFULL',          'Bphys'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID',    'BS ESD AODFULL',          'Cosmics'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_CosmicsN_EFID',         'BS ESD AODFULL',          'Cosmics'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Electron_EFID',         'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnvCombined_Electron_EFID', 'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_FullScan_EFID',         'BS ESD AODFULL',          'Bphys'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_minBias_EFID',          'BS ESD',                  'MinBias'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Muon_EFID',             'BS ESD AODFULL',          'Muon'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Photon_EFID',           'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Tau_EFID',              'BS ESD AODFULL',          'Tau'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnvTRTOnly_Tau_EFID',       'BS ESD AODFULL',          'Tau'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnvTRTOnly_Electron_EFID',  'BS ESD AODFULL',          'Egamma'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnvTRTOnly_Muon_EFID',      'BS ESD AODFULL',          'Muon'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnvTRTOnly_FullScan_EFID',  'BS ESD AODFULL',          'Bphys'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_MuonIso_EFID',          'BS ESD AODFULL',          'Muon'),
    #
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Electron_FTF',         'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Electron_L2ID',        'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Muon_FTF',             'BS ESD AODFULL AODSLIM',  'Muon'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Tau_FTF',              'BS ESD AODFULL AODSLIM',  'Tau'),
    ('xAOD::TrackParticleContainer_v1#HLT_InDetTrigTrackingxAODCnv_Bjet_FTF',             'BS ESD AODFULL AODSLIM',  'Bjet'),

    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Bjet_EFIDAux.',             'BS ESD AODFULL',          'Bjet'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Bphysics_EFIDAux.',         'BS ESD AODFULL',          'Bphys'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFIDAux.',    'BS ESD AODFULL',          'Cosmics'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_CosmicsN_EFIDAux.',         'BS ESD AODFULL',          'Cosmics'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Electron_EFIDAux.',         'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnvCombined_Electron_EFIDAux.', 'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_FullScan_EFIDAux.',         'BS ESD AODFULL',          'Bphys'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_minBias_EFIDAux.',          'BS ESD',                  'MinBias'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Muon_EFIDAux.',             'BS ESD AODFULL',          'Muon'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Photon_EFIDAux.',           'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Tau_EFIDAux.',              'BS ESD AODFULL',          'Tau'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnvTRTOnly_Tau_EFIDAux.',       'BS ESD AODFULL',          'Tau'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnvTRTOnly_Electron_EFIDAux.',  'BS ESD AODFULL',          'Egamma'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnvTRTOnly_Muon_EFIDAux.',      'BS ESD AODFULL',          'Muon'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnvTRTOnly_FullScan_EFIDAux.',  'BS ESD AODFULL',          'Bphys'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_MuonIso_EFIDAux.',          'BS ESD AODFULL',          'Muon'),
    #
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Electron_FTFAux.',         'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Electron_L2IDAux.',        'BS ESD AODFULL AODSLIM',  'Egamma'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Muon_FTFAux.',             'BS ESD AODFULL AODSLIM',  'Muon'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Tau_FTFAux.',              'BS ESD AODFULL AODSLIM',  'Tau'),
    ('xAOD::TrackParticleAuxContainer_v1#HLT_InDetTrigTrackingxAODCnv_Bjet_FTFAux.',             'BS ESD AODFULL AODSLIM',  'Bjet'),

    #vertex
    ('xAOD::VertexContainer_v1#HLT_xPrimVx',                              'BS ESD AODFULL AODSLIM',  'Bjet'),
    ('xAOD::VertexAuxContainer_v1#HLT_xPrimVxAux.',                       'BS ESD AODFULL AODSLIM',  'Bjet'),

    #jets
    ('xAOD::JetContainer_v1#HLT_TrigHLTJetRec',					'BS ESD AODFULL AODSLIM',  'Jet'),
    ('xAOD::JetTrigAuxContainer_v1#HLT_TrigHLTJetRecAux.',			'BS ESD AODFULL AODSLIM',  'Jet'),
    ('xAOD::JetContainer_v1#HLT_TrigCosmicJetRec',				'BS ESD AODFULL AODSLIM',  'Jet'),
    ('xAOD::JetTrigAuxContainer_v1#HLT_TrigCosmicJetRecAux.',			'BS ESD AODFULL AODSLIM',  'Jet')
]

TriggerResultsList=[
    ('TrigDec::TrigDecision#TrigDecision' ,            'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigDecision_v1#xTrigDecision' ,           'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigDecisionAuxInfo_v1#xTrigDecisionAux.', 'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigConfKeys_v1#TrigConfKeys' ,            'ESD AODFULL AODSLIM', 'Steer'),
    ('HLT::HLTResult#HLTResult_L2',                    'ESD AODFULL AODSLIM', 'Steer'),
    ('HLT::HLTResult#HLTResult_EF' ,                   'ESD AODFULL AODSLIM', 'Steer'),
    ('HLT::HLTResult#HLTResult_HLT',                   'ESD AODFULL AODSLIM', 'Steer')
    ]


TriggerLvl1List=[
    ('xAOD::MuonRoIContainer_v1#LVL1MuonRoIs' ,          'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::MuonRoIAuxContainer_v1#LVL1MuonRoIsAux.' ,   'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::EmTauRoIContainer_v1#LVL1EmTauRoIs' ,        'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::EmTauRoIAuxContainer_v1#LVL1EmTauRoIsAux.' , 'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::JetRoIContainer_v1#LVL1JetRoIs' ,            'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::JetRoIAuxContainer_v1#LVL1JetRoIsAux.' ,     'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::JetEtRoI_v1#LVL1JetEtRoI' ,                  'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::JetEtRoIAuxInfo_v1#LVL1JetEtRoIAux.' ,       'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::EnergySumRoI_v1#LVL1EnergySumRoI' ,          'ESD AODFULL AODSLIM', 'L1'),
    ('xAOD::EnergySumRoIAuxInfo_v1#LVL1EnergySumRoIAux.','ESD AODFULL AODSLIM', 'L1'),
    ('DataVector<LVL1::TriggerTower>#TriggerTowers' ,    'ESD', 'L1'),
    ('DataVector<LVL1::TriggerTower>#TriggerTowersMuon' ,'ESD', 'L1'),
    ('DataVector<LVL1::JetElement>#JetElements' ,        'ESD', 'L1'),
    ('DataVector<LVL1::JetElement>#JetElementsOverlap' , 'ESD', 'L1'),
    ('DataVector<LVL1::CPMTower>#CPMTowers' ,            'ESD', 'L1'),
    ('DataVector<LVL1::CPMTower>#CPMTowersOverlap' ,     'ESD', 'L1'),
    ('DataVector<LVL1::CPMHits>#CPMHits' ,               'ESD', 'L1'),
    ('DataVector<LVL1::CPMRoI>#CPMRoIs' ,                'ESD', 'L1'),
    ('DataVector<LVL1::CMMCPHits>#CMMCPHits' ,           'ESD', 'L1'),
    ('DataVector<LVL1::CMMJetHits>#CMMJetHits' ,         'ESD', 'L1'),
    ('DataVector<LVL1::CMMEtSums>#CMMEtSums' ,           'ESD', 'L1'),
    ('DataVector<LVL1::JEMHits>#JEMHits' ,               'ESD', 'L1'),
    ('DataVector<LVL1::JEMRoI>#JEMRoIs' ,                'ESD', 'L1'),
    ('DataVector<LVL1::JEMEtSums>#JEMEtSums' ,           'ESD', 'L1'),
    ('DataVector<LVL1::RODHeader>#RODHeaders' ,          'ESD', 'L1'),
    ('LVL1::CMMRoI#CMMRoIs' ,                            'ESD', 'L1'),
    ('CTP_RDO#CTP_RDO' ,                                 'ESD', 'Steer'),
    ('MuCTPI_RDO#MUCTPI_RDO' ,                           'ESD', 'Unknown'),
    ('ROIB::RoIBResult#RoIBResult' ,                     'ESD', 'Steer')
    ]



TriggerIDTruth= [
    ('TrigInDetTrackTruthMap#TrigInDetTrackTruthMap', 'ESD AODFULL AODSLIM', 'Unknown')
    ]


# below go details of Trigger EDM

EDMDetails = {}

EDMDetails[ "TrigRoiDescriptor" ]         = {'persistent':"TrigRoiDescriptorCollection_p2",     'typealias':'Roi', 'collection':'TrigRoiDescriptorCollection' }
EDMDetails[ "TrigOperationalInfo" ]       = {'persistent':"TrigOperationalInfoCollection_tlp1",   'typealias':'', 'collection':'TrigOperationalInfoCollection' }    
EDMDetails[ "TrigMonConfig" ]             = {'persistent':"TrigMonConfigCollection_tlp1",         'typealias':'', 'collection':'TrigMonConfigCollection' }
EDMDetails[ "TrigMonEvent" ]              = {'persistent':"TrigMonEventCollection_tlp1",          'typealias':'', 'collection':'TrigMonEventCollection' }
EDMDetails[ "TrigInDetTrackCollection" ]  = {'persistent':"TrigInDetTrackCollection_tlp2",        'typealias':'InDetTrk' }
#EDMDetails[ "MuonFeature" ]               = {'persistent':"MuonFeatureContainer_tlp1",            'typealias':'Mu', 'collection':'MuonFeatureContainer'}
EDMDetails[ "MuonFeature" ]               = {'persistent':"MuonFeatureContainer_p3",            'typealias':'Mu', 'collection':'MuonFeatureContainer'}
#EDMDetails[ "MuonFeatureDetails" ]        = {'persistent':"MuonFeatureDetailsContainer_tlp1",     'typealias':'MuDetails', 'collection':'MuonFeatureDetailsContainer'}
EDMDetails[ "MuonFeatureDetails" ]        = {'persistent':"MuonFeatureDetailsContainer_p2",       'typealias':'MuDetails', 'collection':'MuonFeatureDetailsContainer'}
#EDMDetails[ "IsoMuonFeature" ]            = {'persistent':"IsoMuonFeatureContainer_tlp1",         'typealias':'IsoMu', 'collection':'IsoMuonFeatureContainer'}
#EDMDetails[ "IsoMuonFeature" ]            = {'persistent':"IsoMuonFeatureContainer_p2",           'typealias':'IsoMu', 'collection':'IsoMuonFeatureContainer'}
EDMDetails[ "IsoMuonFeature" ]            = {'persistent':"IsoMuonFeatureContainer_p3",           'typealias':'IsoMu', 'collection':'IsoMuonFeatureContainer'}
EDMDetails[ "TrigMuonClusterFeature" ]    = {'persistent':"TrigMuonClusterFeatureContainer_tlp1", 'typealias':'','collection':'TrigMuonClusterFeatureContainer'}
#EDMDetails[ "CombinedMuonFeature" ]       = {'persistent':"CombinedMuonFeatureContainer_tlp1",    'typealias':'CombMu', 'collection':'CombinedMuonFeatureContainer'}
#EDMDetails[ "CombinedMuonFeature" ]       = {'persistent':"CombinedMuonFeatureContainer_p2",      'typealias':'CombMu', 'collection':'CombinedMuonFeatureContainer'}
#EDMDetails[ "CombinedMuonFeature" ]       = {'persistent':"CombinedMuonFeatureContainer_p3",      'typealias':'CombMu', 'collection':'CombinedMuonFeatureContainer'}
EDMDetails[ "CombinedMuonFeature" ]       = {'persistent':"CombinedMuonFeatureContainer_p4",      'typealias':'CombMu', 'collection':'CombinedMuonFeatureContainer'}
#EDMDetails[ "TileMuFeature" ]             = {'persistent':"TileMuFeatureContainer_tlp1",          'typealias':'TileMu', 'collection':'TileMuFeatureContainer'}
EDMDetails[ "TileMuFeature" ]             = {'persistent':"TileMuFeatureContainer_p2",            'typealias':'TileMu', 'collection':'TileMuFeatureContainer'}
#EDMDetails[ "TileTrackMuFeature" ]        = {'persistent':"TileTrackMuFeatureContainer_tlp1",     'typealias':'TileTrackMu', 'collection':'TileTrackMuFeatureContainer'}
EDMDetails[ "TileTrackMuFeature" ]        = {'persistent':"TileTrackMuFeatureContainer_p3",       'typealias':'TileTrackMu', 'collection':'TileTrackMuFeatureContainer'}
#EDMDetails[ "TrigEMCluster" ]             = {'persistent':"TrigEMClusterContainer_tlp1",          'typealias':'EMCl', 'collection':"TrigEMClusterContainer"}
#EDMDetails[ "TrigEMCluster" ]             = {'persistent':"TrigEMClusterContainer_tlp2",          'typealias':'EMCl', 'collection':"TrigEMClusterContainer"}
#EDMDetails[ "TrigEMCluster" ]             = {'persistent':"TrigEMClusterContainer_p3",          'typealias':'EMCl', 'collection':"TrigEMClusterContainer"}
EDMDetails[ "TrigEMCluster" ]             = {'persistent':"TrigEMClusterContainer_p4",          'typealias':'EMCl', 'collection':"TrigEMClusterContainer"}
EDMDetails[ "TrigCaloCluster" ]            = {'persistent':"TrigCaloClusterContainer_p3",          'typealias':'CaloCl', 'collection':"TrigCaloClusterContainer"}
#EDMDetails[ "RingerRings" ]               = {'persistent':"RingerRingsContainer_tlp1",            'typealias':'RiRi', 'collection':"RingerRingsContainer"}
EDMDetails[ "RingerRings" ]               = {'persistent':"RingerRingsContainer_p2",            'typealias':'RiRi', 'collection':"RingerRingsContainer"}
#EDMDetails[ "TrigRNNOutput" ]             = {'persistent':"TrigRNNOutputContainer_tlp1",          'typealias':'RNNOut', 'collection':"TrigRNNOutputContainer"}
EDMDetails[ "TrigRNNOutput" ]             = {'persistent':"TrigRNNOutputContainer_p2",            'typealias':'RNNOut', 'collection':"TrigRNNOutputContainer"}
#EDMDetails[ "TrigTauCluster" ]            = {'persistent':"TrigTauClusterContainer_tlp1",         'typealias':'TauCl', 'collection': 'TrigTauClusterContainer' }
#EDMDetails[ "TrigTauCluster" ]            = {'persistent':"TrigTauClusterContainer_p3",         'typealias':'TauCl', 'collection': 'TrigTauClusterContainer' }
EDMDetails[ "TrigTauCluster" ]            = {'persistent':"TrigTauClusterContainer_p5",         'typealias':'TauCl', 'collection': 'TrigTauClusterContainer' }
#EDMDetails[ "TrigTauClusterDetails" ]     = {'persistent':"TrigTauClusterDetailsContainer_tlp1",  'typealias':'TauDetails', 'collection': 'TrigTauClusterDetailsContainer' }
EDMDetails[ "TrigTauClusterDetails" ]     = {'persistent':"TrigTauClusterDetailsContainer_p2",  'typealias':'TauDetails', 'collection': 'TrigTauClusterDetailsContainer' }
#EDMDetails[ "TrigTauTracksInfo" ]         = {'persistent':"TrigTauTracksInfoCollection_tlp1",     'typealias':'TauTrInfo', 'collection':'TrigTauTracksInfoCollection'}
EDMDetails[ "TrigTauTracksInfo" ]         = {'persistent':"TrigTauTracksInfoCollection_p2",     'typealias':'TauTrInfo', 'collection':'TrigTauTracksInfoCollection'}
#EDMDetails[ "TrigT2Jet" ]                 = {'persistent':"TrigT2JetContainer_tlp1",              'typealias':'T2Jet', 'collection':'TrigT2JetContainer'}
EDMDetails[ "TrigT2Jet" ]                 = {'persistent':"TrigT2JetContainer_p3",              'typealias':'T2Jet', 'collection':'TrigT2JetContainer'}
#EDMDetails[ "TrigElectronContainer" ]     = {'persistent':"TrigElectronContainer_tlp2",           'typealias':'' }
EDMDetails[ "TrigElectronContainer" ]     = {'persistent':"TrigElectronContainer_p3",           'typealias':'' }
#EDMDetails[ "TrigPhotonContainer" ]       = {'persistent':"TrigPhotonContainer_tlp2",             'typealias':'' }
EDMDetails[ "TrigPhotonContainer" ]       = {'persistent':"TrigPhotonContainer_p3",             'typealias':'' }
EDMDetails[ "TrigTau" ]                   = {'persistent':"TrigTauContainer_p3",                'typealias':'tau', 'collection':'TrigTauContainer'}
#EDMDetails[ "TrigL2BjetContainer" ]       = {'persistent':"TrigL2BjetContainer_tlp2",             'typealias':'' }
EDMDetails[ "TrigL2BjetContainer" ]       = {'persistent':"TrigL2BjetContainer_p3",             'typealias':'' }
#EDMDetails[ "TrigMissingET" ]             = {'persistent':"TrigMissingETContainer_tlp1",          'typealias':'MET', 'collection':'TrigMissingETContainer'}
EDMDetails[ "TrigMissingET" ]             = {'persistent':"TrigMissingETContainer_p3",          'typealias':'MET', 'collection':'TrigMissingETContainer'}
#EDMDetails[ "TrigT2MbtsBits" ]            = {'persistent':"TrigT2MbtsBitsContainer_tlp1",         'typealias':'', 'collection':'TrigT2MbtsBitsContainer'} 
EDMDetails[ "TrigT2MbtsBits" ]            = {'persistent':"TrigT2MbtsBitsContainer_p3",         'typealias':'', 'collection':'TrigT2MbtsBitsContainer'} 
#EDMDetails[ "TrigSpacePointCounts" ]      = {'persistent':"TrigSpacePointCountsCollection_tlp1",  'typealias':'', 'collection':'TrigSpacePointCountsCollection'}
EDMDetails[ "TrigSpacePointCounts" ]      = {'persistent':"TrigSpacePointCountsCollection_p4",  'typealias':'', 'collection':'TrigSpacePointCountsCollection'}
#EDMDetails[ "TrigTrtHitCounts" ]          = {'persistent':"TrigTrtHitCountsCollection_tlp1",      'typealias':'', 'collection':'TrigTrtHitCountsCollection'}
EDMDetails[ "TrigTrtHitCounts" ]          = {'persistent':"TrigTrtHitCountsCollection_p2",      'typealias':'', 'collection':'TrigTrtHitCountsCollection'}
EDMDetails[ "CosmicMuonCollection" ]      = {'persistent':"CosmicMuonCollection_tlp1",            'typealias':'', 'collection': 'CosmicMuonCollection'}
#EDMDetails[ "MdtTrackSegmentCollection" ] = {'persistent':"MdtTrackSegmentCollection_tlp1",       'typealias':'', 'collection': 'MdtTrackSegmentCollection'}
EDMDetails[ "MdtTrackSegmentCollection" ] = {'persistent':"MdtTrackSegmentCollection_p2",         'typealias':'', 'collection': 'MdtTrackSegmentCollection'}
EDMDetails[ "TrigVertexCollection" ]      = {'persistent':"TrigVertexCollection_tlp1",            'typealias':''}
EDMDetails[ "TrigL2BphysContainer" ]      = {'persistent':"TrigL2BphysContainer_tlp1",            'typealias':'L2Bphys', 'collection':'TrigL2BphysContainer'}
EDMDetails[ "TrigEFBphysContainer" ]      = {'persistent':"TrigEFBphysContainer_tlp1",            'typealias':'', 'collection':'TrigEFBphysContainer'}
EDMDetails[ "TrigEFBjetContainer" ]       = {'persistent':"TrigEFBjetContainer_tlp2",             'typealias':'' }



#EDMDetails[ "JetCollection" ]             = {'persistent':"JetCollection_tlp2",                   'typealias':'JetColl' }
EDMDetails[ "JetKeyDescriptor" ]          = {'persistent':"JetKeyDescriptorCollection_p1",        'typealias':'', 'collection':'JetKeyDescriptorCollection' }
EDMDetails[ "JetMomentMap" ]              = {'persistent':"JetMomentMapCollection_p6",            'typealias':'', 'collection':'JetMomentMapCollection' }
EDMDetails[ "JetCollection" ]             = {'persistent':"JetCollection_tlp6",                   'typealias':'JetColl' }
#EDMDetails[ "JetCollection" ]             = {'persistent':"JetCollection_tlp5",                   'typealias':'JetColl' }


EDMDetails[ "CaloClusterContainer" ]      = {'persistent':"CaloClusterContainer_p4",              'typealias':'CaloCl' }
EDMDetails[ "CaloCellLinkContainer" ]     = {'persistent':"CaloCellLinkContainer_p2",             'typealias':'CaloLk' }
EDMDetails[ "CaloShowerContainer" ]       = {'persistent':"CaloShowerContainer_p2",               'typealias':'CaloSh' }
#EDMDetails[ "CaloTowerContainer" ]        = {'persistent':"CaloTowerContainer_p1",                'typealias':'' }
#EDMDetails[ "TrigMuonEFContainer" ]       = {'persistent':"TrigMuonEFContainer_tlp1",             'typealias':'' }
EDMDetails[ "TrigMuonEFContainer" ]       = {'persistent':"TrigMuonEFContainer_p2",              'typealias':'' }
#EDMDetails[ "TrigMuonEFIsolationContainer" ]   = {'persistent':"TrigMuonEFIsolationContainer_p1",'typealias':'' }
EDMDetails[ "TrigMuonEFIsolationContainer" ]   = {'persistent':"TrigMuonEFIsolationContainer_p2",'typealias':'' }
EDMDetails[ "TrigMuonEFInfoContainer" ]   = {'persistent':"TrigMuonEFInfoContainer_tlp2",         'typealias':'' }


EDMDetails[ "egDetailContainer" ]         = {'persistent':"egDetailContainer_p2",                 'typealias':'egDet' }
EDMDetails[ "egammaContainer" ]           = {'persistent':"egammaContainer_p2",                   'typealias':'eg' }
#EDMDetails[ "Analysis::TauJetContainer" ] = {'persistent':"TauJetContainer_p3",                   'typealias':'' }
#EDMDetails[ "Analysis::TauJetContainer" ] = {'persistent':"TauJetContainer_p4",                   'typealias':'' }
EDMDetails[ "Analysis::TauJetContainer" ] = {'persistent':"TauJetContainer_p5",                   'typealias':'' }


EDMDetails[ "TrigTrackCounts" ]           = {'persistent':"TrigTrackCountsCollection_tlp1",       'typealias':'TrkCount', 'collection':'TrigTrackCountsCollection'}
EDMDetails[ "TrigVertexCounts" ]           = {'persistent':"TrigVertexCountsCollection_tlp1",     'typealias':'VtxCount', 'collection':'TrigVertexCountsCollection'}
EDMDetails[ "TrackCollection" ]           = { 'persistent':"Trk::TrackCollection_tlp2",           'typealias':'' }
#EDMDetails[ "Rec::TrackParticleContainer" ]   = {'persistent':"Rec::TrackParticleContainer_tlp1", 'typealias':'' }
EDMDetails[ "Rec::TrackParticleContainer" ]   = {'persistent':"Rec::TrackParticleContainer_tlp3", 'typealias':'' }

#EDMDetails[ "Analysis::TauDetailsContainer" ] ={'persistent':"TauDetailsContainer_tlp2",          'typealias':'' }
EDMDetails[ "Analysis::TauDetailsContainer" ] ={'persistent':"TauDetailsContainer_tlp4",          'typealias':'' }
#EDMDetails[ "VxContainer" ]               = {'persistent':"",                                     'typealias':'' }
EDMDetails[ "VxContainer" ]               = {'persistent':"Trk::VxContainer_tlp2",                'typealias':'' }
EDMDetails[ "CaloCellContainer" ]         = {'persistent':"CaloCompactCellContainer" }
EDMDetails[ "TrigPassBits" ]              = {'persistent':"TrigPassBitsCollection_p1",            'typealias':'', 'collection':'TrigPassBitsCollection' }
EDMDetails[ "TrigPassFlags" ]             = {'persistent':"TrigPassFlagsCollection_p1",           'typealias':'', 'collection':'TrigPassFlagsCollection' }
EDMDetails[ "ElectronMuonTopoInfo" ]      = {'persistent':"ElectronMuonTopoInfoContainer_p1",     'typealias':'', 'collection':"ElectronMuonTopoInfoContainer"}
EDMDetails[ "TrigComposite" ]             = {'persistent':"TrigCompositeContainer_p1",            'typealias':'Comp', 'collection':'TrigCompositeContainer'}
EDMDetails["TrigDec::TrigDecision"]               = {'persistent':"",               'typealias':'' }
EDMDetails["HLT::HLTResult"]                      = {'persistent':"",               'typealias':'' }
EDMDetails["HLT::HLTResult"]                      = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::TriggerTower>"]      = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::JetElement>"]        = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::CPMTower>"]          = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::CPMHits>"]           = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::CPMRoI>"]            = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::CMMCPHits>"]         = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::CMMJetHits>"]        = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::CMMEtSums>"]         = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::JEMHits>"]           = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::JEMRoI>"]            = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::JEMEtSums>"]         = {'persistent':"",               'typealias':'' }
EDMDetails["DataVector<LVL1::RODHeader>"]         = {'persistent':"",               'typealias':'' }
EDMDetails["LVL1::CMMRoI"]                        = {'persistent':"",               'typealias':'' }
EDMDetails["CTP_RDO"]                             = {'persistent':"",               'typealias':'' }
EDMDetails["MuCTPI_RDO"]                          = {'persistent':"",               'typealias':'' }
EDMDetails["ROIB::RoIBResult"]                    = {'persistent':"",               'typealias':'' }
EDMDetails["TrigInDetTrackTruthMap"]              = {'persistent':"",               'typealias':'' }


#xAOD details. 

EDMDetails["xAOD::TrigEMCluster_v1"]             = {'persistent':"", 'typealias':'', 'collection': 'xAOD::TrigEMClusterContainer_v1' } 
EDMDetails["xAOD::TrigEMClusterContainer_v1"]    = {'persistent':"", 'typealias':''} 
EDMDetails["xAOD::TrigEMClusterAuxContainer_v1"] = {'persistent':"", 'typealias':'', 'parent': 'xAOD::TrigEMClusterContainer_v1'} 


EDMDetails["xAOD::CaloClusterContainer_v1" ]      = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::CaloClusterAuxContainer_v1" ]   = {'persistent':"",              'typealias':'', 'parent': 'xAOD::CaloClusterContainer_v1'}
EDMDetails[ "xAOD::L2StandAloneMuonContainer_v1" ]     = {'persistent':"",         'typealias':'' }
EDMDetails[ "xAOD::L2StandAloneMuonAuxContainer_v1" ]  = {'persistent':"",         'typealias':'', 'parent': 'xAOD::L2StandAloneMuonContainer_v1'}
EDMDetails["xAOD::MuonContainer_v1" ]             = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::MuonAuxContainer_v1" ]          = {'persistent':"",              'typealias':'', 'parent': 'xAOD::MuonContainer_v1'}
EDMDetails["xAOD::TrackParticleContainer_v1" ]    = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::TrackParticleAuxContainer_v1" ] = {'persistent':"",              'typealias':'', 'parent': 'xAOD::TrackParticleContainer_v1'}
EDMDetails["xAOD::TauJetContainer_v1" ]           = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::TauJetAuxContainer_v1" ]        = {'persistent':"",              'typealias':'', 'parent': 'xAOD::TauJetContainer_v1'}
EDMDetails["xAOD::TrackParticleContainer_v1" ]    = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::TrackParticleAuxContainer_v1" ] = {'persistent':"",              'typealias':'', 'parent': 'xAOD::TrackParticleContainer_v1'}
EDMDetails["xAOD::VertexContainer_v1" ]           = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::VertexAuxContainer_v1" ]        = {'persistent':"",              'typealias':'', 'parent': 'xAOD::VertexContainer_v1'}
EDMDetails["xAOD::MuonRoIContainer_v1"]           = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::MuonRoIAuxContainer_v1"]        = {'persistent':"",              'typealias':'', 'parent': 'xAOD::MuonRoIContainer_v1'}
EDMDetails["xAOD::EmTauRoIContainer_v1"]          = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::EmTauRoIAuxContainer_v1"]       = {'persistent':"",              'typealias':'', 'parent': 'xAOD::EmTauRoIContainer_v1'}
EDMDetails["xAOD::JetRoIContainer_v1"]            = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::JetRoIAuxContainer_v1"]         = {'persistent':"",              'typealias':'', 'parent': 'xAOD::JetRoIAuxContainer_v1'}
EDMDetails["xAOD::JetEtRoI_v1"]                   = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::JetEtRoIAuxInfo_v1"]            = {'persistent':"",              'typealias':'', 'parent': 'xAOD::JetEtRoI_v1'}
EDMDetails["xAOD::EnergySumRoI_v1"]               = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::EnergySumRoIAuxInfo_v1"]        = {'persistent':"",              'typealias':'', 'parent': 'xAOD::EnergySumRoI_v1'}
EDMDetails["xAOD::ElectronContainer"]          = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::ElectronAuxContainer"]       = {'persistent':"",              'typealias':'', 'parent': 'xAOD::ElectronContainer'}
EDMDetails["xAOD::PhotonContainer"]            = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::PhotonAuxContainer"]         = {'persistent':"",              'typealias':'', 'parent': 'xAOD::PhotonContainer'}
EDMDetails["xAOD::TrigBphysContainer_v1" ]        = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::TrigBphysAuxContainer_v1" ]     = {'persistent':"",              'typealias':'', 'parent': 'xAOD::TrigBphysContainer_v1'}

EDMDetails["xAOD::TrigT2MbtsBits_v1" ]                       = {'persistent':"",   'typealias':'', 'collection':"xAOD::TrigT2MbtsBitsContainer_v1"}
EDMDetails["xAOD::TrigT2MbtsBitsAuxContainer_v1" ]           = {'persistent':"",   'typealias':'', 'parent': 'xAOD::TrigT2MbtsBitsContainer_v1'}
EDMDetails["xAOD::TrigSpacePointCounts_v1" ]                 = {'persistent':"",   'typealias':'', 'collection':"xAOD::TrigSpacePointCountsContainer_v1" }
EDMDetails["xAOD::TrigSpacePointCountsAuxContainer_v1" ]     = {'persistent':"",   'typealias':'', 'parent': 'xAOD::TrigSpacePointCountsContainer_v1'}
EDMDetails["xAOD::TrigVertexCounts_v1" ]                     = {'persistent':"",   'typealias':'', 'collection':"xAOD::TrigVertexCountsContainer_v1" }
EDMDetails["xAOD::TrigVertexCountsAuxContainer_v1" ]         = {'persistent':"",   'typealias':'', 'parent': 'xAOD::TrigVertexCountsContainer_v1'}
EDMDetails["xAOD::TrigTrackCounts_v1" ]                      = {'persistent':"",   'typealias':'', 'collection':"xAOD::TrigTrackCountsContainer_v1" }
EDMDetails["xAOD::TrigTrackCountsAuxContainer_v1" ]          = {'persistent':"",   'typealias':'', 'parent': 'xAOD::TrigTrackCountsContainer_v1'}

EDMDetails["xAOD::TrigMissingETContainer_v1"]     = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::TrigMissingETAuxContainer_v1"]  = {'persistent':"",              'typealias':'', 'parent': 'xAOD::TrigMissingETContainer_v1'}

EDMDetails["xAOD::TrigPhotonContainer_v1"]       = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::TrigPhotonAuxContainer_v1"]    = {'persistent':"",              'typealias':'', 'parent': 'xAOD::TrigPhotonContainer_v1'}
EDMDetails["xAOD::TrigElectronContainer_v1"]     = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::TrigElectronAuxContainer_v1"]  = {'persistent':"",              'typealias':'', 'parent': 'xAOD::TrigElectronContainer_v1'}


EDMDetails["xAOD::JetContainer_v1"]               = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::JetTrigAuxContainer_v1"]        = {'persistent':"",              'typealias':'', 'parent': 'xAOD::JetContainer_v1'}
EDMDetails["xAOD::TrigDecision_v1"]               = {'persistent':"",              'typealias':'' }
EDMDetails["xAOD::TrigDecisionAuxInfo_v1"]        = {'persistent':"",              'typealias':'', 'parent': 'xAOD::TrigDecision_v1'}
EDMDetails["xAOD::TrigConfKeys_v1"]               = {'persistent':"",              'typealias':'' }



#
# Rustem Ospanov: move list of EDM libraries from HLTTriggerGetter.py to TriggerEDM.py
#
EDMLibraries = [ 'TrigSteeringEvent', 'TrigMuonEvent',
                 'TrigBphysicsEvent', 'TrigCaloEvent',
                 'TrigCosmicEvent',   'TrigInDetEvent',
                 'TrigParticle',      'TrigMissingEtEvent',
                 'TrigDecisionEvent', 'TrigMonitoringEvent',
                 'TrigCaloEventTPCnv',      'TrigCosmicEventTPCnv',
                 'TrigDecisionEventTPCnv',  'TrigInDetEventTPCnv',
                 'TrigMissingEtEventTPCnv', 'TrigMuonEventTPCnv',
                 'TrigParticleTPCnv',       'TrigSteeringEventTPCnv',
                 'TrigTopoEvent' , 'TrigTopoEventTPCnv',
		 'TrigCombinedEvent', 'TrigCombinedEventTPCnv', 'TrigCombinedEventTPCnv1Dict',
		 'RecTPCnv', 'tauEventTPCnvDict',
                 'TrigCaloEventTPCnv1Dict',          'TrigCosmicEventTPCnv1Dict',
                 'TrigDecisionEventTPCnv1Dict',      'TrigInDetEventTPCnv1Dict',
                 'TrigMissingEtEventTPCnv1Dict',     'TrigMuonEventTPCnv1Dict',
                 'TrigParticleTPCnv1Dict',           'TrigSteeringEventTPCnv1Dict',
		 'RecTPCnvDict',
                 'OLD_TrigCosmicEventTPCnv1Dict',    'TrigMonitoringEventTPCnv',
                 'OLD_TrigDecisionEventTPCnv1Dict',  'TrigMonitoringEventTPCnvDict',
                 'OLD_TrigMuonEventTPCnv1Dict',      #'ARA_TrigMonitoringEventTPCnvDict',
                 'OLD_TrigParticleTPCnv1Dict',       'OLD_TrigSteeringEventTPCnv1Dict',
		 'OLD_RecTPCnvDict',
                 'xAODCaloEvent', 'xAODEgamma', 'xAODTracking', 'xAODMuon', 'xAODTau', 'xAODJet',
                 'xAODTrigBphys', 'xAODTrigMissingET', 'xAODTrigEgamma', 'xAODTrigMuon', 'xAODTrigCalo',
                 'xAODTrigMinBias', 'xAODBTagging',
                 #'ARA_TrigCaloEventTPCnv1Dict',      'ARA_TrigCosmicEventTPCnv1Dict',
                 #'ARA_TrigDecisionEventTPCnv1Dict',  #'ARA_TrigInDetEventTPCnv1Dict',
                 #'ARA_TrigMissingEtEventTPCnv1Dict', 'ARA_TrigMuonEventTPCnv1Dict',
                 #'ARA_TrigParticleTPCnv1Dict',
                 #'ARA_TrigSteeringEventTPCnv1Dict',
		 #'ARA_RecTPCnvDict', 'ARA_tauEventTPCnvDict'
         ]

def getCategory(s):
	""" From name of object in AOD/ESD found by checkFileTrigSize.py, return category """

	""" Clean up object name """
	s = s.strip()
    
    # To-do
    # seperate the first part of the string at the fisrt '_'
    # search in EDMDetails for the key corresponding to the persistent value
    # if a key is found, use this as the first part of the original string
    # put the string back together
    
	if s.count('::'): s = s[s.index(':')+2:]
	if s.count('<'):  s = s[s.index('<')+1:]
	if s.count('>'):  s = s[:s.index('>')]
	
	TriggerList = TriggerL2List + TriggerEFList + TriggerResultsList + TriggerLvl1List + TriggerIDTruth + TriggerHLTList
	
	category = ''
	bestMatch = ''
	
	""" Loop over all objects already defined in lists (and hopefully categorized!!) """
	for item in TriggerList:
		t,k = getTypeAndKey(item[0])
		
		""" Clean up type name """
		if t.count('::'): t = t[t.index(':')+2:]
		if t.count('<'):  t = t[t.index('<')+1:]
		if t.count('>'):  t = t[:t.index('>')]
		
		#print t, k
		
		if (s.startswith(t) and s.endswith(k)) and (len(t) > len(bestMatch)):
			bestMatch = t
			category = item[2]
	
	if category == '': return 'NOTFOUND'
	return category
	
	
def listObjects(category = '*'):
	
	TriggerList = TriggerL2List + TriggerEFList + TriggerResultsList + TriggerLvl1List + TriggerIDTruth + TriggerHLTList
	
	for item in TriggerList:
		if (item[2] == category): print item[2],'\t',item[0]
		elif (category == '*'):   print item[2],'\t',item[0]
	
	return
	

def getTypeAndKey(s):
    """ From the strings containing type and key of trigger EDM extract type and key
    """
    return s[:s.index('#')], s[s.index('#')+1:]

def keyToLabel(key):
    """ The key is usually HLT_blablah, this function returns second part of it or empty string
    """
    if '_' not in key:
        return ''
    else:
        return key[key.index('_'):].lstrip('_')


def getTriggerObjList(destination, lst):
    """
    Gives back the Python dictionary  with the content of ESD/AOD (dst) which can be inserted in OKS.
    """
    dset = set(destination.split())
    
    toadd = {}
    import itertools

    for item in itertools.chain(*lst):
        if item[1] == '':
            continue
        confset = set(item[1].split())
        if dset & confset: # intersection of the sets
            t,k = getTypeAndKey(item[0])
            colltype = t
            if EDMDetails[t].has_key('collection'):
                colltype = EDMDetails[t]['collection']
            if toadd.has_key(colltype):
                if k not in toadd[colltype]:
                    toadd[colltype] += [k]
            else:
                    toadd[colltype] = [k]
    return InsertContainerNameForHLT(toadd)

def getAllTriggerObjList():
    for l in TriggerL2List+TriggerEFList+TriggerResultsList+TriggerLvl1List+TriggerIDTruth+TriggerHLTList:
        yield l[0]


def getTrigIDTruthList(dst):
    """
    Gives back the Python dictionary  with the truth trigger content of ESD/AOD (dst) which can be inserted in OKS.
    """
    return getTriggerObjList(dst,[TriggerIDTruth])
    
def getLvl1ESDList():
    """
    Gives back the Python dictionary  with the lvl1 trigger result content of ESD which can be inserted in OKS.
    """
    return getTriggerObjList('ESD',[TriggerLvl1List])
 
def getLvl1AODList():
    """
    Gives back the Python dictionary  with the lvl1 trigger result content of AOD which can be inserted in OKS.
    """
    return getTriggerObjList('AODFULL',[TriggerLvl1List])
 
def getESDList(key='ESD'):
    """
    List (Literally Python dict) of trigger objects to be placed in ESD
    """
    return getTriggerObjList(key,[TriggerL2List,TriggerEFList,TriggerHLTList, TriggerResultsList])

def getAODList(tag='AODFULL'):
    """
    List (Literally Python dict) of trigger objects to be placed in AOD
    """
    return getTriggerObjList(tag, [TriggerL2List, TriggerEFList, TriggerHLTList, TriggerResultsList])

def getDSList(tag='DS'):
    """
    List (Literally Python dict) of trigger objects to be placed in DS
    """
    return getTriggerObjList(tag, [TriggerL2List,TriggerEFList,TriggerHLTList, TriggerResultsList])

def getL2PreregistrationList():
    """
    List (Literally Python list) of trigger objects to be preregistered i.e. this objects we want in every event for L2
    """
    l = []
    for item in TriggerL2List:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[0])
        if('Aux' in t):
            continue #we don't wat to preregister Aux containers
        l += [t+"#"+keyToLabel(k)]
    return l
    
def getEFPreregistrationList():
    """
    List (Literally Python list) of trigger objects to be preregistered i.e. this objects we want in every event for EF
    """
    l = []
    for item in TriggerEFList:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[0])
        if('Aux' in t):
            continue #we don't wat to preregister Aux containers
        l += [t+"#"+keyToLabel(k)]
    return l

def getHLTPreregistrationList():
    """
    List (Literally Python list) of trigger objects to be preregistered i.e. this objects we want in every event for merged L2/EF in addition to default L2 and EF
    """
    l = []
    for item in TriggerHLTList:
        if len (item[1]) == 0: continue
        t,k = getTypeAndKey(item[0])
        if('Aux' in t):
            continue #we don't wat to preregister Aux containers
        l += [t+"#"+keyToLabel(k)]
    return l


def getPreregistrationList():
    """
    List (Literally Python list) of trigger objects to be preregistered i.e. this objects we want for all levels
    """
    l = []
    l += getL2PreregistrationList()
    l += getEFPreregistrationList()
    l += getHLTPreregistrationList()
    return l


def getEFDSList():
    """
    List (Literally Python list) of trigger objects to be placed in RAW data. i.e. BS after EF
    """
    l = []
    for item in TriggerEFList:
        if 'DS' in item[1].split():
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]   
    return l

def getHLTDSList():
    """
    List (Literally Python list) of trigger objects to be placed in RAW data. i.e. BS after merged L2EF
    """
    l = []
    for item in TriggerHLTList:
        if 'DS' in item[1].split():
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]   
    return l

def getL2BSList():
    """
    List (Literally Python list) of trigger objects to be placed in RAW data. i.e. BS after L2
    """
    l = []
    for item in TriggerL2List:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]   
    return l

def getEFBSList():
    """
    List (Literally Python list) of trigger objects to be placed in RAW data. i.e. BS after EF
    """
    l = []
    for item in TriggerEFList:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]   
    return l

def getHLTBSList():
    """
    List (Literally Python list) of trigger objects to be placed in RAW data. i.e. BS after L2EF
    """
    l = []
    for item in TriggerHLTList:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            l += [t+"#"+keyToLabel(k)]   
    return l

def getL2BSTypeList():
    """ List of types to be placed in BS after L2
    """
    l = []
    for item in TriggerL2List:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            ctype = t
            if EDMDetails[t].has_key('collection'):
                ctype = EDMDetails[t]['collection']
            l += [ctype]
    return l

def getEFBSTypeList():
    """ List of types to be placed in BS after EF
    """
    l = []
    for item in TriggerEFList:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            ctype = t
            if EDMDetails[t].has_key('collection'):
                ctype = EDMDetails[t]['collection']
            l += [ctype]
    return l

def getHLTBSTypeList():
    """ List of types to be placed in BS after L2EF
    """
    l = []
    for item in TriggerHLTList:
        if 'BS' in item[1]:
            t,k = getTypeAndKey(item[0])
            ctype = t
            if EDMDetails[t].has_key('collection'):
                ctype = EDMDetails[t]['collection']
            l += [ctype]
    return l

def getEFDSTypeList():
    """ List of types to be placed in BS after EF
    """
    l = []
    for item in TriggerEFList:
        if 'DS' in item[1].split():
            t,k = getTypeAndKey(item[0])
            ctype = t
            if EDMDetails[t].has_key('collection'):
                ctype = EDMDetails[t]['collection']
            l += [ctype]
    return l

def getHLTDSTypeList():
    """ List of types to be placed in BS after L2EF
    """
    l = []
    for item in TriggerHLTList:
        if 'DS' in item[1].split():
            t,k = getTypeAndKey(item[0])
            ctype = t
            if EDMDetails[t].has_key('collection'):
                ctype = EDMDetails[t]['collection']
            l += [ctype]
    return l

def getTPList():
    """
    Mapping  of Transient objects to Peristent during serialization (BS creation)
    """
    l = {}
    bslist = getL2BSTypeList() + getEFBSTypeList()+getHLTBSTypeList()
    for t,d in EDMDetails.iteritems():                
        colltype = t
        if d.has_key('collection'):
            colltype = EDMDetails[t]['collection']
        if colltype in bslist: 
            l[colltype] = d['persistent']
    return l


def getARATypesRenaming():
    """
    Defines how to rename collecion keys in ARA when two types have the same key.
    i.e. TrigRoiDescriptorCollection#HLT
    and TrigTau#HLT
    After the remapping they will be named HLT_tau and HLT_roi so are distinct.
    """
    edm = set(TriggerL2List + TriggerEFList + TriggerHLTList)
    keys = [ getTypeAndKey(i[0])[1] for i in edm]
    # find repeating keys
    tmp = [ i for i in keys if keys.count(i) > 1 ]
    nonunique = {}
    for i in tmp:
        nonunique[i] = 1
    nonunique = nonunique.keys()

    # define remens for all object of which key appeared in nonunique
    renames = {}
    for entry in edm:
        t, key = getTypeAndKey(entry[0])
        if key in nonunique: # potential problem we have to do something
            
            if not EDMDetails[t].has_key('typealias') or EDMDetails[t]['typealias'] == '':
                print "ERROR types/keys will catch ", t, " ", key
                continue
            else:
                obj = t                    
                if EDMDetails[t].has_key('collection'):
                    obj = EDMDetails[t]['collection']

                # form the branch name
                bname = key+'_'+EDMDetails[t]['typealias']
                        
                #print 'type/key : ',key, '/', obj, ' to be aliased to: ', bname
                renames[(key, obj)] = bname
            
    return renames

def getEDMLibraries():
    return EDMLibraries

def InsertContainerNameForHLT(typedict):
    import re    
    output = {}
    for k,v in typedict.iteritems():
        newnames = []
        for el in v:
            if el.startswith('HLT_') or el == 'HLT':
                prefixAndLabel = el.split('_',1) #only split on first underscore
                containername = k if not 'Aux' in k else EDMDetails[k]['parent'] #we want the type in the Aux SG key to be the parent type #104811
                containername = re.sub('::','__',re.sub('_v[0-9]+$','',containername))
                newnames+=['_'.join([prefixAndLabel[0],containername]+([prefixAndLabel[1]] if len(prefixAndLabel) > 1 else []))]
            else:
                newnames+=[el]
            output[k] = newnames
    return output
