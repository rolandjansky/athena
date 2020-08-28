# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


# Definition of trigger EDM for the Run 3

# Concept of categories is kept similar to TriggerEDMRun2.py, categories are:
#   Bjet, Bphys, Egamma, ID/Tracking (to be concluded if can be merged),
#   Jet, L1, MET, MinBias, Muon, Steer, Tau, Calo

# Please note:
#   Dynamic varialbes/Container slimming: All dyn vars are removed unless explicitly specified to be kept!
#   Please refer to ATR-20275 for discussion about policy/handling of dynamic variables

from AthenaCommon.Logging import logging
__log = logging.getLogger('TriggerEDMRun3Config')

from TrigEDMConfig import DataScoutingInfo

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

    if "HLTNav_" in name:
        __log.error( "Don't call recordable({0}), or add any \"HLTNav_\" collection manually to the EDM. See:collectDecisionObjects.".format( name ) )
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
AllowedOutputFormats.extend(DataScoutingInfo.getAllDataScoutingIdentifiers())


JetVarsToKeep = ['ActiveArea', 'ActiveArea4vec_eta', 'ActiveArea4vec_m', 'ActiveArea4vec_phi', 'ActiveArea4vec_pt', 'AlgorithmType',
                 'DetectorEta', 'DetectorPhi', 'EMFrac', 'EnergyPerSampling', 'GhostTrack', 'HECFrac', 'InputType',
                 'JetConstitScaleMomentum_eta', 'JetConstitScaleMomentum_m', 'JetConstitScaleMomentum_phi', 'JetConstitScaleMomentum_pt',
                 'JetPileupScaleMomentum_eta', 'JetPileupScaleMomentum_m', 'JetPileupScaleMomentum_phi', 'JetPileupScaleMomentum_pt',
                 'JetEtaJESScaleMomentum_eta', 'JetEtaJESScaleMomentum_m', 'JetEtaJESScaleMomentum_phi', 'JetEtaJESScaleMomentum_pt',
                 'JetGSCScaleMomentum_eta', 'JetGSCScaleMomentum_m', 'JetGSCScaleMomentum_phi', 'JetGSCScaleMomentum_pt',
                 'Jvt', 'JVFCorr', 'NumTrkPt500', 'NumTrkPt1000', 'SizeParameter', 'SumPtTrkPt500', 'SumPtTrkPt1000', 'TrackWidthPt1000',]
JetVars = '.'.join(JetVarsToKeep)

BTagOutput = ['jetLink','BTagTrackToJetAssociator','minimumTrackRelativeEta','maximumTrackRelativeEta','averageTrackRelativeEta','trkSum_ntrk','trkSum_SPt','trkSum_VPt','trkSum_VEta','Muons',]
BTagOutput_IP2D = ['IP2D_TrackParticleLinks','IP2D_gradeOfTracks','IP2D_weightBofTracks','IP2D_weightCofTracks','IP2D_weightUofTracks','IP2D_sigD0wrtPVofTracks','IP2D_flagFromV0ofTracks','IP2D_nTrks','IP2D_isDefaults','IP2D_cu','IP2D_bu','IP2D_bc',]
BTagOutput_IP3D = ['IP3D_TrackParticleLinks','IP3D_gradeOfTracks','IP3D_weightBofTracks','IP3D_weightCofTracks','IP3D_weightUofTracks','IP3D_valD0wrtPVofTracks','IP3D_sigD0wrtPVofTracks','IP3D_valZ0wrtPVofTracks','IP3D_sigZ0wrtPVofTracks','IP3D_flagFromV0ofTracks','IP3D_nTrks','IP3D_isDefaults','IP3D_cu','IP3D_bu','IP3D_bc',]
BTagOutput_SV1 = ['SV1_TrackParticleLinks','SV1_vertices','SV1_isDefaults','SV1_NGTinSvx','SV1_masssvx','SV1_N2Tpair','SV1_efracsvx','SV1_deltaR','SV1_Lxy','SV1_L3d','SV1_significance3d','SV1_energyTrkInJet','SV1_dstToMatLay','SV1_badTracksIP','SV1_normdist',]
BTagOutput_JetFitter = ['JetFitter_deltaeta','JetFitter_deltaphi','JetFitter_fittedPosition','JetFitter_JFvertices','JetFitter_nVTX','JetFitter_nSingleTracks','JetFitter_isDefaults','JetFitter_deltaR','JetFitterSecondaryVertex_isDefaults','JetFitterSecondaryVertex_nTracks','JetFitterSecondaryVertex_mass','JetFitterSecondaryVertex_energy','JetFitterSecondaryVertex_energyFraction','JetFitterSecondaryVertex_displacement3d','JetFitterSecondaryVertex_displacement2d','JetFitterSecondaryVertex_minimumTrackRelativeEta','JetFitterSecondaryVertex_maximumTrackRelativeEta','JetFitterSecondaryVertex_averageTrackRelativeEta','JetFitter_mass','JetFitter_energyFraction','JetFitter_significance3d','JetFitter_nTracksAtVtx','JetFitter_N2Tpair','JetFitter_fittedCov','JetFitter_tracksAtPVchi2','JetFitter_tracksAtPVndf','JetFitter_tracksAtPVlinks','JetFitter_massUncorr','JetFitter_chi2','JetFitter_ndof','JetFitter_dRFlightDir',]
BTagOutput_rnnip = ['rnnip_isDefaults','rnnip_pu','rnnip_pc','rnnip_pb','rnnip_ptau',]
BTagOutput_highLevelTaggers = ['MV2c10_discriminant','DL1_pu','DL1_pc','DL1_pb','DL1r_pu','DL1r_pc','DL1r_pb',]

