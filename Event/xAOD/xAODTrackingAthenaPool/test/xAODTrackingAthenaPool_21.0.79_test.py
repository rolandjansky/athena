#!/usr/bin/env python
"""
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""

from AthenaPoolUtilities.TPCnvTestConfig import TPCnvTest

if __name__ == "__main__":

    infile = 'aod/AOD-21.0.79/AOD-21.0.79-full.pool.root'
    keys = [
        #xAOD::TrackParticleAuxContainer_v3
        'CombinedMuonTrackParticles',
        'CombinedStauTrackParticles',
        'ExtrapolatedMuonTrackParticles',
        'ExtrapolatedStauTrackParticles',
        'GSFTrackParticles',
        'HLT_xAOD__TrackParticleContainer_GSFTrigTrackParticles',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTK',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTKRefit',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTKRefit_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_FTK_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bjet_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_BphysHighPt_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Bphysics_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Electron_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_FullScan_FTK',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_FullScan_FTKRefit',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTK',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTKRefit',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTKRefit_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_FTK_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Muon_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTK',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefit',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTKRefit_IDTrig',
        'HLT_xAOD__TrackParticleContainer_InDetTrigTrackingxAODCnv_Tau_FTK_IDTrig',
        'HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles',
        'HLT_xAOD__TrackParticleContainer_MuonEFInfo_CombTrackParticles_FullScan',
        'HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles',
        'HLT_xAOD__TrackParticleContainer_MuonEFInfo_ExtrapTrackParticles_FullScan',
        'InDetForwardTrackParticles',
        'InDetPixelPrdAssociationTrackParticles',
        'MSOnlyExtrapolatedMuonTrackParticles',
        'MSonlyTracklets',
        'MuonSpectrometerTrackParticles',

        #xAOD::VertexAuxContainer_v1
        'BTagging_AntiKt4EMTopoSecVtx',
        'GSFConversionVertices',
        'HLT_xAOD__VertexContainer_BjetSecondaryVertexFex',
        'HLT_xAOD__VertexContainer_EFHistoPrmVtx',
        'HLT_xAOD__VertexContainer_PrimVertexFTK',
        'HLT_xAOD__VertexContainer_PrimVertexFTKRaw',
        'HLT_xAOD__VertexContainer_PrimVertexFTKRefit',
        'HLT_xAOD__VertexContainer_SecondaryVertex',
        'HLT_xAOD__VertexContainer_xPrimVx',
        'MSDisplacedVertex',
        'PrimaryVertices',
        'TauSecondaryVertices',
             ]

    TPCnvTest(infile, keys)
