# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# File: D3PDMakerTest/python/difftuple_text.py
# Author: snyder@bnl.gov
# Date: Oct, 2010
# Purpose: Diff a root tuple file against a text dump.
#

import operator
import os
import types
import re
import string
import fnmatch
import ROOT
from functools import reduce
from D3PDMakerTest.split_list import split_list, split_list1

ignore_event_differences = False


# NEW:OLD
renames = {
    }

rename_keys = {
    # Hack for ordering difference when scell tuple is on.
    'caloMeta/EventInfoD3PDObject_000005' : 'caloMeta/EventInfoD3PDObject_000004',
    'physicsMeta/EventInfoD3PDObject_000003' : 'physicsMeta/EventInfoD3PDObject_000002',
    'physicsTrigDecMeta/EventInfoD3PDObject_000004' : 'physicsTrigDecMeta/EventInfoD3PDObject_000003',
    }

def atlasProduction(ver):
    return (ver and
            (ver.find('17.2')>=0 or
             ver.find('17.3')>=0) and
            #ver.find('AtlasProduction')>=0 and
            ver.find('AtlasPhysics')==-1 and
            ver.find('usatlas+u/snyder')==-1)


# List of branches to ignore for various versions.
# Format is a list of tuples: (V, V, V, [BRANCHES...])
# We consider the strings V in turn and see if they're contained
# in CMTPATH.  If the first V is `-', then we ignore the branches
# if there are any matches.  Otherwise, we ignore the branches
# unless there are any matches.
# If V is callable, then instead of looking for at match in CMTPATH,
# we call it with CMTPATH as an argument.
# The list of branches can include glob patterns.
_ignore_branches = [
    # temp
    ('17.2',
     ['EF_*',
      'L1_*',
      'L2_*',
      'trig_roidescriptor_*',
      'trig_bgCode',
      'trig_L1_esum_*',
      'trig_L2_sitrack_*',
      'trig_L2_idscan_*',
      'trig_L2_tilemufeature_*',
      'trig_L2_muonfeature_*',
      'trig_L2_muonfeaturedetails_*',
      'trig_L2_isomuonfeature_*',
      'trig_L2_combmuonfeature_*',
      'trig_L2_tiletrackmufeature_*',
      'trig_L2_sitrackfinder_*',
      'trig_L2_el_*',
      'trig_L2_ph_*',
      'trig_L2_met_*',
      'trig_L2_feb_met_*',
      'trig_DB_*',
      'trackjet_*',
      'el*_EF_*',
      'ph*_EF_*',
      'mu*_EF*',
      'el*_L1_*',
      'ph*_L1_*',
      'mu*_L1_*',
      'el*_L2_*',
      'ph*_L2_*',
      'mu*_L2*',
      'trig_EF_trigmuonefisolation_*',
      #'trig_EF_trigmuonef_*',
      'trig_EF_trigmugirl_*',
      'trig_EF_met_*',
        ]),
    
    # changed in devval
    ('17.2',
     ['*_BCH_CORR_CELL',
      '*_BCH_CORR_DOTX',
      '*_BCH_CORR_JET',
      '*_BCH_CORR_JET_FORCELL',
      '*_ENG_BAD_CELLS',
      '*_N_BAD_CELLS',
      '*_N_BAD_CELLS_CORR',
      '*_BAD_CELLS_CORR_E',
      '*_n90',
      '*_isBadMedium',
      '*_isBadTight',
      '*_isBadLooseMinus',
      '*_tgap3f',
      '*_hecf',
      'backgroundFlags',
      'lumiFlags',
      'el*_isEMLoose',
      'trig_EF_el*_isEMLoose',
      'el*_isEMMedium',
      'trig_EF_el*_isEMMedium',
      'el*_isEMTight',
      'trig_EF_el*_isEMTight',
      'el*_looseIso',
      'trig_EF_el*_looseIso',
      'el*_mediumIso',
      'trig_EF_el*_mediumIso',
      'el*_tightIso',
      'trig_EF_el*_tightIso',
      'el*_loosePP',
      'trig_EF_el*_loosePP',
      'el*_loosePPIso',
      'trig_EF_el*_loosePPIso',
      'el*_mediumPP',
      'trig_EF_el*_mediumPP',
      'el*_mediumPPIso',
      'trig_EF_el*_mediumPPIso',
      'el*_tightPP',
      'trig_EF_el*_tightPP',
      'el*_tightPPIso',
      'trig_EF_el*_tightPPIso',
      'el*_mediumWithoutTrack',
      'trig_EF_el*_mediumWithoutTrack',
      'el*_mediumIsoWithoutTrack',
      'trig_EF_el*_mediumIsoWithoutTrack',
      'el*_tightWithoutTrack',
      'trig_EF_el*_tightWithoutTrack',
      'el*_tightIsoWithoutTrack',
      'trig_EF_el*_tightIsoWithoutTrack',
      'el*_loose',
      'trig_EF_el*_loose',
      'el*_medium',
      'trig_EF_el*_medium',
      'el*_tight',
      'trig_EF_el*_tight',
      'el*_reta',
      'el*_rphi',
      'el*_hastrack',
      'el*_convanglematch',
      'el*_convtrackmatch',
      'el*_etacorrmag',
      'trig_EF_el*_etacorrmag',
      'el*_deltaPhiFromLast',
      'el*_deltaPhiRot',
      'el*_trackd0_physics',
      'trig_EF_el*_trackd0_physics',
      'el*_etaSampling1',
      'el*_hasconv',
      'el*_convvtxx',
      'el*_convvtxy',
      'el*_convvtxz',
      'el*_Rconv',
      'el*_zconv',
      'el*_convvtxchi2',
      'el*_pt1conv',
      'el*_convtrk1nBLHits',
      'el*_convtrk1nPixHits',
      'el*_convtrk1nSCTHits',
      'el*_convtrk1nTRTHits',
      'el*_pt2conv',
      'el*_convtrk2nBLHits',
      'el*_convtrk2nPixHits',
      'el*_convtrk2nSCTHits',
      'el*_convtrk2nTRTHits',
      'el*_ptconv',
      'el*_pzconv',
      'el*_EtringnoisedR03sig2',
      'el*_EtringnoisedR03sig3',
      'el*_EtringnoisedR03sig4',
      'trig_EF_el*_EtringnoisedR03sig2',
      'trig_EF_el*_EtringnoisedR03sig3',
      'trig_EF_el*_EtringnoisedR03sig4',
      'el*_ptcone20_zpv05',
      'el*_ptcone30_zpv05',
      'el*_ptcone40_zpv05',
      'el*_nucone20_zpv05',
      'el*_nucone30_zpv05',
      'el*_nucone40_zpv05',
      'el*_originbkg',
      'el*_typebkg',
      'el*_isolationlikelihoodjets',
      'el*_isolationlikelihoodhqelectrons',
      'el*_electronweight',
      'el*_electronbgweight',
      'el*_softeweight',
      'el*_softebgweight',
      'el*_neuralnet',
      'el*_Hmatrix',
      'el*_Hmatrix5',
      'el*_adaboost',
      'el*_softeneuralnet',
      'el*_ringernn',
      'el*_refittedTrack_*',
      'el*_nPixSharedHits',
      'el*_nPixSplitHits',
      'trig_EF_el*_nPixSplitHits',
      'el*_nPixHoles',
      'trig_EF_el*_nPixHoles',
      'el*_nSCTSharedHits',
      'el*_nSCTDoubleHoles',
      'el*_nTRTHoles',
      'trig_EF_el*_nTRTHoles',
      'el*_nSCTHoles',
      'trig_EF_el*_nSCTHoles',
      'el*_nSCTDeadSensors',
      'trig_EF_el*_nSCTDeadSensors',
      'el*_nBLayerSplitHits',
      'trig_EF_el*_nBLayerSplitHits',
      'el*_nBLSharedHits',
      'el*_nPixelDeadSensors',
      'trig_EF_el*_nPixelDeadSensors',
      'el*_nTRTXenonHits',
      'trig_EF_el*_nTRTXenonHits',
      'el*_vertx',
      'el*_verty',
      'el*_vertz',
      'el*_vertweight',
      'el*_LM',
      'el*_ES0_real',
      'el*_ES1_real',
      'el*_ES2_real',
      'el*_ES3_real',
      'el*_EcellS0',
      'el*_etacellS0',
      'el*_topoEtcone60',
      'el*_ED_Njets',
      'el*_ED_median',
      'el*_ED_sigma',
      'el*_Etcone40_ED_corrected',
      'el*_Etcone40_corrected',
      'el*_topoEtcone20_corrected',
      'el*_topoEtcone30_corrected',
      'el*_topoEtcone40_corrected',
      'el*_rawcl_*',
      'el*_goodOQ',
      'el*_isConv',
      'el*_nConv',
      'el*_convFlag',
      'el*_isEM',
      'trig_EF_el*_isEM',
      'el*_TRTHighTHitsRatio',
      'trig_EF_el*_TRTHighTHitsRatio',
      'el*_TRTHighTOutliersRatio',
      'trig_EF_el*_TRTHighTOutliersRatio',
      'el*_expectHitInBLayer',
      'trig_EF_el*_expectHitInBLayer',
      'el*_likelihoodsPixeldEdx',
      'trig_EF_el*_likelihoodsPixeldEdx',
      'el*_massPixeldEdx',
      'trig_EF_el*_massPixeldEdx',
      'el*_nGoodHitsPixeldEdx',
      'trig_EF_el*_nGoodHitsPixeldEdx',
      'el*_pixeldEdx',
      'trig_EF_el*_pixeldEdx',
      'el*_nSiHits',
      'trig_EF_el*_nSiHits',
      'el*_nSingleTrackConv',
      'el*_nDoubleTrackConv',
      'el*_HPV_*',
      'el*_CaloPointing_*',
      'el*_truth_bremLoc',
      'el*_truth_bremSi',
      'el*_truth_sumbrem',
      'el*_truth_charge',
      'el*_truth_mothercharge',
      'el*_truth_motherstatus',
      'mcevt_event_number',
      'mcevt_signal_process_id',
      'mcevt_nparticle',
      'mcevt_pileUpType',
      'mc_n',
      'mc_phi',
      'mc_eta',
      'mc_m',
      'mc_pt',
      'mc_barcode',
      'mc_pdgId',
      'mc_status',
      'mc_charge',
      'mc_child_index',
      'mc_parent_index',
      'mc_children',
      'mc_parents',
      'mc_vx_x',
      'mc_vx_y',
      'mc_vx_z',
      'mc_vx_barcode',
      'el*_truth_index',
      'ph*_isEMLoose',
      'trig_EF_ph*_isEMLoose',
      'ph*_isEMMedium',
      'trig_EF_ph*_isEMMedium',
      'ph*_isEMTight',
      'trig_EF_ph*_isEMTight',
      'ph*_isEM',
      'trig_EF_ph*_isEM',
      'ph*_loose',
      'trig_EF_ph*_loose',
      'ph*_tight',
      'trig_EF_ph*_tight',
      'ph*_type',
      'ph*_origin',
      'ph*_truth_E',
      'ph*_truth_matched',
      'ph*_truth_pt',
      'ph*_truth_eta',
      'ph*_truth_phi',
      'ph*_truth_barcode',
      'ph*_truth_status',
      'ph*_truth_motherbarcode',
      'ph*_truth_mothertype',
      'ph*_truth_mothercharge',
      'ph*_truth_motherstatus',
      'ph*_truth_index',
      'ph*_truth_charge',
      'ph*_deltaRRecPhoton',
      'ph*_goodOQ',
      'ph*_Etcone45',
      'trig_EF_ph*_Etcone45',
      'ph*_EtringnoisedR03sig2',
      'ph*_EtringnoisedR03sig3',
      'ph*_EtringnoisedR03sig4',
      'ph*_ptcone20_zpv05',
      'ph*_ptcone30_zpv05',
      'ph*_ptcone40_zpv05',
      'ph*_nucone20_zpv05',
      'ph*_nucone30_zpv05',
      'ph*_nucone40_zpv05',
      'ph*_isolationlikelihoodjets',
      'ph*_isolationlikelihoodhqelectrons',
      'ph*_loglikelihood',
      'ph*_photonweight',
      'ph*_photonbgweight',
      'ph*_neuralnet',
      'ph*_Hmatrix',
      'ph*_Hmatrix5',
      'ph*_adaboost',
      'ph*_ringernn',
      'ph*_etas3',
      'ph*_phis3',
      'ph*_Es3',
      'ph*_rawcl_*',
      'ph*_rings_E',
      'ph*_convMatchDeltaPhi1',
      'ph*_convMatchDeltaPhi2',
      'ph*_convMatchDeltaEta1',
      'ph*_convMatchDeltaEta2',
      'ph*_vx_px',
      'ph*_vx_py',
      'ph*_vx_pz',
      'ph*_vx_m',
      'ph*_vx_E',
      'ph*_vx_sumPt',
      'ph*_vx_convTrk_patternReco*',
      'ph*_vx_convTrk_nSCTSharedHits',
      'ph*_vx_convTrk_nBLSharedHits',
      'ph*_vx_convTrk_nTRTDeadStraws',
      'ph*_vx_convTrk_nPixelDeadSensors',
      'ph*_vx_convTrk_nSCTDeadSensors',
      'ph*_vx_convTrk_nPixHoles',
      'ph*_vx_convTrk_nPixSplitHits',
      'ph*_vx_convTrk_nPixSharedHits',
      'ph*_vx_convTrk_nSCTDoubleHoles',
      'ph*_vx_convTrk_nTRTXenonHits',
      'ph*_vx_convTrk_nTRTHoles',
      'ph*_vx_convTrk_nSCTHoles',
      'ph*_vx_convTrk_nBLayerSplitHits',
      'ph*_vx_convTrk_nCSCEtaHits',
      'ph*_vx_convTrk_nCSCPhiHits',
      'ph*_vx_convTrk_nCscEtaHoles',
      'ph*_vx_convTrk_nCscPhiHoles',
      'ph*_vx_convTrk_nTGCEtaHits',
      'ph*_vx_convTrk_nTGCPhiHits',
      'ph*_vx_convTrk_nTgcEtaHoles',
      'ph*_vx_convTrk_nTgcPhiHoles',
      'ph*_vx_convTrk_nRPCEtaHits',
      'ph*_vx_convTrk_nRPCPhiHits',
      'ph*_vx_convTrk_nRpcEtaHoles',
      'ph*_vx_convTrk_nRpcPhiHoles',
      'ph*_vx_convTrk_nMDTHits',
      'ph*_vx_convTrk_nMdtHoles',
      'ph*_vx_convTrk_nHits',
      'ph*_vx_convTrk_nHoles',
      'ph*_vx_convTrk_nSiHits',
      'ph*_vx_convTrk_hitPattern',
      'ph*_vx_convTrk_TRTHighTHitsRatio',
      'ph*_vx_convTrk_TRTHighTOutliersRatio',
      'ph*_vx_convTrk_weight',
      'ph*_truth_isConv',
      'ph*_truth_Rconv',
      'ph*_truth_zconv',
      'ph*_topoEtcone60',
      'ph*_vx_Chi2',
      'ph*_vx_Dcottheta',
      'ph*_vx_Dphi',
      'ph*_vx_Dist',
      'ph*_vx_DR1R2',
      'ph*_CaloPointing_eta',
      'ph*_CaloPointing_sigma_eta',
      'ph*_CaloPointing_zvertex',
      'ph*_CaloPointing_sigma_zvertex',
      'ph*_HPV_eta',
      'ph*_HPV_sigma_eta',
      'ph*_HPV_zvertex',
      'ph*_HPV_sigma_zvertex',
      'ph*_NN_passes',
      'ph*_NN_discriminant',
      'ph*_ES0_real',
      'ph*_ES1_real',
      'ph*_ES2_real',
      'ph*_ES3_real',
      'ph*_EcellS0',
      'ph*_etacellS0',
      'ph*_ED_median',
      'ph*_ED_sigma',
      'ph*_ED_Njets',
      'ph*_Etcone40_ED_corrected',
      'ph*_Etcone40_corrected',
      'ph*_convIP',
      'ph*_convIPRev',
      'ph*_el_index',
      'ph*_convanglematch',
      'ph*_zvertex',
      'ph*_errz',
      'ph*_etap',
      'ph*_depth',
      'trig_EF_ph*_convanglematch',
      'ph*_convtrackmatch',
      'trig_EF_ph*_convtrackmatch',
      'ph*_isRecovered',
      'trig_EF_ph*_isRecovered',
      'ph*_looseAR',
      'trig_EF_ph*_looseAR',
      'ph*_looseARIso',
      'trig_EF_ph*_looseARIso',
      'ph*_tightAR',
      'trig_EF_ph*_tightAR',
      'ph*_tightARIso',
      'trig_EF_ph*_tightARIso',
      'ph*_tightIso',
      'trig_EF_ph*_tightIso',
      'ph*_looseIso',
      'trig_EF_ph*_looseIso',
      'ph*_topoEtcone20_corrected',
      'ph*_topoEtcone30_corrected',
      'ph*_topoEtcone40_corrected',
      'ph*_convFlag',
      'el*_Unrefittedtrack_*',
      'el_gxx_*',
      'ef_el_*',
      'elfilt_*',
      'elfilt2_*',
      'elfilt4_*',
      'elfilt3_*',
      'emu_*',
      'cocl_m',
      'cocl_raw_eta_*',
      'cocl_raw_phi_*',
      'cocl_eta_*',
      'cocl_phi_*',
      'cocl_E_em',
      'cocl_E_had',
      'cocl_etamax_*',
      'mu*_energyLossErr',
      'mu*_energyLossPar',
      'mu*_etCore',
      'mu*_caloMuonIdTag',
      'mu*_caloLRLikelihood',
      'mu*_muonType',
      'mu*_bestMatch',
      'mu*_isStandAloneMuon',
      'mu*_isCombinedMuon',
      'mu*_isLowPtReconstructedMuon',
      'mu*_isSegmentTaggedMuon',
      'mu*_isCaloMuonId',
      'mu*_alsoFoundByLowPt',
      'mu*_alsoFoundByCaloMuonId',
      'mu*_isSiliconAssociatedForwardMuon',
      'mu*_ie_*',
      'mu*_ms_*',
      'mu*_cov_d0_exPV',
      'mu*_cov_phi_exPV',
      'mu*_cov_qoverp_exPV',
      'mu*_cov_theta_exPV',
      'mu*_cov_z0_exPV',
      'mu*_err_d0_exPV',
      'mu*_err_phi_exPV',
      'mu*_err_qoverp_exPV',
      'mu*_err_theta_exPV',
      'mu*_err_z0_exPV',
      'mu*_nMDTBIHits',
      'mu*_nMDTBMHits',
      'mu*_nMDTBOHits',
      'mu*_nMDTBEEHits',
      'mu*_nMDTBIS78Hits',
      'mu*_nMDTEIHits',
      'mu*_nMDTEMHits',
      'mu*_nMDTEOHits',
      'mu*_nMDTEEHits',
      'mu*_nRPCLayer1EtaHits',
      'mu*_nRPCLayer2EtaHits',
      'mu*_nRPCLayer3EtaHits',
      'mu*_nRPCLayer1PhiHits',
      'mu*_nRPCLayer2PhiHits',
      'mu*_nRPCLayer3PhiHits',
      'mu*_nTGCLayer1EtaHits',
      'mu*_nTGCLayer2EtaHits',
      'mu*_nTGCLayer3EtaHits',
      'mu*_nTGCLayer4EtaHits',
      'mu*_nTGCLayer1PhiHits',
      'mu*_nTGCLayer2PhiHits',
      'mu*_nTGCLayer3PhiHits',
      'mu*_nTGCLayer4PhiHits',
      'mu*_barrelSectors',
      'mu*_endcapSectors',
      'mu*_primarySector',
      'mu*_secondarySector',
      'mu*_innerSmallHits',
      'mu*_innerLargeHits',
      'mu*_middleSmallHits',
      'mu*_middleLargeHits',
      'mu*_outerSmallHits',
      'mu*_outerLargeHits',
      'mu*_extendedSmallHits',
      'mu*_extendedLargeHits',
      'mu*_innerSmallHoles',
      'mu*_innerLargeHoles',
      'mu*_middleSmallHoles',
      'mu*_middleLargeHoles',
      'mu*_outerSmallHoles',
      'mu*_outerLargeHoles',
      'mu*_extendedSmallHoles',
      'mu*_extendedLargeHoles',
      'mu*_phiLayer1Hits',
      'mu*_phiLayer2Hits',
      'mu*_phiLayer3Hits',
      'mu*_phiLayer4Hits',
      'mu*_etaLayer1Hits',
      'mu*_etaLayer2Hits',
      'mu*_etaLayer3Hits',
      'mu*_etaLayer4Hits',
      'mu*_phiLayer1Holes',
      'mu*_phiLayer2Holes',
      'mu*_phiLayer3Holes',
      'mu*_phiLayer4Holes',
      'mu*_etaLayer1Holes',
      'mu*_etaLayer2Holes',
      'mu*_etaLayer3Holes',
      'mu*_etaLayer4Holes',
      'mu*_nCSCEtaHits',
      'mu*_nCSCEtaHoles',
      'mu*_nCSCPhiHits',
      'mu*_nCSCPhiHoles',
      'mu*_nCSCUnspoiledEtaHits',
      'mu*_nMDTHits',
      'mu*_nMDTHoles',
      'mu*_nRPCEtaHits',
      'mu*_nRPCEtaHoles',
      'mu*_nRPCPhiHits',
      'mu*_nRPCPhiHoles',
      'mu*_nTGCEtaHits',
      'mu*_nTGCEtaHoles',
      'mu*_nTGCPhiHits',
      'mu*_nTGCPhiHoles',
      'mu*_nTRTOutliers',
      'mu*_nPixHits',
      'mu*_nTRTDeadStraws',
      'mu*_ntrigEtaLayers',
      'mu*_nBLHits',
      'mu*_nphiLayers',
      'mu*_nprecisionLayers',
      'mu*_nTRTHits',
      'mu*_type',
      'mu*_origin',
      'mu*_truth_eta',
      'mu*_truth_phi',
      'mu*_truth_dr',
      'mu*_truth_pt',
      'mu*_truth_matched',
      'mu*_truth_status',
      'mu*_truth_barcode',
      'mu*_truth_E',
      'mu*_truth_mothertype',
      'mu*_truth_motherbarcode',
      'mu*_beta',
      'mu*_matchchi2',
      'mu*_matchndof',
      'mu*_isMuonLikelihood',
      'mu*_trackIPEstimate_*unbiased',
      'mu*_track*unbiased',
      'mu*_loose',
      'mu*_medium',
      'mu*_tight',
      'mu*_truth_mothercharge',
      'mu*_truth_motherstatus',
      'mu*_truth_charge',
      'mu*_mc_index',
      'mu*_dr',
      'mu*_numberOfSegments',
      'vxp_ndof',
      'vxp_py',
      'vxp_trk_weight',
      'vxp_trk_z0',
      'vxp_trk_d0',
      'vxp_trk_unbiased_z0',
      'vxp_trk_unbiased_d0',
      'vxp_trk_err_unbiased_z0',
      'vxp_trk_err_unbiased_d0',
      'vxp_trk_qoverp',
      'vxp_trk_chi2',
      'muonTruth_m',
      'muonTruth_status',
      'MET_*',
      'METJetsInfo_*',
      'met_*',
      'cl_MET_*',
      'tau_MET_*',
      'trk_MET_*',
      'jet_*_MET_*',
      'el*_MET_*',
      'ph*_MET_*',
      'mu*_MET_*',
      'trig_HLT_*',
      #'el_truth_*',
      'mcevt_alphaQCD',
      'mcevt_alphaQED',
      'mcevt_event_scale',
      #'el_type',
      'trig_L2_emcl_energyInSample',
      'trig_L2_trk_idscan_*',
      'trig_L2_trk_sitrack_*',
      'trig_EF_emcl*_avg_lar_q',
      'trig_EF_emcl*_avg_tile_q',
      'trig_EF_emcl*_badlarq_frac',
      'trig_EF_emcl*_deltaPhi',
      'trig_EF_emcl*_deltaTheta',
      'trig_EF_emcl*_eng_pos',
      'trig_EF_emcl*_nvertexfraction',
      'trig_EF_emcl*_vertexfraction',
      'trig_EF_emcl*_significance',
      'trig_EF_el_EF_*',
      'trig_EF_ph_EF_*',
      'trig_L1_mu_thrValue',
      'trig_L1_mu_pt',
      'ph_Etcone15',
      'ph_Etcone25',
      'ph_Etcone35',
      'ph_Etcone15_pt_corrected',
      'ph_Etcone25_pt_corrected',
      'ph_Etcone35_pt_corrected',
      'trig_EF_trigmuonef_*',
      'trig_EF_el_Etcone15',
      'trig_EF_el_Etcone25',
      'trig_EF_el_Etcone35',
      'trig_EF_ph_Etcone15',
      'trig_EF_ph_Etcone25',
      'trig_EF_ph_Etcone35',
      'trig_L1_mu_*',
      'egtruth_etaCalo',
      'egtruth_phiCalo',
      'ph_topoEM*',
      'trig_EF_el_m',
      'trig_EF_el_pt',
      'trig_EF_el_Et',
      'trig_EF_el_px',
      'trig_EF_el_py',
      'trig_EF_el_pz',
      'trig_EF_el_eta',
      'trig_EF_el_phi',
      'trig_EF_el_ptcone20',
      'el_m',
      'el_E',
      'el_Et',
      'el_px',
      'el_py',
      'el_pz',
      'el_pt',
      'el_eta',
      'el_phi',
      'el_jet_dr',
      'ph_nucone*',
      'mu_nucone*',
      'mu_etconeNoEm*',
      'ph_Etcone20_pt_corrected',
      'ph_Etcone30_pt_corrected',
      'ph_Etcone40_pt_corrected',
      'trig_L1_emtau_thrPattern',
      'mcevt_pdf_scale',
      
      # fwd eles?
      'el*_f3',
      'el*_f3core',
      'el*_E233',
      'el*_E277',
      'el*_E237',
      'el*_weta2',
      'el*_Ethad',
      'el*_Ethad1',
      'el*_depth',
      'el*_zvertex',
      'el*_errz',
      'el*_etap',
      'el*_f1core',
      'el*_f1',
      'el*_Emins1',
      'el*_fside',
      'el*_Emax2',
      'el*_ws3',
      'el*_wstot',
      'el*_emaxs1',
      'el*_Etcone40_pt_corrected',
      'el*_Etcone35_pt_corrected',
      'el*_Etcone30_pt_corrected',
      'el*_Etcone25_pt_corrected',
      'el*_Etcone20_pt_corrected',
      'el*_Etcone15_pt_corrected',
      'el*_nucone20',
      'el*_nucone30',
      'el*_nucone40',
      'el*_ptcone20',
      'el*_ptcone30',
      'el*_ptcone40',
      'el*_Etcone15',
      'el*_Etcone20',
      'el*_Etcone25',
      'el*_Etcone30',
      'el*_Etcone35',
      'el*_Etcone40',
      'el*_Etcone45',
      'el*_pos7',
      'el*_deltaeta1',
      'el*_deltaeta2',
      'el*_deltaphi2',
      'el*_deltaphiRescaled',
      'el*_rphiallcalo',
      'el*_topoEtcone20',
      'el*_topoEtcone30',
      'el*_topoEtcone40',
      'el*_deltaEs',
      'el*_deltaEmax2',
      'trig_EF_el_Etcone45',
         ]),
     
    # Changed in devval.
    ('17.2',
     ['StreamRDO_ref',
      'StreamESD_ref',
      ]),
    
    # Changed by TriggerMenuPython in devval.
    ('17.2',
    ['trig_RoI_*',
     'trig_Nav_*']),
    
    # Changed in devval.
    ('17.2',
     ['trk_blayerPrediction_*',
      ]),
    # This also differs between 32/64 bits.
    # For now, we're generating reference files on 32-bits, so ignore
    # on 64-bits.
    ('-', 'x86_64',
     ['trk_blayerPrediction_*',
      ]),

    # TauDiscriminant changes not in AtlasProduction.
    ('-', atlasProduction,
     ['tau_*',
      ]),

    # MissingET-03-03-41 not in AtlasProduction.
    # Newer version in devval.
    ('-', atlasProduction, '/dev', '/18', '17.7', '17.X', '/mig0', 'localbuild',
     ['MET_RefFinal_STVF_*',
      ]),


    # Changed in TauDiscriminant-01-07-28 in devval.
    ('17.2',
     ['tau_EleBDTTight',
      ]),

    # Changed in devval and in 17.2.9.
    ('17.2.9',
     ['mu*_SpaceTime_*',
      ]),

    # Changed in devval --- TMVA change in root 34?
    ('17.2',
     ['ph_NN_passes',
      'ph_NN_discriminant',
      ]),

    # Changed by CaloClusterCorrection-00-07-74 in devval.
    ('17.2',
     ['*_rawcl_pt',
      '*_rawcl_eta',
      ]),
    
    # TriggerD3PDMaker-00-01-99, TrigAnalysisInterfaces-00-02-00,
    # TrigBunchCrossingTool-00-03-10, TriggerMenuNtuple-00-01-47
    # in AtlasPhysics but not AtlasProduction...
    ('-', atlasProduction,
     ['UnpairedBunches',
      'UnpairedBunchIntensities',
      'BunchIntensities',
      'FilledBunchIntBeam1',
      'FilledBunchIntBeam2',
      'UnpairedBeam1',
      'UnpairedBeam2',
      'UnpairedIntBeam1',
      'UnpairedIntBeam2',
      'trig_RoI_*',
      'trig_Nav_*',
      ]),

    # Changed in devval.
    ('17.2',
     ['StreamRAW_ref']),

    # Jet changes, not in AtlasProduction.
    ('-', atlasProduction,
     ['jet_*ActiveArea*',
      'jet_*_jvtx_x',
      'jet_*_jvtx_y',
      'jet_*_jvtx_z',
      'jet_*_jvtxfFull',
      'jet_*_jvtxf',
      'jet_*_nTrk',
      'jet_*_nTrk_pv0_500MeV',
      'jet_*_sumPtTrk',
      'jet_*_sumPtTrk_pv0_500MeV',
      'jet_*_flavor_component_ip3d_pc',
      'jet_*_flavor_component_sv1_pc',
      'jet_OriginIndex',
      'jet_*TruthMF',
      'jet_*TruthMFindex',
      'jet_*CENTER_LAMBDA',
      'jet_*SECOND_LAMBDA',
      'jet_*SECOND_R',
      'jet_*CLUSTER_PT',
      'jet_antikt4truth_isBad*',
      'jet_antikt4h1tower_isBad*',
      'jet_antikt4h1topo_isBad*',
      'jet_antikt6h1tower_isBad*',
      ]),

    # Changed by TrkGeometry-00-23-05 in devval.
    ('17.2',
     ['*_materialTraversed',
      ]),

    # Jet changes in devval.
    ('17.2',
     ['jet_*_ActiveArea*',
      'jet_*_TruthMFindex',
      'jet_*_TruthMF',
      'jet_*_jvtxf',
      'jet_*_jvtxfFull',
      'jet_*_isBad*',
      'jet_*_nTrk',
      'jet_*_sumPtTrk',
      'jet_*_jvtx_x',
      'jet_*_jvtx_y',
      'jet_*_jvtx_z',
      'jet_*CENTER_LAMBDA',
      'jet_*SECOND_LAMBDA',
      'jet_*SECOND_R',
      'jet_*CLUSTER_PT',
      ]),

    # Pool changes in devval.
    ('17.2',
     ['Token',
      ]),

    # Change by det descr bugfix in devval.
    ('17.2',
     ['ph_rawcl_etas1']),

    # Changed by TrkExTools-02-37-82 in devval.
    ('17.2',
     ['mu_curvatureSig',
      'mu_id_curvatureSig']),

    # Changed in devval.
    ('17.2',
     ['mu_*neighbourSig']),

    # Changed in devval.
    ('17.2',
     ['trig_L2_passedThrough',
      'trig_L2_resurrected',
      'trig_L2_passedPhysics',
      'trig_L2_passedRaw',
      'trig_EF_passedThrough',
      'trig_EF_resurrected',
      'trig_EF_passedPhysics',
      'trig_EF_passedRaw',
      'HLTRerunPrescaleMap',
      ]),

    # Changed in devval.
    ('17.2',
     ['trig_roidescriptor_forID_etaMinus',
      'trig_roidescriptor_forID_etaPlus',
      ]),

    # Changed in devval.
    ('17.2',
     ['el_jettrack_*',
      'el_trackd0pvunbiased',
      'el_tracksigd0pvunbiased',
      'el_trackz0pvunbiased',
      'el_tracksigz0pvunbiased',
      'el_trackIPEstimate*',
      'trk_*_wrtBL',
      'mu*_spec_surf_*',
      ]),
    ]
    

