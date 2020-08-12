#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-20.1.7.2/AOD-20.1.7.2-full.pool.root'
    keys = [
        #xAOD::TrackParticle


        'CombinedMuonTrackParticles',
        'CombinedStauTrackParticles',
        'ExtrapolatedMuonTrackParticles',
        'ExtrapolatedStauTrackParticles',
        'GSFTrackParticles',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_BjetPrmVtx_FTF',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTF',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_FTF',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_EFID',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_EFID',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTF',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauCore_FTF',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_TauIso_FTF',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_EFID',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTF',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_IDTrig',
        'HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_CombTrackParticles',
        'HLT_xAOD__TrackParticleContainer_MuTagIMO_EF_ExtrapTrackParticles',
        'HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles',
        'HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles',
        'HLT_xAOD__TrackParticleContainer_eMuonEFInfo_CombTrackParticles',
        'HLT_xAOD__TrackParticleContainer_eMuonEFInfo_ExtrapTrackParticles',
        'InDetForwardTrackParticles',
        'InDetPixelPrdAssociationTrackParticles',
        'InDetTrackParticles',
        'MSonlyTracklets',
        'MuonSpectrometerTrackParticles',

        #xAOD::VertexAuxContainer_v1
        'BTagging_AntiKt10LCTopoSecVtx',
        'BTagging_AntiKt10TruthWZSecVtx',
        'BTagging_AntiKt2TrackSecVtx',
        'BTagging_AntiKt3TrackSecVtx',
        'BTagging_AntiKt4EMTopoSecVtx',
        'BTagging_AntiKt4LCTopoSecVtx',
        'BTagging_AntiKt4TrackSecVtx',
        'BTagging_AntiKt4TruthWZSecVtx',
        'GSFConversionVertices',
        'HLT_xAOD__VertexContainer_BjetSecondaryVertexFex',
        'HLT_xAOD__VertexContainer_EFHistoPrmVtx',
        'HLT_xAOD__VertexContainer_SecondaryVertex',
        'HLT_xAOD__VertexContainer_xPrimVx',
        'MSDisplacedVertex',
        'PrimaryVertices',
        'TauSecondaryVertices',
             ]

    TPCnvTest(infile, keys)
