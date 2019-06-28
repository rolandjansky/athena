# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration


# definition of trigger EDM for the Run 3, mostly similar to Run 2
# however there are different set of EDM to stream and different functionalities required
from AthenaCommon.Logging import logging
__log = logging.getLogger('TriggerEDMRun3Config')
def recordable( name ):
    """
    Verify that the name is in the list of recorded objects and conform to the name convention


    In Run 2 it was a delicate process to configure correctly what get recorded
    as it had to be set in the produce arlgoirhm as well in here in a consistent manner.

    For Run 3 every alg input/output key can be crosschecked agains the list of objects to record defined here.
    I.e. in the configuration alg developer would do this:
    from TriggerEDM.TriggerEDMRun3 import recordable

    alg.outputKey = recordable("SomeKey")
    If the names are correct the outputKey is assigned with SomeKey, if there is a missmatch an exception is thrown.
    """

    if name in ["HLTSummary", "L1DecoderSummary"] or "L1" in name or "RoI" in name:
        pass
    else: #negative filtering
        if not name.startswith( "HLT_" ):
            __log.error( "The collection name {0} does not start with HLT_".format( name ) )
        if "Aux" in name and not name[-1] != ".":
            __log.error( "The collection name {0} is Aux but the name does not end with the '.'".format( name ) )

    for entry in TriggerHLTList:
        if entry[0].split( "#" )[1] == name:
            return name
    msg = "The collection name {0} is not declared to be stored by HLT. Add it to TriggerEDMRun3.py".format( name )
    __log.error("ERROR in recordable() - see following stack trace.")
    raise RuntimeError( msg )
    return name