_ignore_keys = [
    # temp
    ('17.2',
     ['*Meta/ElectronD3PDObject*',
      '*Meta/MissingETCompositionD3PDObject*',
      '*Meta/JetD3PDObject*',
      '*Meta/EMTauROID3PDObject*',
      '*Meta/GenEventD3PDObject*',
      '*Meta/RefFinalMETD3PDObject*',
      '*Meta/TopoMETD3PDObject*',
      '*Meta/DBKeysD3PDObject*',
      '*Meta/TrigDecisionD3PDObject*',
      '*Meta/TrigInDetTrackD3PDObject*',
      '*Meta/TrigMuonEFInfoD3PDObject*',
      '*Meta/TrigPhotonD3PDObject*',
      '*Meta/TruthMuonD3PDObject*',
      '*Meta/egammaTruthD3PDObject*',
      '*Meta/MissingETTruthD3PDObject*',
      '*Meta/MuonD3PDObject*',
      '*Meta/MuonROID3PDObject*',
      '*Meta/TrigDBKeysD3PDObject*',
      '*Meta/TrigEMClusterD3PDObject*',
      '*Meta/TrigElectronD3PDObject*',
      '*Meta/TrigMETD3PDObject*',
      '*Meta/TruthParticleD3PDObject*',
      '*Meta/CombLinksD3PDObject*',
      '*Meta/CollisionDecisionD3PDObject*',
      '*Meta/PhotonD3PDObject*',
      '*Meta/PrimaryVertexD3PDObject*',
      '*Meta/TileMuFeatureD3PDObject*',
      '*Meta/TileTrackMuFeatureD3PDObject*',
      '*Meta/TrigMuonEFIsolationD3PDObject*',
      '*Meta/triggerBitsD3PDObject*',
      '*Meta/ChainEntryD3PDObject*',
      '*Meta/ClusterD3PDObject*',
      '*Meta/BGCodeD3PDObject*',
      '*Meta/CorrectionClusterD3PDObject*',
      '*Meta/EFElectronD3PDObject*',
      '*Meta/EFPhotonD3PDObject*',
      '*Meta/JetsInfoMETD3PDObject*',
      '*Meta/MBTSD3PDObject*',
      '*Meta/MBTSTimeD3PDObject*',
      '*Meta/MBTSTriggerBitsD3PDObject*',
      '*Meta/MuonFeatureD3PDObject*',
      '*Meta/MuonFeatureDetailsD3PDObject*',
      '*Meta/CombinedMuonFeatureD3PDObject*',
      '*Meta/EmTauROID3PDObject*',
      '*Meta/EnergySumD3PDObject*',
      '*Meta/METD3PDObject*',
      '*Meta/TrigRoiDescD3PDObject*',
      '*Meta/BunchConfigIDD3PDObject*',
      '*Meta/IsoMuonFeatureD3PDObject*',
      '*Meta/MissingETGoodnessD3PDObject*',
      '*Meta/EnergySumROID3PDObject*',
      '*Meta/EventInfoD3PDObject*',
      '*Meta/CompositeParticle*',
      '*Meta/BunchConfTree',
      '*Meta/TrigConfTree',
      'egammaTrigDec',
      'egammaTrigDecMeta/*',
      'trigtest',
      'trigtestMeta/*',
      'Lumi/egamma'
      ]),
    
    # TauDiscriminant-01-07-19 / TauD3PDMaker-01-08-19 not in 17.2.X.Y.
    ('-', atlasProduction, 
     ['*Meta/TauD3PDObject_*']),


    # JetTagD3PDMaker-00-01-89 not in AtlasProduction.
    ('-', atlasProduction, 
     ['*Meta/JetD3PDObject_*']),


    # Types change when moving to 64-bit ids.
    ('17.2',
     ['trackMeta/PixelClusterD3PDObject_*']),
    ]


