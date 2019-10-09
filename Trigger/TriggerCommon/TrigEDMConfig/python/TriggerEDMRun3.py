# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


# Definition of trigger EDM for the Run 3

# Concept of categories is kept similar to TriggerEDMRun2.py, categories are: 
#   Bjet, Bphys, Egamma, ID/Tracking (to be concluded if can be merged), 
#   Jet, L1, MET, MinBias, Muon, Steer, Tau, Calo

# Please note: 
#   Dynamic varialbes/Container slimming: All dyn vars are removed unless explicitly specified to be kept!
#   Please refer to ATR-20275 for discussion about policy/handling of dynamic variables

from AthenaCommon.Logging import logging
__log = logging.getLogger('TriggerEDMRun3Config')


def recordable( name ):
    """
    Verify that the name is in the list of recorded objects and conform to the name convention

    In Run 2 it was a delicate process to configure correctly what got recorded
    as it had to be set in the algorithm that produced it as well in the TriggerEDM.py in a consistent manner.

    For Run 3 every alg input/output key can be crosschecked against the list of objects to record which is defined here.
    I.e. in the configuration alg developer would do this:
    from TriggerEDM.TriggerEDMRun3 import recordable

    alg.outputKey = recordable("SomeKey")
    If the names are correct the outputKey is assigned with SomeKey, if there is a missmatch an exception is thrown.
    """

    if name in ["HLTNav_Summary", "L1DecoderSummary"] or "L1" in name or "RoI" in name:
        pass
    else: #negative filtering
        if not name.startswith( "HLT_" ):
            __log.error( "The collection name {0} does not start with HLT_".format( name ) )
        if "Aux" in name and not name[-1] != ".":
            __log.error( "The collection name {0} is Aux but the name does not end with the '.'".format( name ) )

    for entry in TriggerHLTListRun3:
        if entry[0].split( "#" )[1] == name:
            return name
    msg = "The collection name {0} is not declared to be stored by HLT. Add it to TriggerEDMRun3.py".format( name )
    __log.error("ERROR in recordable() - see following stack trace.")
    raise RuntimeError( msg )


# ------------------------------------------------------------
# Trigger EDM list for Run 3 with all containers that should
# be stored in the specified format as well as the category
# ------------------------------------------------------------
AllowedOutputFormats = ['BS', 'ESD', 'AODFULL', 'AODSLIM', 'AODVERYSLIM', 'AODBLSSLIM', 'AODLARGE', 'AODSMALL', ]

JetVarsToKeep = ['ActiveArea', 'ActiveArea4vec_eta', 'ActiveArea4vec_m', 'ActiveArea4vec_phi', 'ActiveArea4vec_pt', 'AlgorithmType', 
                 'DetectorEta', 'DetectorPhi', 'EMFrac', 'EnergyPerSampling', 'GhostTrack', 'HECFrac', 'InputType', 
                 'JetConstitScaleMomentum_eta', 'JetConstitScaleMomentum_m', 'JetConstitScaleMomentum_phi', 'JetConstitScaleMomentum_pt', 
                 'Jvt', 'JVFCorr', 'NumTrkPt500', 'NumTrkPt1000', 'SizeParameter', 'SumPtTrkPt500', 'SumPtTrkPt1000', 'TrackWidthPt1000',]
JetVars = '.'.join(JetVarsToKeep)