BTagOutput += BTagOutput_IP2D + BTagOutput_IP3D + BTagOutput_SV1 + BTagOutput_JetFitter + BTagOutput_rnnip + BTagOutput_highLevelTaggers
BTagVars = '.'.join(BTagOutput)


TriggerHLTListRun3 = [

    # framework/steering
    ('xAOD::TrigDecision#xTrigDecision' ,                    'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.',          'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigConfKeys#TrigConfKeys' ,                     'ESD AODFULL AODSLIM', 'Steer'),

    ('TrigRoiDescriptorCollection#HLT_EMRoIs',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_MURoIs',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_METRoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_JETRoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_TAURoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_FSRoI',                    'BS ESD AODFULL AODSLIM',  'Steer'),

    ('xAOD::TrigCompositeContainer#HLT_TrigCostContainer',   'CostMonDS ESD', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#HLT_TrigCostContainerAux.alg.store.view.thread.slot.roi.start.stop', 'CostMonDS ESD', 'Steer'),

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
    ('xAOD::TrigEMClusterContainer#HLT_FastCaloEMClusters',           'BS ESD AODFULL', 'Egamma', 'inViews:EMCaloViews'), # last arg specifies in which view container the fragments are, look into the proprty of View maker alg for it
    ('xAOD::TrigEMClusterAuxContainer#HLT_FastCaloEMClustersAux.',    'BS ESD AODFULL', 'Egamma'),
    ('xAOD::TrigRingerRingsContainer#HLT_FastCaloRinger',             'BS ESD AODFULL', 'Egamma', 'inViews:EMCaloViews'), #Ringer
    ('xAOD::TrigRingerRingsAuxContainer#HLT_FastCaloRingerAux.',      'BS ESD AODFULL', 'Egamma'), #Ringer

    ('xAOD::TrigPhotonContainer#HLT_FastPhotons',                     'BS ESD AODFULL', 'Egamma', 'inViews:EMPhotonViews'),
    ('xAOD::TrigPhotonAuxContainer#HLT_FastPhotonsAux.',              'BS ESD AODFULL', 'Egamma'),
    ('xAOD::TrigElectronContainer#HLT_FastElectrons',                 'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews'),
    ('xAOD::TrigElectronAuxContainer#HLT_FastElectronsAux.',          'BS ESD AODFULL', 'Egamma'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Electron_FTF',        'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Electron_FTFAux.', 'BS ESD AODFULL', 'Egamma'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Electron_IDTrig',        'BS ESD AODFULL', 'Egamma', 'inViews:precisionElectronViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Electron_IDTrigAux.', 'BS ESD AODFULL', 'Egamma'),


    # these two corresponds to the output of the precisionCalo step
    ('xAOD::CaloClusterContainer#HLT_CaloEMClusters',               'BS ESD AODFULL', 'Egamma', 'inViews:precisionCaloViews'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_CaloEMClustersAux.',    'BS ESD AODFULL', 'Egamma'),

    # This variant needed by TrigUpgradeTest/egammaRinger.py
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClusters',             'BS ESD AODFULL', 'Egamma'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersAux.',  'BS ESD AODFULL', 'Egamma'),
    #
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersLCFS',                               'BS ESD AODFULL', 'Jet', 'alias:CaloClusterContainerShallowCopy'), # special argument indicating that this collection has a different Aux
    ('xAOD::ShallowAuxContainer#HLT_TopoCaloClustersLCFSAux.calE.calEta.calPhi',         'BS ESD AODFULL', 'Jet'),

    # fullscan Jet Roi
    ('TrigRoiDescriptorCollection#HLT_FSJETRoI',                   'BS ESD AODFULL', 'Jet'),

    # Not sure we need these two...
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersRoI',          'BS ESD AODFULL', 'Egamma', 'inViews:precisionCaloViews'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersRoIAux.nCells', 'BS ESD AODFULL', 'Egamma'),

    # These are for precision photon and precision Electron Keeping same names as in Run2
    ('xAOD::ElectronContainer#HLT_egamma_Electrons',                'BS ESD AODFULL', 'Egamma', 'inViews:precisionElectronViews'),
    ('xAOD::ElectronAuxContainer#HLT_egamma_ElectronsAux.',     'BS ESD AODFULL', 'Egamma'),
    # enable when iso chains are present
    # ('xAOD::ElectronContainer#HLT_egamma_Iso_Electrons',            'BS ESD AODFULL', 'Egamma', 'inViews:precisionIsoElectronViews'),
    # ('xAOD::ElectronTrigAuxContainer#HLT_egamma_Iso_ElectronsAux.', 'BS ESD AODFULL', 'Egamma'),

    ('xAOD::PhotonContainer#HLT_egamma_Photons',                    'BS ESD AODFULL', 'Egamma', 'inViews:precisionPhotonViews'),
    ('xAOD::PhotonAuxContainer#HLT_egamma_PhotonsAux.',         'BS ESD AODFULL', 'Egamma'),
    # enable when iso chains are present
    # ('xAOD::PhotonContainer#HLT_egamma_Iso_Photons',                'BS ESD AODFULL', 'Egamma', 'inViews:precisionIsoPhotonViews'),
    # ('xAOD::PhotonTrigAuxContainer#HLT_egamma_Iso_PhotonsAux.',     'BS ESD AODFULL', 'Egamma'),

    # Muon

    # Id track particles
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Muon_FTF',                 'BS ESD AODFULL', 'Muon', 'inViews:MUCombViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Muon_FTFAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Muon_IDTrig',                 'BS ESD AODFULL AODSLIM', 'Muon', 'inViews:MUEFCBViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Muon_IDTrigAux.',          'BS ESD AODFULL AODSLIM', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonFS_FTF',                 'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonFS_FTFAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonFS_IDTrig',              'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonFS_IDTrigAux.',       'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonIso_FTF',                 'BS ESD AODFULL', 'Muon', 'inViews:MUEFIsoViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonIso_FTFAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonIso_IDTrig',              'BS ESD AODFULL', 'Muon', 'inViews:MUEFIsoViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonIso_IDTrigAux.',       'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonLate_FTF',                 'BS ESD AODFULL', 'Muon', 'inViews:MUEFLATEViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonLate_FTFAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MuonLate_IDTrig',              'BS ESD AODFULL', 'Muon', 'inViews:MUEFLATEViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MuonLate_IDTrigAux.',       'BS ESD AODFULL', 'Muon'),


    # bphys
    ('xAOD::TrigBphysContainer#HLT_DimuEF',                                 'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),
    ('xAOD::TrigBphysAuxContainer#HLT_DimuEFAux.',                          'BS ESD AODFULL AODSLIM AODVERYSLIM AODBLSSLIM', 'Bphys'),

    # xAOD muons (msonly (x2: roi+FS), combined (x3: FS+RoI (outside-in, inside-out+outside-in))
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

    # xAOD isolated muon
    ('xAOD::MuonContainer#HLT_MuonsIso',                                'BS ESD AODFULL', 'Muon', 'inViews:MUEFIsoViewRoIs'),
    ('xAOD::MuonAuxContainer#HLT_MuonsIsoAux.',                         'BS ESD AODFULL', 'Muon'),

    # Muon track particle containers (combined (x2: FS+RoI), extrapolated (x2: FS+RoI), MSonly (x1: FS))
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

    ('xAOD::L2CombinedMuonContainer#HLT_MuonL2CBInfo',          'BS ESD AODFULL', 'Muon', 'inViews:MUCombViewRoIs'),
    ('xAOD::L2CombinedMuonAuxContainer#HLT_MuonL2CBInfoAux.',   'BS ESD AODFULL', 'Muon'),

    ('xAOD::L2IsoMuonContainer#HLT_MuonL2ISInfo',               'BS ESD AODFULL', 'Muon', 'inViews:MUIsoViewRoIs'),
    ('xAOD::L2IsoMuonAuxContainer#HLT_MuonL2ISInfoAux.',        'BS ESD AODFULL', 'Muon'),

    ('TrigRoiDescriptorCollection#HLT_Roi_L2SAMuon',                   'BS ESD AODFULL', 'Muon'),
    ('TrigRoiDescriptorCollection#HLT_Roi_L2SAMuonForEF',              'BS ESD AODFULL', 'Muon'),
    ('TrigRoiDescriptorCollection#HLT_Roi_MuonIso',                    'BS ESD AODFULL', 'Muon'),

    # Tau

    ('xAOD::TrackParticleContainer#HLT_IDTrack_TauCore_FTF',                 'BS ESD AODFULL', 'Tau', 'inViews:TAUFTFCoreViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_TauCore_FTFAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_TauIso_FTF',                 'BS ESD AODFULL', 'Tau', 'inViews:TAUFTFIsoViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_TauIso_FTFAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Tau_FTF',                 'BS ESD AODFULL', 'Tau', 'inViews:TAUFTFIdViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Tau_FTFAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_IDTrack_Tau_IDTrig',                 'BS ESD AODFULL', 'Tau', 'inViews:TAUFTFIdViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Tau_IDTrigAux.',          'BS ESD AODFULL', 'Tau'),

    ('TrigRoiDescriptorCollection#HLT_Roi_Tau_TrackTwo',              'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_Roi_Tau_Track',              'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_Roi_Tau',              'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_Roi_TauCore_MVA',                'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_Roi_TauCore',             'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_Roi_TauIso_TauID',             'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#HLT_Roi_TauID',           'BS ESD AODFULL AODSLIM',  'Steer'),

    # Jet
    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjesIS',                      'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_subjesISAux.'+JetVars,       'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjesIS_ftf',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_subjesIS_ftfAux.'+JetVars,   'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjesgscIS_ftf',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_subjesgscIS_ftfAux.'+JetVars, 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subresjesgscIS_ftf',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_subresjesgscIS_ftfAux.'+JetVars, 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_subjes',                        'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_subjesAux.'+JetVars,         'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoJets_nojcalib',                      'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoJets_nojcalibAux.'+JetVars,       'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoSKJets_nojcalib',                    'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoSKJets_nojcalibAux.'+JetVars,     'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMTopoCSSKJets_nojcalib',                  'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMTopoCSSKJets_nojcalibAux.'+JetVars,   'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoJets_subjes',                       'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoJets_subjesAux.'+JetVars,        'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoJets_nojcalib',                     'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoJets_nojcalibAux.'+JetVars,      'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10JetRCJets_subjesIS',                      'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10JetRCJets_subjesISAux.'+JetVars,       'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_jes',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets_jesAux.'+JetVars, 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_subjesIS_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMPFlowJets_subjesIS_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_subjesgscIS_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMPFlowJets_subjesgscIS_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMPFlowJets_subresjesgscIS_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowJets_nojcalib_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMPFlowJets_nojcalib_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    ('xAOD::JetContainer#HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftf',                'BS ESD AODFULL', 'Jet'),
    ('xAOD::JetAuxContainer#HLT_AntiKt4EMPFlowCSSKJets_nojcalib_ftfAux.'+JetVars, 'BS ESD AODFULL', 'Jet'),

    # FS tracks
    ('xAOD::TrackParticleContainer#HLT_IDTrack_FS_FTF',                 'BS ESD AODFULL', 'Jet'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_FS_FTFAux.',          'BS ESD AODFULL', 'Jet'),

    # FS vertices
    ('xAOD::VertexContainer#HLT_IDVertex_FS',                  'BS ESD AODFULL', 'Jet'),
    ('xAOD::VertexAuxContainer#HLT_IDVertex_FSAux.',          'BS ESD AODFULL', 'Jet'),


    # custom BeamSpot tracks - we don't want to write these out in general so this 
    # is commented, if we want to write them out at some point, then these lines
    # should be uncommented and they should get written out
    #    ('xAOD::TrackParticleContainer#HLT_IDTrack_BeamSpot_FTF',         'BS ESD AODFULL', 'ID', 'inViews:beamspotViewRoIs' ),
    #    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_BeamSpot_FTFAux.',  'BS ESD AODFULL', 'ID', 'inViews:beamspotViewRoIs' ),

    # MET
    ('xAOD::TrigMissingETContainer#HLT_MET_cell',                               'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_cellAux.',                        'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_mht',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_mhtAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tcpufit',                       'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tcpufitAux.',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tc',                            'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tcAux.',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tc_em',                            'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tc_emAux.',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_trkmht',                        'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_trkmhtAux.',                 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_pfsum',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_pfsumAux.',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersFS',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersFSAux.nCells', 'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    # tau
    # will enable when needed
    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_CaloOnly',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUCaloViews'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_CaloOnlyAux.',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_CaloOnlyMVA',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUCaloMVAViews'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_CaloOnlyMVAAux.',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_MVA',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFIsoViews'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_MVAAux.',              'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_Precision',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFIdViews'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_PrecisionAux.',              'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_Presel',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFTrackViews'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_PreselAux.',              'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    # tau calo clusters
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersLC',                  'BS ESD AODFULL', 'Tau', 'inViews:TAUCaloViews'),
    ('xAOD::CaloClusterTrigAuxContainer#HLT_TopoCaloClustersLCAux.nCells' ,'BS ESD AODFULL', 'Tau'),

    # tau tracks
    ('xAOD::TauTrackContainer#HLT_tautrack_MVA',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFIsoViews'),
    ('xAOD::TauTrackAuxContainer#HLT_tautrack_MVAAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),
    ('xAOD::TauTrackContainer#HLT_tautrack_Precision',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFIdViews'),
    ('xAOD::TauTrackAuxContainer#HLT_tautrack_PrecisionAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),
    ('xAOD::TauTrackContainer#HLT_tautrack_Presel',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau', 'inViews:TAUFTFTrackViews'),
    ('xAOD::TauTrackAuxContainer#HLT_tautrack_PreselAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    # bjet RoI Descriptor used for EventView creation
    ('TrigRoiDescriptorCollection#HLT_Roi_Bjet',                   'BS ESD AODFULL', 'Bjet'),

    # bjet Second Stage Fast tracks
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Bjet_FTF',        'BS ESD AODFULL', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Bjet_FTFAux.', 'BS ESD AODFULL', 'Bjet'),

    # bjet Second Stage Precision tracks
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Bjet_IDTrig',        'BS ESD AODFULL', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Bjet_IDTrigAux.', 'BS ESD AODFULL', 'Bjet'),

    # FIXME: add vertex tracks

    # bjet jets
    ('xAOD::JetContainer#HLT_bJets',                             'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::JetAuxContainer#HLT_bJetsAux.btaggingLink',          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),


    # secvertex for b-jets
    ('xAOD::VertexContainer#HLT_BTaggingSecVtx',                          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::VertexAuxContainer#HLT_BTaggingSecVtxAux.',                   'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),  

    # btagvertex for b-jets
    ('xAOD::BTagVertexContainer#HLT_BTaggingJFVtx',                          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::BTagVertexAuxContainer#HLT_BTaggingJFVtxAux.',                   'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),
    
    # bjet b-tagging
    ('xAOD::BTaggingContainer#HLT_BTagging',                          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet', 'inViews:BTagViews'),
    ('xAOD::BTaggingAuxContainer#HLT_BTaggingAux.'+BTagVars,          'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Bjet'),

    # MinBias

    ('xAOD::TrackParticleContainer#HLT_IDTrack_MinBias_FTF',                 'BS ESD AODFULL', 'MinBias', 'inViews:TrkView'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_MinBias_FTFAux.',          'BS ESD AODFULL', 'MinBias'),


    # Cosmic
    ('xAOD::TrackParticleContainer#HLT_IDTrack_Cosmic_FTF',                 'BS ESD AODFULL', 'Cosmic'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrack_Cosmic_FTFAux.',             'BS ESD AODFULL', 'Cosmic'),

    # ID
    # Requested by TrigUpgradeTest/IDCalo.py
    # what are these ???
    ('xAOD::TrackParticleContainer#HLT_IDTrack',                        'BS ESD AODFULL', 'ID'),
    ('xAOD::TrackParticleAuxContainer#HLT_IDTrackAux.',                 'BS ESD AODFULL', 'ID'),

    ('ROIB::RoIBResult#*',                         'ESD', 'Misc'),

    ('xAOD::TrigCompositeContainer#HLT_SpacePointCounts',            'BS ESD AODFULL AODSLIM', 'MinBias'),
    ('xAOD::TrigCompositeAuxContainer#HLT_SpacePointCountsAux.totNumPixSP.totNumPixCL_1.totNumPixCL_2.totNumPixCLmin3.pixClBarrel.pixClEndcapA.pixClEndcapC.totNumSctSP.SctSpBarrel.SctSpEndcapA.SctSpEndcapC',     'BS ESD AODFULL AODSLIM', 'MinBias'),

    ('xAOD::TrigCompositeContainer#HLT_TrackCount',                                             'BS ESD AODFULL AODSLIM', 'MinBias'),
    ('xAOD::TrigCompositeAuxContainer#HLT_TrackCountAux.ntrks.pTcuts.z0cuts.counts',            'BS ESD AODFULL AODSLIM', 'MinBias'),
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