def compile_list (ignore_list):
    for vlist in ignore_list:
        l = vlist[-1]
        for i in range(len(l)):
            l[i] = re.compile (fnmatch.translate (l[i]))
    return


compile_list(_ignore_branches)
compile_list(_ignore_keys)


def get_ver():
    ver = os.environ.get ('CMTPATH')
    if ver:
        ver = string.split (ver, ':')
        ver = [s for s in ver if s.find('AtlasSetup') < 0]
        ver.append (os.environ.get ('CMTCONFIG'))
        ver = string.join (ver, ':')
    else:
        ver = 'localbuild'
    return ver
    

def ignore_p (match, ignore_list):
    ver = get_ver()

    for vlist in ignore_list:
        vv = vlist[:-1]
        ignore = True
        if vv and vv[0] == '-':
            ignore = False
            vv = vv[1:]
        for v in vv:
            if (callable(v) and v(ver) or
                not callable(v) and ver and ver.find (v) >= 0):
                ignore = not ignore
                break
        if not ignore: continue
        for b in vlist[-1]:
            if b.match (match):
                return True
    return False


# Should a branch be ignored?
def ignore_br_p (br):
    return ignore_p (br, _ignore_branches)


def ignore_key_p (key):
    return ignore_p (key, _ignore_keys)


