# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# ************************* WARNING ***************************
#
# Collections in this file are those used in the 3 level system
# used for the Run1 of the LHC. Please do not modify! 
#
# *************************************************************

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
    ('TrigT2ZdcSignals#HLT_zdcsignals',                                   'BS ESD AODFULL AODSLIM', 'MinBias'), #add by AZ
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
    ('TrigMissingET#HLT_TrigEFMissingET_topocl_PS',                       'BS ESD AODFULL AODSLIM',                'MET'),
    ('TrigMissingET#HLT_EFJetEtSum',                                      'BS ESD AODFULL AODSLIM',                'MET'),
    ('ElectronMuonTopoInfo#HLT_EF_PhotonMuonTopoFEX',                                   'BS ESD AODFULL AODSLIM',  'Combined'),       # Combined?
    ('ElectronMuonTopoInfo#HLT_EF_EgMuTopoFEX',                                         'BS ESD AODFULL AODSLIM',  'Combined'),       # Combined?
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bjet_EFID',             '',           'Bjet'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Bphysics_EFID',         '',           'Bphys'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationIOTRT_CosmicsN_EFID',    '',           'Cosmics'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_CosmicsN_EFID',         '',           'Cosmics'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Electron_EFID',         '',           'Egamma'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationCombined_Electron_EFID', '',           'Egamma'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_FullScan_EFID',         '',           'Bphys'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_minBias_EFID',          '',           'MinBias'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Muon_EFID',             '',           'Muon'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Photon_EFID',           '',           'Egamma'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_EFID',              '',           'Tau'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_IDOnly_EFID',       '',                        'Tau'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_Tau_SiTrk_EFID',        '',                        'Tau'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Tau_EFID',       '',          'Tau'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Electron_EFID',  '',          'Egamma'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_Muon_EFID',      '',          'Muon'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreationTRTOnly_FullScan_EFID',  '',          'Bphys'),
    ('Rec::TrackParticleContainer#HLT_InDetTrigParticleCreation_MuonIso_EFID',          '',          'Muon'),

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
    ('VxContainer#HLT_PrimVx',                                            '',                        'Bjet'),
    ('VxContainer#HLT_ConversionVx',                                      '',                        'Bjet'),
    ('TrigOperationalInfo#HLT_OPI_EF',                                    'BS',                      'Steer'),
    ('TrigOperationalInfo#HLT_EXPRESS_OPI_EF',                            'BS',                      'Steer'),
    ('TrigMonConfig#HLT_OPI_EF_monitoring_config',                        'BS',                      'Steer'),
    ('TrigMonEvent#HLT_OPI_EF_monitoring_event',                          'BS',                      'Steer'),
    ('CaloCellContainer#HLT_TrigCaloCellMaker_eGamma',                    'BS ESD',                  'Calo'),
    ('CaloCellContainer#HLT_TrigCaloCellMaker_tau',                       'BS ESD',                  'Calo'),
    ('CaloCellContainer#HLT_TrigCaloCellLoF',                             'BS ESD',                  'Calo')         # category?
    #

    # examples for the configuration of a collection that goes *only* into a data-scouting streams
    # but not into the full event (flag with 'DS')
    # ('TrackCollection#HLT_InDetTrigTracksIDCalib',                      'DS ESD AODFULL AODSLIM',  'EgammaDS'), 
    # ('TrigRoiDescriptor#HLT_secondaryRoI_EF',                            'DS ESD AODFULL AODSLIM',  'EgammaDS'),
    ]



TriggerResultsRun1List=[
    ('TrigDec::TrigDecision#TrigDecision' ,            'ESD AODFULL AODSLIM', 'Steer'),
    ('HLT::HLTResult#HLTResult_L2',                    'ESD AODFULL AODSLIM', 'Steer'),
    ('HLT::HLTResult#HLTResult_EF' ,                   'ESD AODFULL AODSLIM', 'Steer')
    ]
