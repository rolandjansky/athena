# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

__all__ = 'getTriggerDerivationConfig'

TriggerDerivationConfig = {}


# egamma
egammavars = '.'.join([ 'eta', 'phi' ])
TriggerDerivationConfig['egamma'] = [
    'xAOD::ElectronContainer#HLT_egamma_Electrons',             
    'xAOD::ElectronAuxContainer#HLT_egamma_ElectronsAux.' + egammavars,
    'xAOD::PhotonContainer#HLT_egamma_Photons',
    'xAOD::PhotonAuxContainer#HLT_egamma_PhotonsAux.' + egammavars
    ]


trackingvars = '.'.join([ 'pt', 'eta', 'phi', 'd0', 'z0'])

# muon
muonvars = '.'.join([ 'pt', 'eta', 'phi', 'muonType', 'author', 'inDetTrackParticleLink'])
TriggerDerivationConfig['muon'] = [
    'xAOD::MuonContainer#HLT_MuonEFInfo',
    'xAOD::MuonAuxContainer#HLT_MuonEFInfoAux.' + muonvars,
    # tracking
    'xAOD::TrackParticleContainer#HLT_InDetTrigTrackingxAODCnv_Muon_EFID',
    'xAOD::TrackParticleAuxContainer#HLT_InDetTrigTrackingxAODCnv_Muon_EFIDAux.' + trackingvars,
    ]



# tau
tauvars = '.'.join([ 'pt', 'eta', 'phi', 'm' ])
TriggerDerivationConfig['tau'] = [
    'xAOD::TauJetContainer#HLT_TrigTauRecMerged',
    'xAOD::TauJetAuxContainer#HLT_TrigTauRecMergedAux.' + tauvars,
    ]



# jet

jetvars = '.'.join([ 'pt', 'eta', 'phi' ])
TriggerDerivationConfig['jet'] = [

    # antiKt0.4
    # em  clusters Full Scan
    'xAOD::JetContainer#HLT_a4tcemsubjesFS',
    'xAOD::JetTrigAuxContainer#HLT_a4tcemsubjesFSAux.' + jetvars,
    'xAOD::JetContainer#HLT_a4tcemnojcalibFS',
    'xAOD::JetTrigAuxContainer#HLT_a4tcemnojcalibFSAux.' + jetvars,

    # antiKt1.0
    # em  clusters Full Scan

    'xAOD::JetContainer#HLT_a10tcemsubjesFS',
    'xAOD::JetTrigAuxContainer#HLT_a10tcemsubjesFSAux.' + jetvars,
    'xAOD::JetContainer#HLT_a10tcemnojcalibFS',
    'xAOD::JetTrigAuxContainer#HLT_a10tcemnojcalibFSAux.' + jetvars,
    ]



# b-jets (variables not clearly defined)
TriggerDerivationConfig['bjet'] = [
    'xAOD::JetContainer#HLT_EFJet',
    'xAOD::JetTrigAuxContainer#HLT_EFJetAux.',
    'xAOD::JetContainer#HLT_SplitJet',
    'xAOD::JetTrigAuxContainer#HLT_SplitJetAux.',
    ]



#btagging
TriggerDerivationConfig['btagging'] = [
    'xAOD::BTaggingContainer#HLT_HLTBjetFex',
    'xAOD::BTaggingAuxContainer#HLT_HLTBjetFexAux.',
    ]



# b-physics (all variables)
TriggerDerivationConfig['bphys'] = [
    'xAOD::TrigBphysContainer#HLT_EFBMuMuFex',
    'xAOD::TrigBphysAuxContainer#HLT_EFBMuMuFexAux.',
    'xAOD::TrigBphysContainer#HLT_EFBMuMuXFex',
    'xAOD::TrigBphysAuxContainer#HLT_EFBMuMuXFexAux.',

    'xAOD::TrigBphysContainer#HLT_EFTrackMass',
    'xAOD::TrigBphysAuxContainer#HLT_EFTrackMassAux.',
    'xAOD::TrigBphysContainer#HLT_EFMultiMuFex',
    'xAOD::TrigBphysAuxContainer#HLT_EFMultiMuFexAux.',

    'xAOD::TrigBphysContainer#HLT_L2BMuMuXFex',
    'xAOD::TrigBphysAuxContainer#HLT_L2BMuMuXFexAux.',

    # tracking (the next two are also listed in the muon derivation list)
    'xAOD::TrackParticleContainer#HLT_InDetTrigTrackingxAODCnv_Muon_EFID',
    'xAOD::TrackParticleAuxContainer#HLT_InDetTrigTrackingxAODCnv_Muon_EFIDAux.' + trackingvars,
    'xAOD::TrackParticleContainer#HLT_InDetTrigTrackingxAODCnv_Bphysics_IDTrig',
    'xAOD::TrackParticleAuxContainer#HLT_InDetTrigTrackingxAODCnv_Bphysics_IDTrigAux.' + trackingvars,
    'xAOD::TrackParticleContainer#HLT_InDetTrigTrackingxAODCnv_Muon_IDTrig',
    'xAOD::TrackParticleAuxContainer#HLT_InDetTrigTrackingxAODCnv_Muon_IDTrigAux.' + trackingvars,    

    ]