def read_val (val):
    if not val: return val

    if val.find ('+') >= 0:
      psplit = split_list1 (val, '+')
      if len(psplit) > 1:
      #psplit = split_list1 (val, '+')
      #if len(psplit) > 1:
        good = True
        for i in range(len(psplit)):
            psplit[i] = read_val (psplit[i])
            if not isinstance(psplit[i], list):
                good = False
                break
        if good:
            return reduce (operator.__add__, psplit)

    if val[-1] == 'L':
        try:
            return int(val)
        except TypeError:
            pass

    if val[0] in '0123456789-':
        if val.find ('.') < 0:
            try:
                return int(val)
            except TypeError:
                pass
            except ValueError:
                pass

        try:
            return float (val)
        except TypeError:
            pass
        except ValueError:
            pass

    try:
        (count, elt) = val.split ('*', 1)
        count = int (count)
        if elt.startswith('[') and elt.endswith(']'):
            return count * [read_val(elt[1:-1])]
    except ValueError:
        pass

    if val[0] == '[':
        return [read_val(s) for s in split_list(val)]
    return val


class Reader (object):
    def __init__ (self, f):
        self.saved = None
        self.f = f
        return


    def getline (self):
        if self.saved is not None:
            out = self.saved
            self.saved = None
            return out
        l = self.f.readline()
        if not l:
            return None
        if len(l) > 0 and l[-1] == '\n':
            l = l[:-1]
        return l


    def ungetline (self, l):
        assert self.saved is None
        self.saved = l
        return