TriggerHLTList = [

    #framework/steering
    ('xAOD::TrigCompositeContainer#HLTSummary',              'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#HLTSummaryAux.',       'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1DecoderSummary',        'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1DecoderSummaryAux.', 'BS ESD AODFULL AODSLIM', 'Steer'),

    ('xAOD::TrigDecision#xTrigDecision' ,                    'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.',          'ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigConfKeys#TrigConfKeys' ,                     'ESD AODFULL AODSLIM', 'Steer'),

    ('TrigRoiDescriptorCollection#EMRoIs',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#MURoIs',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#METRoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),
    ('TrigRoiDescriptorCollection#JETRoI',                   'BS ESD AODFULL AODSLIM',  'Steer'),

    ('xAOD::TrigCompositeContainer#L1EM',                    'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1EMAux.',             'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1MU',                    'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1MUAux.',             'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1MET',                   'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1METAux.',            'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeContainer#L1J',                     'BS ESD AODFULL AODSLIM', 'Steer'),
    ('xAOD::TrigCompositeAuxContainer#L1JAux.',              'BS ESD AODFULL AODSLIM', 'Steer'),

    # Egamma


    ('xAOD::TrigEMClusterContainer#HLT_L2CaloEMClusters',           'BS ESD AODFULL', 'Egamma', 'inViews:EMCaloViews'), # last arg specifies in which view container the fragments are, look into the proprty of View maker alg for it
    ('xAOD::TrigEMClusterAuxContainer#HLT_L2CaloEMClustersAux.',    'BS ESD AODFULL', 'Egamma'),
    ('xAOD::TrigPhotonContainer#HLT_L2Photons',                     'BS ESD AODFULL', 'Egamma', 'inViews:L2PhotonRecoViews'),
    ('xAOD::TrigPhotonAuxContainer#HLT_L2PhotonsAux.',              'BS ESD AODFULL', 'Egamma'),
    ('xAOD::TrigElectronContainer#HLT_L2Electrons',                 'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews'),
    ('xAOD::TrigElectronAuxContainer#HLT_L2ElectronsAux.',          'BS ESD AODFULL', 'Egamma'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_Electron',        'BS ESD AODFULL', 'Egamma', 'inViews:EMElectronViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_ElectronAux.', 'BS ESD AODFULL', 'Egamma'),

    ('xAOD::CaloClusterContainer#HLT_CaloEMClusters',               'BS ESD AODFULL', 'Egamma', 'inViews:precisionCaloViews'),
    ('xAOD::CaloClusterAuxContainer#HLT_CaloEMClustersAux.',        'BS ESD AODFULL', 'Egamma'),

    # This varient needed by TrigUpgradeTest/egammaRinger.py
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClusters',             'BS ESD AODFULL', 'Egamma' 'inViews:EMCaloViews'),
    ('xAOD::CaloClusterAuxContainer#HLT_TopoCaloClustersAux.',      'BS ESD AODFULL', 'Egamma'),

    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersRoI',          'BS ESD AODFULL', 'Egamma' 'inViews:precisionCaloViews'),
    ('xAOD::CaloClusterAuxContainer#HLT_TopoCaloClustersRoIAux.',   'BS ESD AODFULL', 'Egamma'),

    # Muon

    #Id track particles
    ('xAOD::TrackParticleContainer#HLT_xAODTracks_Muon',                 'BS ESD AODFULL', 'Muon', 'inViews:EMCombViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_MuonAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_MuonFS',                 'BS ESD AODFULL', 'Muon', 'inViews:MUCBFSViews'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_MuonFSAux.',          'BS ESD AODFULL', 'Muon'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_MuonIso',                 'BS ESD AODFULL', 'Muon', 'inViews:MUEFIsoViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_MuonIsoAux.',          'BS ESD AODFULL', 'Muon'),

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


    # Tau

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_TauCore',                 'BS ESD AODFULL', 'Tau', 'inViews:TCoreViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_TauCoreAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_TauIso',                 'BS ESD AODFULL', 'Tau', 'inViews:TIsoViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_TauIsoAux.',          'BS ESD AODFULL', 'Tau'),

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_FastTrack',                 'BS ESD AODFULL', 'Tau', 'inViews:TCoreViewRoIs'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_FastTrackAux.',          'BS ESD AODFULL', 'Tau'),

    # MET
    ('xAOD::TrigMissingETContainer#HLT_MET',                               'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_METAux.',                        'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_mht',                           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_mhtAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tcPufit',                       'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tcPufitAux.',                'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::TrigMissingETContainer#HLT_MET_tc',                            'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::TrigMissingETAuxContainer#HLT_MET_tcAux.',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersFS',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),
    ('xAOD::CaloClusterAuxContainer#HLT_TopoCaloClustersFSAux.',           'BS ESD AODFULL AODSLIM AODVERYSLIM', 'MET'),

    # tau
    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged',                         'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'), 
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMergedAux.',                  'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'), 

    ('xAOD::TauJetContainer#HLT_TrigTauRecMerged_MVA',                     'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),
    ('xAOD::TauJetAuxContainer#HLT_TrigTauRecMerged_MVAAux.',              'BS ESD AODFULL AODSLIM AODVERYSLIM', 'Tau'),

    # tau calo clusters
    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersLC',                'BS ESD AODFULL', 'Tau', 'inViews:TAUCaloRoIs'),
    ('xAOD::CaloClusterAuxContainer#HLT_TopoCaloClustersLCAux.',         'BS ESD AODFULL', 'Tau'),

    ('xAOD::CaloClusterContainer#HLT_TopoCaloClustersLCLC',                'BS ESD AODFULL', 'Tau', 'inViews:TAUCaloRoIs'),
    ('xAOD::CaloClusterAuxContainer#HLT_TopoCaloClustersLCLCAux.',         'BS ESD AODFULL', 'Tau'),

    # tau tracks
    ('xAOD::TauTrackContainer#HLT_tautrack_MVA',                           'BS ESD AODFULL AODSLIM AODVERYSLYM', 'MET'),
    ('xAOD::TauTrackAuxContainer#HLT_tautrack_MVAAux.',                    'BS ESD AODFULL AODSLIM AODVERYSLYM', 'MET'),

    #bjet

    ('xAOD::TrackParticleContainer#HLT_xAODTracks_FS',                     'BS ESD AODFULL', 'Bjet'),
    ('xAOD::TrackParticleAuxContainer#HLT_xAODTracks_FSAux.',              'BS ESD AODFULL', 'Bjet'),

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

EDMDetails = {}

EDMDetails[ "TrigRoiDescriptorCollection" ]     = {'persistent':"TrigRoiDescriptorCollection_p3"}

EDMDetails[ "xAOD::TrigCompositeAuxContainer" ] = {'persistent':"xAOD::TrigCompositeAuxContainer_v2"}

EDMDetails[ "xAOD::TrigEMClusterAuxContainer" ] = {'persistent':"xAOD::TrigEMClusterAuxContainer_v2"}



def persistent( transient ):
    """
    Persistent EDM claass, for xAOD it is the actual class version

    Uses list defined above. If absent assumes v1
    """
    if transient in EDMDetails:
        return EDMDetails[transient]['persistent']
    return transient+"_v1"


def tpMap():
    """
    List
    """
    l = {}
    for tr in EDMDetails.keys():
        if "xAOD" in tr:
            continue
        l[tr] = persistent(tr)
    return l