#minbias
TriggerDerivationConfig['minbias'] = [
    'xAOD::TrigSpacePointCounts#HLT_spacepoints',
    'xAOD::TrigSpacePointCountsAuxContainer#HLT_spacepointsAux.',
    'xAOD::TrigT2MbtsBits#HLT_T2Mbts',
    'xAOD::TrigT2MbtsBitsAuxContainer#HLT_T2MbtsAux.',
    'xAOD::TrigVertexCounts#HLT_vertexcounts',
    'xAOD::TrigVertexCountsAuxContainer#HLT_vertexcountsAux.',
    'xAOD::TrigTrackCounts#HLT_trackcounts',
    'xAOD::TrigTrackCountsAuxContainer#HLT_trackcountsAux.',
    'xAOD::TrigT2ZdcSignals#HLT_zdcsignals',
    'xAOD::TrigT2ZdcSignalsAuxContainer#HLT_zdcsignalsAux.',


    # particles == L2 List
    'TrigT2MbtsBits#HLT_T2Mbts',
    'TrigSpacePointCounts#HLT_spacepoints',   # ? do we realy need it!

    # == EF List
    'TrigTrackCounts#HLT_trackcounts',        #? this is the CMS trigger obj
    'TrigVertexCounts#HLT_vertexcounts',
    ]



metvars = '.'.join([ 'Met', 'Metx', 'Mety', 'MetPhi', 'SumET' ])
TriggerDerivationConfig['met'] = [
    'xAOD::TrigMissingETContainer#HLT_TrigEFMissingET',
    'xAOD::TrigMissingETAuxContainer#HLT_TrigEFMissingETAux.' + metvars,
    ]



TriggerDerivationConfig['level1'] = [
    'xAOD::MuonRoIContainer#LVL1MuonRoIs',
    'xAOD::MuonRoIAuxContainer#LVL1MuonRoIsAux.' + '.'.join(['thrValue', 'eta', 'phi', 'RoI']),
    'xAOD::EmTauRoIContainer#LVL1EmTauRoIs',
    'xAOD::EmTauRoIAuxContainer#LVL1EmTauRoIsAux.',
    'xAOD::JetRoIContainer#LVL1JetRoIs',
    'xAOD::JetRoIAuxContainer#LVL1JetRoIsAux.',
    'xAOD::JetEtRoI#LVL1JetEtRoI',
    'xAOD::JetEtRoIAuxInfo#LVL1JetEtRoIAux.',
    'xAOD::EnergySumRoI#LVL1EnergySumRoI',
    'xAOD::EnergySumRoIAuxInfo#LVL1EnergySumRoIAux.',
    ]



TriggerDerivationConfig['decision'] = [
    'xAOD::TrigDecision#xTrigDecision',
    'xAOD::TrigDecisionAuxInfo#xTrigDecisionAux.',
    'xAOD::TrigConfKeys#TrigConfKeys',
    ]



TriggerDerivationConfig['navigation'] = [
    'xAOD::TrigNavigation#TrigNavigation',
    'xAOD::TrigNavigationAuxInfo#TrigNavigationAux.'
    ]





def getTriggerDerivationConfig(signatures):
    from AthenaCommon.Logging import logging
    logger = logging.getLogger( "TriggerEDMAnalysis.getTriggerDerivationConfig" )

    # turn single string into list
    if type(signatures) == str:
        signatures = [signatures]

    # check if signatures are requested that don't exist and print warning
    unknownSignatures = [ s for s in signatures if not s in TriggerDerivationConfig ]
    if unknownSignatures:
        logger.warning("requesting derivation configuration for unknown trigger signatures: %s" % ", ".join(unknownSignatures) )
    

    # build list of collections for the requested signatures, removing duplicates
    derivationCollections = []
    for s in signatures:
        if not s in signatures: continue

        for c in TriggerDerivationConfig[s]:

            if c in derivationCollections: continue

            derivationCollections += [c]

    return derivationCollections