class Tree (object):
    def __init__ (self, reader):
        self.dummy = False
        self.reader = reader
        self.branches = {}
        self.read()
        return


    def read (self):
        while True:
            l = self.reader.getline()
            if l is None: break
            if not l: continue
            if l == '[Dummy tree skipped]':
                self.dummy = True
                break
            if l.startswith ('Branches'):
                l = self.reader.getline()
                if not l.startswith ('-------'):
                    self.reader.ungetline (l)
                    break
                self.read_by_branches()
                return
                
            if not l.startswith ('Event '):
                self.reader.ungetline (l)
                break
            iev = int(l.split()[1])
            l = self.reader.getline()
            if not l.startswith ('-------'):
                self.reader.ungetline (l)
                break
            self.read_branches (iev)
        return


    def read_by_branches (self):
        lasttag = ''
        while True:
            l = self.reader.getline()
            if not l: break
            if l.startswith (':'):
                lasttag = l[1:]
                continue
            (name, val) = l.split (None, 1)
            if name == '$': name = ''
            name = lasttag + name
            br = self.branches.setdefault (name, {})
            val = read_val (val)
            for (iev, v) in enumerate(val):
                br[iev] = v
        return


    def read_branches (self, iev):
        while True:
            l = self.reader.getline()
            if not l: break
            (name, val) = l.split (None, 1)
            br = self.branches.setdefault (name, {})
            br[iev] = read_val (val)
        return