TriggerHLTListRun3 = [

    #framework/steering
    ('xAOD::TrigCompositeContainer#HLTNav_Summary',          'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#HLTNav_SummaryAux.',   'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1DecoderSummary',        'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1DecoderSummaryAux.', 'BS ESD AODFULL AODSLIM', 'Steer'),

    ('xAOD::TrigDecision#xTrigDecision' ,                    'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.',          'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigConfKeys#TrigConfKeys' ,                     'ESD AODFULL AODSLIM', 'Steer'),

    ('TrigRoiDescriptorCollection#EMRoIs',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#MURoIs',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#METRoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#JETRoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_RoiForTau',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_RoiForID2',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_RoiForID1',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_RoiForID',                   'BS ESD AODFULL AODSLIM',  'Steer'),

    ('xAOD::TrigCompositeContainer#L1EM',                    'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1EMAux.',             'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1MU',                    'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1MUAux.',             'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1MET',                   'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1METAux.',            'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1J',                     'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1JAux.',              'BS ESD AODFULL AODSLIM', 'Steer'),

    ('xAOD::TrigCompositeContainer#HLT_TrigCostContainer',   'BS ESD', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#HLT_TrigCostContainerAux.alg.store.view.thread.slot.roi.start.stop.', 'BS ESD', 'Steer'),

    # Run-2 L1 (temporary)
    ('xAOD::MuonRoIContainer#LVL1MuonRoIs' ,                 'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux.' ,          'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EmTauRoIContainer#LVL1EmTauRoIs' ,               'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EmTauRoIAuxContainer#LVL1EmTauRoIsAux.' ,        'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::JetRoIContainer#LVL1JetRoIs' ,                   'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::JetRoIAuxContainer#LVL1JetRoIsAux.' ,            'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::JetEtRoI#LVL1JetEtRoI' ,                         'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::JetEtRoIAuxInfo#LVL1JetEtRoIAux.' ,              'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EnergySumRoI#LVL1EnergySumRoI' ,                 'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),
    ('xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux.',       'ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'L1'),


    # Egamma
    ('xAOD::TrigEMClusterContainer#HLT_L2CaloEMClusters',           'BS ESD AODFULL', 'Egamma', 'inViews:EMCaloViews'), # last arg specifies in which view container the fragments are, look into the proprty of View maker alg for it
    ('xAOD::TrigEMClusterAuxContainer#HLT_L2CaloEMClustersAux.',    'BS ESD AODFULL', 'Egamma'),
    ('xAOD::TrigPhotonContainer#HLT_L2Photons',                     'BS ESD AODFULL', 'Egamma', 'inViews:L2PhotonRecoViews'),
    ('xAOD::TrigPhotonAuxContainer#HLT_L2PhotonsAux.',              'BS ESD AODFULL', 'Egamma'),
    ('xAOD::TrigElectronContainer#HLT_L2Electrons',                 'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews'),
    ('xAOD::TrigElectronAuxContainer#HLT_L2ElectronsAux.',          'BS ESD AODFULL', 'Egamma'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_Electron',        'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_ElectronAux.', 'BS ESD AODFULL', 'Egamma'),
    
    #these two corresponds to the output of the precisionCalo step
    ('xAOD::CaloClusterContainer#HLT_CaloEMClusters',               'BS ESD AODFULL', 'Egamma', 'inViews:precisionCaloViews'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_CaloEMClustersAux.',    'BS ESD AODFULL', 'Egamma'),

    # This varient needed by TrigUpgradeTest/egammaRinger.py
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClusters',             'BS ESD AODFULL', 'Egamma'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersAux.',  'BS ESD AODFULL', 'Egamma'),

    # Not sure we need these two... 
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersRoI',          'BS ESD AODFULL', 'Egamma', 'inViews:precisionCaloViews'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersRoIAux.nCells.', 'BS ESD AODFULL', 'Egamma'),

    # These are for precision photon and precision Electron Keeping same names as in Run2
    ('xAOD::ElectronContainer#HLT_egamma_Electrons',                'BS ESD AODFULL', 'Egamma', 'inViews:precisionElectronViews'),
    ('xAOD::ElectronTrigAuxContainer#HLT_egamma_ElectronsAux',      'BS ESD AODFULL', 'Egamma'),
    ('xAOD::ElectronContainer#HLT_egamma_Iso_Electrons',            'BS ESD AODFULL', 'Egamma', 'inViews:precisionElectronViews'),
    ('xAOD::ElectronTrigAuxContainer#HLT_egamma_Iso_ElectronsAux',  'BS ESD AODFULL', 'Egamma'),

    ('xAOD::PhotonContainer#HLT_egamma_Photons',                    'BS ESD AODFULL', 'Egamma', 'inViews:precisionPhotonViews'),
    ('xAOD::PhotonTrigAuxContainer#HLT_egamma_PhotonsAux.',         'BS ESD AODFULL', 'Egamma'),
    ('xAOD::PhotonContainer#HLT_egamma_Iso_Photons',                'BS ESD AODFULL', 'Egamma', 'inViews:precisionPhotonViews'),
    ('xAOD::PhotonTrigAuxContainer#HLT_egamma_Iso_PhotonsAux',      'BS ESD AODFULL', 'Egamma'),

    # Muon

    #Id track particles
    ('xAOD::TrackParticleContainer#HLT_xAODTracks_Muon',                 'BS ESD AODFULL', 'Muon', 'inViews:EMCombViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_MuonAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_MuonFS',                 'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_MuonFSAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_MuonIso',                 'BS ESD AODFULL', 'Muon', 'inViews:MUEFIsoViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_MuonIsoAux.',          'BS ESD AODFULL', 'Muon'),

    #bphys
    ('xAOD::TrigBphysContainer#TrigBphysDimu',                              'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),
    ('xAOD::TrigBphysAuxContainer#TrigBphysDimuAux.',                       'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),
    ('xAOD::TrigBphysContainer#TrigBphysEFDimu',                            'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),
    ('xAOD::TrigBphysAuxContainer#TrigBphysEFDimuAux.',                     'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),

    #xAOD muons (msonly (x2: roi+FS), combined (x3: FS+RoI (outside-in, inside-out+outside-in))
    ('xAOD::MuonContainer#HLT_Muons_RoI',                                       'BS ESD AODFULL', 'Muon', 'inViews:MUEFSAViewRoIs'),
    ('xAOD::MuonAuxContainer#HLT_Muons_RoIAux.',                                'BS ESD AODFULL', 'Muon'),

    ('xAOD::MuonContainer#HLT_Muons_FS',                                        'BS ESD AODFULL', 'Muon', 'inViews:MUFSViewRoI'),
    ('xAOD::MuonAuxContainer#HLT_Muons_FSAux.',                                 'BS ESD AODFULL', 'Muon'),

    ('xAOD::MuonContainer#HLT_MuonsCB_RoI',                                     'BS ESD AODFULL', 'Muon', 'inViews:MUEFCBViewRoIs'),
    ('xAOD::MuonAuxContainer#HLT_MuonsCB_RoIAux.',                              'BS ESD AODFULL', 'Muon'),

    ('xAOD::MuonContainer#HLT_MuonsCB_FS',                                      'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
    ('xAOD::MuonAuxContainer#HLT_MuonsCB_FSAux.',                               'BS ESD AODFULL', 'Muon'),

    ('xAOD::MuonContainer#HLT_MuonsCBOutsideIn',                                'BS ESD AODFULL', 'Muon', 'inViews:MUEFCBViewRoIs'),
    ('xAOD::MuonAuxContainer#HLT_MuonsCBOutsideInAux.',                         'BS ESD AODFULL', 'Muon'),

    ('TrigRoiDescriptorCollection#MuonCandidates_FS_ROIs',                      'BS ESD AODFULL', 'Muon'),

    #xAOD isolated muon
    ('xAOD::MuonContainer#HLT_MuonsIso',                                'BS ESD AODFULL', 'Muon', 'inViews:MUEFIsoViewRoIs'),
    ('xAOD::MuonAuxContainer#HLT_MuonsIsoAux.',                         'BS ESD AODFULL', 'Muon'),

    #Muon track particle containers (combined (x2: FS+RoI), extrapolated (x2: FS+RoI), MSonly (x1: FS))
    ('xAOD::TrackParticleContainer#HLT_CBCombinedMuon_RoITrackParticles',                     'BS ESD AODFULL', 'Muon', 'inViews:MUEFCBViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_CBCombinedMuon_RoITrackParticlesAux.',              'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_CBCombinedMuon_FSTrackParticles',                      'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_CBCombinedMuon_FSTrackParticlesAux.',               'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_MSExtrapolatedMuons_RoITrackParticles',                'BS ESD AODFULL', 'Muon', 'inViews:MUEFSAViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_MSExtrapolatedMuons_RoITrackParticlesAux.',         'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_MSExtrapolatedMuons_FSTrackParticles',                 'BS ESD AODFULL', 'Muon', 'inViews:MUFSViewRoI'),
    ('xAOD::TrackParticleAuxContainer#HLT_MSExtrapolatedMuons_FSTrackParticlesAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_MSOnlyExtrapolatedMuons_FSTrackParticles',             'BS ESD AODFULL', 'Muon', 'inViews:MUFSViewRoI'),
    ('xAOD::TrackParticleAuxContainer#HLT_MSOnlyExtrapolatedMuons_FSTrackParticlesAux.',      'BS ESD AODFULL', 'Muon'),

    #xAOD L2 muons (SA, CB, isolation)
    ('xAOD::L2StandAloneMuonContainer#HLT_MuonL2SAInfo',        'BS ESD AODFULL', 'Muon', 'inViews:MUViewRoIs'),
    ('xAOD::L2StandAloneMuonAuxContainer#HLT_MuonL2SAInfoAux.', 'BS ESD AODFULL', 'Muon'),

    ('xAOD::L2CombinedMuonContainer#HLT_MuonL2CBInfo',          'BS ESD AODFULL', 'Muon', 'inViews:EMCombViewRoIs'),
    ('xAOD::L2CombinedMuonAuxContainer#HLT_MuonL2CBInfoAux.',   'BS ESD AODFULL', 'Muon'),

    ('xAOD::L2IsoMuonContainer#HLT_MuonL2ISInfo',               'BS ESD AODFULL', 'Muon', 'inViews:MUIsoViewRoIs'),
    ('xAOD::L2IsoMuonAuxContainer#HLT_MuonL2ISInfoAux.',        'BS ESD AODFULL', 'Muon'),


    # Tau

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_TauCore',                 'BS ESD AODFULL', 'Tau', 'inViews:TCoreViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_TauCoreAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_TauIso',                 'BS ESD AODFULL', 'Tau', 'inViews:TIsoViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_TauIsoAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_Tau',                 'BS ESD AODFULL', 'Tau', 'inViews:TCoreViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_TauAux.',          'BS ESD AODFULL', 'Tau'),

    # Jet
    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjesIS',                      'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_subjesISAux.'+JetVars,       'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjes',                        'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_subjesAux.'+JetVars,         'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_nojcalib',                      'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_nojcalibAux.'+JetVars,       'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoJets_subjes',                       'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoJets_subjesAux.'+JetVars,        'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoJets_nojcalib',                     'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoJets_nojcalibAux.'+JetVars,      'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10JetRCJets_subjesIS',                      'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10JetRCJets_subjesISAux.'+JetVars,       'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_jes',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_jesAux.'+JetVars, 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    # MET
    ('xAOD::TrigMissingETContainer#HLT_MET_cell',                               'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_cellAux.',                        'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_mht',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_mhtAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tcPufit',                       'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tcPufitAux.',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tc',                            'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tcAux.',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersFS',                  'BS ESD', 'MET'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersFSAux.nCells.','BS ESD', 'MET'),

    # tau
    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMergedAux.',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_MVA',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_MVAAux.',              'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    # tau calo clusters
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersLC',                  'BS ESD AODFULL', 'Tau', 'inViews:TAUCaloRoIs'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersLCAux.nCells.','BS ESD AODFULL', 'Tau'),

    # tau tracks
    ('xAOD::TauTrackContainer#HLT_tautrack_MVA',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),
    ('xAOD::TauTrackAuxContainer#HLT_tautrack_MVAAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    #bjet
    ('xAOD::TrackParticleContainer#HLT_xAODTracks_FS',                     'BS ESD AODFULL', 'Bjet'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_FSAux.',              'BS ESD AODFULL', 'Bjet'),

    #bjet jets

    ('xAOD::JetContainer#HLT_SplitJet',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),
    ('xAOD::JetAuxContainer#HLT_SplitJetAux.',                       'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),

    ('xAOD::JetContainer#HLT_GSCJet',                             'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet', 'inViews:BJetViews'),
    ('xAOD::JetAuxContainer#HLT_GSCJetAux.',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),

    # vertex for b-jets
    ('xAOD::VertexContainer#HLT_EFHistoPrmVtx',                          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),
    ('xAOD::VertexAuxContainer#HLT_EFHistoPrmVtxAux.',                   'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),  

    # MinBias
    ('xAOD::TrackParticleContainer#HLT_xAODTracksMinBias',                 'BS ESD AODFULL', 'MinBias'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracksMinBiasAux.',          'BS ESD AODFULL', 'MinBias'),

    # ID
    # Requested by TrigUpgradeTest/IDCalo.py
    ('xAOD::TrackParticleContainer#HLT_xAODTracks',                        'BS ESD AODFULL', 'ID'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracksAux.',                 'BS ESD AODFULL', 'ID'),

    ('EventInfo#ByteStreamEventInfo',              'ESD', 'Misc'),
    ('ROIB::RoIBResult#*',                         'ESD', 'Misc'), 

    ('xAOD::TrigCompositeContainer#HLT_SpacePointCounts',            'BS ESD AODFULL AODSLIM', 'MinBias'),
    ('xAOD::TrigCompositeAuxContainer#HLT_SpacePointCountsAux.',     'BS ESD AODFULL AODSLIM', 'MinBias'),
]


#-------------------------------------------------------------------------------
# EDM details list to store the transient-persistent version
#-------------------------------------------------------------------------------
EDMDetailsRun3 = {}

# xAOD versions are auto-detected at serialisation, but T/P classes need to specify P version here
EDMDetailsRun3[ "TrigRoiDescriptorCollection" ]     = {'persistent':"TrigRoiDescriptorCollection_p3"}

EDMDetailsRun3[ "xAOD::TrigDecisionAuxInfo" ]         = {'parent':"xAOD::TrigDecision"}
EDMDetailsRun3[ "xAOD::EnergySumRoIAuxInfo" ]         = {'parent':"xAOD::EnergySumRoI"}
EDMDetailsRun3[ "xAOD::JetEtRoIAuxInfo" ]             = {'parent':"xAOD::JetEtRoI"}
EDMDetailsRun3[ "xAOD::CaloClusterTrigAuxContainer" ] = {'parent':"xAOD::CaloClusterContainer"}


#-------------------------------------------------------------------------------
# Helper functions 
#-------------------------------------------------------------------------------
def persistent( transient ):
    """
    Persistent EDM class, for xAOD it is the actual class version

    Uses list defined above. If absent assumes v1
    """
    if transient in EDMDetailsRun3:
        if 'persistent' in EDMDetailsRun3[transient]:
            return EDMDetailsRun3[transient]['persistent']
    return transient


def tpMap():
    """
    List
    """
    l = {}
    for tr in EDMDetailsRun3.keys():
        if "xAOD" in tr:
            continue
        l[tr] = persistent(tr)
    return l