class String (object):
    def __init__ (self, reader):
        self.val = ''
        while True:
            l = reader.getline()
            if l is None: break
            ipos = l.find ('__END_OF_STRING__')
            if ipos >= 0:
                self.val = self.val + l[:ipos]
                break
            else:
                self.val = self.val + l + '\n'
        return


class Hist (object):
    def __init__ (self, reader):
        l = reader.getline()
        self.bins = []
        self.errs = []
        if l.startswith ('bins:'):
            self.bins = read_val (string.strip (l[5:]))
            l = reader.getline()
            if l.startswith ('errs:'):
                self.errs = read_val (string.strip (l[5:]))
        return


class Dumpreader (object):
    def __init__ (self, f):
        self.reader = Reader (f)
        self.keys = {}
        self.read()
        return


    def read (self):
        while True:
            l = self.reader.getline()
            if l is None: break
            if not l: continue
            if l.find ('wrong interface id') >= 0: continue
            ll = l.split()
            if len(ll) != 2:
                print ('Unknown line', l)
                break
            (typ, key) = ll
            if typ == 'Tree':
                self.keys[key] = Tree (self.reader)
            elif typ == 'String':
                self.keys[key] = String (self.reader)
            elif typ == 'TH1':
                self.keys[key] = Hist (self.reader)
            else:
                print ('Unknown type', typ)
                break


    def read_tree (self):
        return None


def dictkey_diff (d1, d2, msg, filter = None):
    keys = [k for k in d1.keys() if k not in d2]
    if filter: keys = [k for k in keys if not filter(k)]
    if keys:
        keys.sort()
        print (msg)
        for k in keys:
            print ('  ', k)
    return


def apply_renames (d1, d2, renames):
    for (knew, kold) in renames.items():
        if (d2.has_key (kold) and not d2.has_key (knew) and
            not d1.has_key (kold) and d1.has_key (knew)):
            d2[knew] = d2[kold]
            del d2[kold]
    return


def diff_string (k, s1, s2):
    if s1.val != s2.val:
        print ('String', k, 'has value', s1.val,
               'in new file but value', s2.val, 'in reference file')
    return


inttypes = [types.IntType, types.LongType]
def compare (o1, o2, thresh = 1e-6, ithresh = None, eltcmp = None):
    if eltcmp and not isinstance(o1, list):
        return eltcmp (o1, o2)
    # Allow comparing int/long int.
    if type(o1) in inttypes and type(o2) in inttypes:
        if o1 < 0: o1 = o1 + (1<<32)
        if o2 < 0: o2 = o2 + (1<<32)
        return o1 == o2
    # Promote int to float if needed.
    if type(o1) in inttypes and isinstance(o2, float):
        o1 = float(o1)
    elif type(o2) in inttypes and isinstance(o1, float):
        o2 = float(o2)

    if type(o1) != type(o2):
        return False
    if isinstance(o1, list):
        if len(o1) != len(o2):
            return False

        # Inline this to speed up this common case.
        if len(o1) > 0 and isinstance(o1[0], float) and not ithresh and not callable(thresh) and not eltcmp:
            xabs = abs
            for i in range(len(o1)):
                x1 = o1[i]
                x2 = o2[i]
                ax1 = xabs(x1)
                ax2 = xabs(x2)
                num = x1-x2
                den = ax1 + ax2
                if den == 0: continue
                x = xabs(num / den)
                if x > thresh:
                    print ('fnmismatch', x1, x2, x, thresh)
                    return False
            return True

        for i in range(len(o1)):
            if not compare (o1[i], o2[i],
                            thresh=thresh,
                            ithresh=ithresh,
                            eltcmp=eltcmp):
                return False
        return True
    if type(o1).__name__ in ['map<string,int>',
                             'map<string,float>',
                             'map<string,string>']:
        return ROOT.D3PDTest.MapDumper.equal (o1, o2)
    if isinstance(o1, float):
        if callable(ithresh):
            ret = ithresh(o1, o2)
            if ret is True or ret is False: return ret
        elif ithresh and abs(o1) < ithresh and abs(o2) < ithresh:
            return True
        num = o1-o2
        den = abs(o1)+abs(o2)
        if den == 0: return True
        x = abs(num / den)
        if callable(thresh): thresh = thresh(den)
        if x > thresh:
            print ('fmismatch', o1, o2, x, thresh)
            return False
        return True
    return o1 == o2


def mc_eta_thresh (x):
    if x > 36: return 1e-2
    if x > 34: return 2e-3
    if x > 32: return 2e-4
    if x > 30: return 1e-4
    if x > 28: return 1e-5
    return 1e-6


def eg_pid_compare (o1, o2):
    if o1 == 0xffffffff and o2 == 0: return True
    return None


def compare_detElementId (o1, o2):
    if o1 == o2: return True
    if o1 == (o2 << 32): return True
    return False


def compare_pixid (o1, o2):
    if o1 == o2: return True
    o2a = ((o2&0x7fffffff)-1)<<28
    if o1 == o2a : return True
    return False


def compare_abs (o1, o2):
    return compare (abs(o1), abs(o2))


branch_thresh = [

    ['mu*_cov_theta_qoverp_exPV'       , (2e-2, 2e-14)],
    ['mu_cov_phi_exPV'                 , 4e-4],
    ['mu*_cov_*_exPV'                  , 1e-2],
    ['mu*_d0_exPV'                     , 5e-3],
    ['mu*_z0_exPV'                     , 2e-5],
    ['mu*_theta_exPV'                  , 1e-5],
    ['mu*_spec_surf_*'                 , 6e-2],
    ['mu*_trackz0pv'                   , 2e-6],
    ['mu*_trackd0pv'                   , 1e-5],
    ['mu*_trackz0pvunbiased'           , 2e-6],
    ['mu*_trackd0pvunbiased'           , 1e-5],
    ['mu*_trackd0beam'                 , 2e-5],
    ['mu*_trackIPEstimate_d0_unbiasedpvunbiased', 1e-5],
    ['mu*_trackIPEstimate_z0_unbiasedpvunbiased', 1e-5],
    ['mu*_trackIPEstimate_d0_biasedpvunbiased', 1e-5],
    ['mu*_trackIPEstimate_z0_biasedpvunbiased', 1e-5],

    ['trk_z0_wrtPV'                    , 2e-4],
    ['trk_d0_wrtPV'                    , 2e-4],
    ['trk_phi_wrtPV'                   , 1e-4],

    ['trk_err_theta_wrtPV'             , 1e-5],
    ['trk_err_phi_wrtPV'               , 2e-5],

    ['trk_phi_wrtBL'                   , 2e-2],
    ['trk_z0_wrtBL'                    , 1],
    ['trk_d0_wrtBL'                    , 3e-1],

    ['trk_theta_err_wrtBL'             , 3e-2],
    ['trk_qoverp_err_wrtBL'            , 1e-4],
    ['trk_phi_err_wrtBL'               , 3e-2],
    ['trk_z0_err_wrtBL'                , 6e-2],
    ['trk_d0_err_wrtBL'                , 7e-2],

    ['trk_d0_phi_err_wrtBL'            , 1e-1],
    ['trk_d0_qoverp_err_wrtBL'         , 4e-3],
    ['trk_d0_theta_err_wrtBL'          , 1],
    ['trk_d0_z0_err_wrtBL'             , 1],
    ['trk_phi_theta_err_wrtBL'         , 1],
    ['trk_phi_qoverp_err_wrtBL'        , 4e-3],
    ['trk_theta_qoverp_err_wrtBL'      , 1],
    ['trk_z0_phi_err_wrtBL'            , 1],
    ['trk_z0_qoverp_err_wrtBL'         , 1],
    ['trk_z0_theta_err_wrtBL'          , 8e-2],

    ['trk_d0_wrtBS'                    , 2e-4],
    ['trk_z0_wrtBS'                    , 5e-5],
    ['trk_phi_wrtBS'                   , 5e-5],

    ['trk_err_theta_wrtBS'             , 1e-5],
    ['trk_err_phi_wrtBS'               , 2e-5],

    ['trk_cov_theta_qoverp_wrtBS'      , (4e-1, 1e-14)],
    ['trk_cov_phi_theta_wrtBS'         , 5e-2],
    ['trk_cov_phi_qoverp_wrtBS'        , 4e-5],
    ['trk_cov_z0_qoverp_wrtBS'         , 5e-3],
    ['trk_cov_z0_phi_wrtBS'            , 2e-3],
    ['trk_cov_z0_theta_wrtBS'          , 1e-5],
    ['trk_cov_d0_phi_wrtBS'            , 1e-5],
    ['trk_cov_d0_z0_wrtBS'             , 3e-1],
    ['trk_cov_d0_theta_wrtBS'          , 6e-3],

    ['trk_mc_z0'                       , 4e-2],
    ['trk_mc_d0'                       , 2e-2],
    ['trk_mc_phi'                      , 2e-2],
    ['trk_mc_theta'                    , 1e-3],

    ['vxp_trk_d0'                      , (9e-4, 2e-4)],
    ['vxp_trk_z0'                      , 9e-4],
    ['vxp_trk_phi'                     , 5e-5],
    ['vxp_trk_theta'                   , 2e-5],
    ['vxp_trk_unbiased_d0'             , (1e-3, 2e-4)],
    ['vxp_trk_unbiased_z0'             , 1e-3],

    ['el*_jettrack_m'                   , 6e-6],
    ['el*_jettrack_reducedPt'           , 2e-6],
    ['el*_jettrack_ptrel'               , 1e-5],
    ['el*_jettrack_dr'                  , 1e-5],
    ['el*_jettrack_phi'                 , 1e-5],

    ['el*_jetcone_dr'                   , 1e-5],
    ['el*_jetcone_ptrel'                , 2e-5],

    ['el*_refittedTrack_d0_err_wrtBL'   , 1e-4],
    ['el*_refittedTrack_phi_err_wrtBL'  , 3e-3],
    ['el*_refittedTrack_theta_err_wrtBL' , 4e-5],
    ['el*_refittedTrack_phi_theta_err_wrtBL', (2e-2, 1e-10)],
    ['el*_refittedTrack_phi_qoverp_err_wrtBL', 5e-3],
    ['el*_refittedTrack_theta_qoverp_err_wrtBL', 1],
    ['el*_refittedTrack_z0_theta_err_wrtBL', 3e-4],
    ['el*_refittedTrack_z0_qoverp_err_wrtBL', 6e-3],
    ['el*_refittedTrack_z0_phi_err_wrtBL', 9e-3],
    ['el*_refittedTrack_d0_wrtBL'       , 6e-4],
    ['el*_refittedTrack_d0_phi_err_wrtBL', 3e-3],
    ['el*_refittedTrack_d0_z0_err_wrtBL', 5e-4],
    ['el*_refittedTrack_d0_theta_err_wrtBL', 6e-2],
    ['el*_refittedTrack_d0_qoverp_err_wrtBL', 6e-4],
    ['el*_tracksigz0pvunbiased'         , 1e-5],
    ['el*_trackz0pvunbiased'            , 7e-4],
    ['el*_trackd0pvunbiased'            , 2e-4],
    ['el*_trackz0pv'                    , 6e-4],
    ['el*_trackd0pv'                    , 1e-4],
    ['el*_trackz0beam'                  , 4e-5],
    ['el*_trackd0beam'                  , 1e-3],
    ['el*_trackIPEstimate_z0_unbiasedpvunbiased', 1e-3],
    ['el*_trackIPEstimate_d0_unbiasedpvunbiased', 2e-4],
    ['el*_trackIPEstimate_sigz0_unbiasedpvunbiased', 1e-5],
    ['el*_trackIPEstimate_z0_biasedpvunbiased', 6e-4],
    ['el*_trackIPEstimate_d0_biasedpvunbiased', 1e-4],
    ['el*_trackIPEstimate_sigz0_biasedpvunbiased', 1e-5],
    ['el*_m',         (1e-4, 2e-3)],
    ['ph*_m',         (1e-4, 2e-3)],
    ['mu*_m',         (1e-4, 2e-3)],
    ['trig_EF_el*_m',         (1e-4, 2e-3)],
    ['trig_EF_ph*_m',         (1e-4, 2e-3)],

    ['*_cl_eta0Calo',      1e-3],
    ['*_cl_etaCalo',       1e-3],

    ['ph_vx_Dist'                      , (6e-4, 2e-2)],

    ['egtruth_phiCalo'                 , 3e-5],
    ['egtruth_etaCalo'                 , 6e-5],

    ]

for l in branch_thresh:
    l[0] = re.compile (fnmatch.translate (l[0]))


def diff_branch (k, kb, v1, v2):
    head = '%s branch %s:' % (k, kb)
    ev1 = v1.keys()
    ev2 = v2.keys()
    ev1.sort()
    ev2.sort()
    if ev1 != ev2:
        if ignore_event_differences:
            ev1 = list(set(ev1).intersection(set(ev2)))
            ev1.sort()
        else:
            print (head, 'Different set of events; new file: ',
                   ev1, 'reference file', ev2)
            return

    ithresh = None
    eltcmp = None

    thresh = 1e-6
    for (pat, th) in branch_thresh:
        if pat.match (kb):
            if isinstance(th, tuple):
                thresh = th[0]
                ithresh = th[1]
            else:
                thresh = th
            break

    # GSF reco run in d3pd job, so we have dbg/opt diffs.
    if kb.startswith ('el_gsf_'): thresh = 1e-4
    if kb == 'el_gsf_deltaeta2': thresh = 3e-3
    if kb == 'el_gsf_deltaphi2': thresh = 2e-1
    if kb == 'el_gsf_deltaphiRescaled': thresh = 2e-2
    if kb == 'el_gsf_deltaeta1': thresh = 3e-3
    if kb == 'el_gsf_trackz0pv': thresh = 1e-1
    if kb == 'el_gsf_covqoverp': thresh = 5e-2
    if kb == 'el_gsf_L2_dr': thresh = 3e-2
    if kb == 'el_gsf_EF_dr': thresh = 7e-2
    if kb == 'el_gsf_covd0theta': thresh = 3e-4
    if kb == 'el_gsf_electronweight': thresh = 2e-2
    if kb == 'el_gsf_electronbgweight': thresh = 3e-1
    if kb == 'el_gsf_adaboost': thresh = 7e-2
    if kb == 'el_gsf_neuralnet': thresh = 2e-1
    if kb == 'el_gsf_isolationlikelihoodhqelectrons': thresh = 4e-2
    if kb == 'el_gsf_jet_dr': thresh = 2e-2
    if kb == 'el_gsf_Et': thresh = 3e-3
    if kb == 'el_gsf_E': thresh = 4e-3
    if kb == 'sc_yCells': thresh = 2e-6
    if kb.startswith ('el_gsf_p'): thresh = 3e-3
    if kb.startswith ('el_gsf_refittedTrack_cov'): thresh = 3e-1
    if kb.startswith ('el_gsf_trackcov'): thresh = 5e-1
    if kb.startswith ('el_gsf_tracksig'): thresh = 4e-2
    if kb.startswith ('el_gsf_trackd0'): thresh = 6e-2
    if kb.startswith ('el_gsf_trackz0'): thresh = 3e-2
    if kb.startswith ('el_gsf_trackfitchi2'): thresh = 7e-3
    if kb.startswith ('el_gsf_trackqoverp'): thresh = 2e-2
    if kb.startswith ('el_gsf_trackpt'): thresh = 2e-2
    if kb.startswith ('el_gsf_refittedTrack_d0'): thresh = 3e-3
    if kb.startswith ('el_gsf_refittedTrack_LMqoverp'): thresh = 3e-3
    if kb.startswith ('el_gsf_refittedTrack_qoverp'): thresh = 2e-2
    if kb.startswith ('el_gsf_refittedTrack_covphitheta'): thresh = 5e-1
    if kb.startswith ('el_gsf_mvaptcone'): ithresh = 1e-10
    if kb == 'el_gsf_m':
        thresh = 2e-2
        ithresh = 0.01

    if kb.find('jet_')>=0 and kb.endswith ('_m'): ithresh = 0.1
    if kb.find('jetcone_')>=0 and kb.endswith ('_m'): ithresh = 1e-3
    if kb.find('muonTruth_')>=0 and kb.endswith ('_m'): ithresh = 0.5
    if kb == 'mc_m':
        ithresh = 0.1
        thresh = 2e-5
    if kb.endswith ('_effTopoInvMass'): thresh = 1e-2
    if kb.endswith ('_topoInvMass'): thresh = 1e-2
    if (kb.endswith ('_topoMeanDeltaR') or
        kb.endswith ('_effTopoMeanDeltaR')):
        ithresh = 1e-6
    if kb.find ('_rawcl_etas') >= 0: thresh = 2e-4
    if kb.endswith ('_convIP'): thresh = 4e-4
    if kb.endswith ('_convIPRev'): thresh = 6e-5
    if kb.endswith ('_emscale_E'): thresh = 9e-5
    if kb.endswith ('_emscale_eta'): thresh = 9e-5
    if kb.endswith ('_emscale_m'): ithresh = 0.1
    if kb.endswith ('_constscale_E'): thresh = 9e-5
    if kb.endswith ('_constscale_eta'): thresh = 9e-5
    if kb == 'mc_eta': thresh = mc_eta_thresh
    if kb.endswith ('_seg_locX'): ithresh = 2e-12
    if kb.endswith ('_seg_locY'): ithresh = 2e-12
    if kb == 'MET_Goodness_DeltaEt_JetAlgs_Jet': ithresh = 3e-11
    if kb == 'MET_Goodness_EEM_Jet': thresh = 2e-5
    if kb == 'MET_Goodness_HECf_Jet': thresh = 3e-6
    if kb.find ('_blayerPrediction') >= 0: thresh = 1e-2
    if kb.endswith ('_hecf'): thresh = 4e-6
    if kb.endswith ('_SpaceTime_t'): thresh = 4e-6
    if kb.endswith ('_SpaceTime_weight'): thresh = 2e-5
    if kb.endswith ('_SpaceTime_tError'): thresh = 8e-6
    if kb.endswith ('_calcVars_ChPiEMEOverCaloEME'): thresh = 1e-5
    if kb.endswith ('_vx_m'): ithresh = 1e-2
    if kb.endswith ('MET_Goodness_DeltaEt_JetAlgs_Jet'): ithresh = 1e-10

    # Diffs btn 32/64-bit
    if kb == 'mc_perigee_theta': thresh = 1
    if kb == 'mc_perigee_d0': thresh = 1
    if kb == 'mc_perigee_phi': thresh = 1 #(ev 2,9)
    if kb == 'mc_perigee_z0': thresh = 1
    if kb == 'tau_likelihood': thresh = 1e-5
    if kb == 'tau_SafeLikelihood': thresh = 1e-5
    if kb == 'el_gsf_softeweight': thresh = 1e-1
    if kb == 'el_gsf_softebgweight': thresh = 2e-2

    if kb.endswith ('_mvaptcone20'): ithresh = 1e-10
    if kb.endswith ('_mvaptcone30'): ithresh = 1e-10
    if kb.endswith ('_mvaptcone40'): ithresh = 1e-10

    # more 32/64 dbg/opt diffs
    if kb == 'tau_SafeLikelihood': thresh = 0.4
    if kb == 'tau_likelihood': thresh = 0.4
    if kb == 'tau_BDTJetScore': thresh = 1.0
    if kb == 'tau_calcVars_corrFTrk': thresh = 0.02
    if kb == 'tau_calcVars_corrCentFrac': thresh = 0.4
    if kb == 'tau_JetBDTSigLoose': thresh = 1.0
    if kb == 'tau_JetBDTSigMedium': thresh = 1.0
    if kb == 'tau_JetBDTSigTight': thresh = 1.0
    if kb == 'tau_tauLlhMedium': thresh = 1.0
    if kb == 'tau_pi0_cl1_pt': thresh = 1e-5

    if kb.endswith ('_neighbourSig'): thresh = 1e-4
    if kb.endswith ('_curvatureSig'): thresh = 1e-4

    if kb == 'pixClus_detElementId': eltcmp = compare_detElementId
    if kb == 'pixClus_id': eltcmp = compare_pixid
    if kb == 'trig_EF_emcl_pt': eltcmp = compare_abs

    for e in ev1:
        head = '%s branch %s event %d:' % (k, kb, e)
        b1 = v1[e]
        b2 = v2[e]

        # Promote int to float if needed.
        if type(b1) in inttypes and isinstance(b2, float):
            b1 = float(b1)
        elif type(b2) in inttypes and isinstance(b1, float):
            b2 = float(b2)

        if type(b1) != type(b2):
            print (head, 'Type differs; new file: ', type(b1).__name__,
                   'reference file:', type(b2).__name__)
        elif not compare (b1, b2, thresh = thresh, ithresh = ithresh,
                          eltcmp = eltcmp):
            print (head, 'Branch mismatch')
            print ('  new file:', b1)
            print ('  ref file:', b2)
        
    return

def diff_tree (k, t1, t2):
    if t1.dummy and t2.dummy: return
    apply_renames (t1.branches, t2.branches, renames)
    dictkey_diff (t1.branches, t2.branches, '%s: Branches only in new file:'%k,
                  ignore_br_p)
    dictkey_diff (t2.branches, t1.branches, '%s: Branches only in reference file:'%k,
                  ignore_br_p)
    for kb in t1.branches:
        if t2.branches.has_key (kb):
            if not ignore_br_p(kb):
                diff_branch (k, kb, t1.branches[kb], t2.branches[kb])
    return


def diff_hist (k, h1, h2):
    if not compare (h1.bins, h2.bins):
        print ('Hist', k, 'has bins', h1.bins)
        print ('in new file but bins', h2.bins)
        print ('in reference file')
    if not compare (h1.errs, h2.errs):
        print ('Hist', k, 'has errs', h1.errs)
        print ('in new file but errs', h2.errs)
        print ('in reference file')
    return


def diff_files (d1, d2):
    apply_renames (d1.keys, d2.keys, rename_keys)
    dictkey_diff (d1.keys, d2.keys, 'Keys only in new file:',
                  ignore_key_p)
    dictkey_diff (d2.keys, d1.keys, 'Keys only in reference file:',
                  ignore_key_p)
    for k in d1.keys.keys():
        if k in d2.keys and not ignore_key_p (k):
            v1 = d1.keys[k]
            v2 = d2.keys[k]
            if type(v1) != type(v2):
                print ('Key', k, 'has type', type(v1).__name__,
                       'in new file, but type', type(v2).__name__,
                       'in reference file')
            elif type(v1) == String:
                diff_string (k, v1, v2)
            elif type(v1) == Tree:
                diff_tree (k, v1, v2)
            elif type(v1) == Hist:
                diff_hist (k, v1, v2)
            else:
                print ('Unknown type for diff:', type(v1).__name__)
    return


if __name__ == '__main__':
    import sys
    import subprocess

    if sys.argv[1] == '--ignore-event-differences':
        ignore_event_differences = True
        del sys.argv[1]

    # New tuple
    p = subprocess.Popen ('python -m D3PDMakerTest.dumptuple_any %s' %
                            sys.argv[1],
                          shell = True,
                          stdout = subprocess.PIPE)
    d1 = Dumpreader (p.stdout)
    # Reference file
    d2 = Dumpreader (open (sys.argv[2]))

    diff_files (d1, d2)
    

