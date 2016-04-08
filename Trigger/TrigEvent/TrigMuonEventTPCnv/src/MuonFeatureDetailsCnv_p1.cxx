/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetails_p1.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p1.h"

void MuonFeatureDetailsCnv_p1::persToTrans(const MuonFeatureDetails_p1* persObj,
                                                 MuonFeatureDetails* transObj,
                                                 MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureDetailsCnv_p1::persToTrans called " << endreq;

   *transObj = MuonFeatureDetails (persObj->m_id,
                                   persObj->m_te_id,
                                   persObj->m_lvl1_id,
                                   persObj->m_lumi_block,
                                   persObj->m_muondetmask, 
                                   persObj->m_RoIID,
                                   persObj->m_RoISystem,
                                   persObj->m_RoISubsystem,
                                   persObj->m_RoISector,
                                   persObj->m_RoINumber,
                                   persObj->m_RoIThreshold,
                                   persObj->m_RoIEta,
                                   persObj->m_RoIPhi,
                                   -1, // m_extension_capacity
                                   -1, // m_max_rob_capacity
                                   -1, // m_max_csm_capacity
                                   -1, // m_max_lvl1_emu_capacity
                                   -1, // m_max_rpc_hits_capacity
                                   -1, // m_max_tgc_hits_capacity
                                   -1); // m_max_mdt_hits_capacity
    
   transObj->setError       (persObj->m_error);
   transObj->setRpcPadError (persObj->m_rpc_pad_error);
   transObj->setTgcPadError (persObj->m_tgc_rdo_error);
    
   transObj->setRpc1 (persObj->m_rpc1_x, persObj->m_rpc1_y, persObj->m_rpc1_z);
   transObj->setRpc2 (persObj->m_rpc2_x, persObj->m_rpc2_y, persObj->m_rpc2_z);
   transObj->setRpc3 (persObj->m_rpc3_x, persObj->m_rpc3_y, persObj->m_rpc3_z);

   transObj->setTgcMid1 (persObj->m_tgc_Mid1_eta,
                         persObj->m_tgc_Mid1_phi,
                         persObj->m_tgc_Mid1_r,
                         persObj->m_tgc_Mid1_z);
   transObj->setTgcMid2 (persObj->m_tgc_Mid2_eta,
                         persObj->m_tgc_Mid2_phi,
                         persObj->m_tgc_Mid2_r,
                         persObj->m_tgc_Mid2_z);
   transObj->setTgcMidF (persObj->m_tgc_Mid_rho_chi2,
                         persObj->m_tgc_Mid_rho_N,
                         persObj->m_tgc_Mid_phi_chi2,
                         persObj->m_tgc_Mid_phi_N);
   transObj->setTgcInn (persObj->m_tgc_Inn_eta,
                        persObj->m_tgc_Inn_phi,
                        persObj->m_tgc_Inn_r,
                        persObj->m_tgc_Inn_z);
   transObj->setTgcInnF (persObj->m_tgc_Inn_rho_std,
                         persObj->m_tgc_Inn_rho_N,
                         persObj->m_tgc_Inn_phi_std,
                         persObj->m_tgc_Inn_phi_N);
    
   transObj->setTgcPt (persObj->m_tgc_PT);
    
    transObj->setType           (persObj->m_type);
    transObj->setT_x_center     (persObj->m_t_x_center);
    transObj->setT_y_center     (persObj->m_t_y_center);
    transObj->setT_radius       (persObj->m_t_radius);
    transObj->setT_ap           (persObj->m_t_ap);
    transObj->setT_aphi         (persObj->m_t_aphi);
    transObj->setT_phi          (persObj->m_t_phi);
    transObj->setExtrInnerEta   (persObj->m_extrInnerEta);
    transObj->setExtrInnerPhi   (persObj->m_extrInnerPhi);
    
    transObj->setChamber_type_1 (persObj->m_chamber_type_1);
    transObj->setChamber_type_2 (persObj->m_chamber_type_2);
    transObj->setPos            (persObj->m_pos);
    transObj->setAw             (persObj->m_aw);
    transObj->setBw             (persObj->m_bw);
    transObj->setZetaMin        (persObj->m_zetaMin);
    transObj->setZetaMax        (persObj->m_zetaMax);
    transObj->setRadMin         (persObj->m_radMin);
    transObj->setRadMax         (persObj->m_radMax);
    transObj->setEtaMin         (persObj->m_etaMin);
    transObj->setEtaMax         (persObj->m_etaMax);
    transObj->setSt_phi         (persObj->m_st_phi);
    transObj->setSt_roads       (persObj->m_st_roads);

    transObj->setSP1 (persObj->m_mdt_Inner_slope,
                      persObj->m_mdt_Inner_intercept,
                      persObj->m_mdt_Inner_Z,
                      persObj->m_mdt_Inner_R,
                      persObj->m_mdt_Inner_fit_chi);
    transObj->setSP2 (persObj->m_mdt_Middle_slope,
                      persObj->m_mdt_Middle_intercept,
                      persObj->m_mdt_Middle_Z,
                      persObj->m_mdt_Middle_R,
                      persObj->m_mdt_Middle_fit_chi);
    transObj->setSP3 (persObj->m_mdt_Outer_slope,
                      persObj->m_mdt_Outer_intercept,
                      persObj->m_mdt_Outer_Z,
                      persObj->m_mdt_Outer_R,
                      persObj->m_mdt_Outer_fit_chi);

    transObj->setAddress      (persObj->m_Address);
    transObj->setSagitta      (persObj->m_Sagitta);
    transObj->setRadius       (persObj->m_Radius);
    transObj->setSlope        (persObj->m_Slope);
    transObj->setIntercept    (persObj->m_Intercept);
    transObj->setAlpha        (persObj->m_Alpha);
    transObj->setBeta         (persObj->m_Beta);
    transObj->setDeltaR       (persObj->m_DeltaR);
    transObj->setSpeed_over_c (persObj->m_Speed_over_c);
    transObj->setEta          (persObj->m_Eta);
    transObj->setPhiMap       (persObj->m_PhiMap);
    transObj->setPhi          (persObj->m_Phi);
    transObj->setPhiDir       (persObj->m_PhiDir);
    transObj->setPt           (persObj->m_Pt);
    transObj->setCharge       (persObj->m_Charge);
    
    transObj->setExtension0   (persObj->m_extension0);
    transObj->setExtension1   (persObj->m_extension1);
    transObj->setExtension2   (persObj->m_extension2);
    transObj->setExtension3   (persObj->m_extension3);
    transObj->setExtension4   (persObj->m_extension4);
    transObj->setExtension5   (persObj->m_extension5);
    transObj->setExtension6   (persObj->m_extension6);
    transObj->setExtension7   (persObj->m_extension7);
    transObj->setExtension8   (persObj->m_extension8);
    transObj->setExtension9   (persObj->m_extension9);
    
    transObj->setLvl1Emulation (persObj->m_lvl1_emulation);
    
    transObj->setRequestedRob     (persObj->m_rob_id);
    transObj->setConvertedCsm     (persObj->m_csm_id);
    transObj->setConvertedCsmSize (persObj->m_csm_size);
    transObj->setCsmError         (persObj->m_csm_error);
    transObj->setRemovedRob       (persObj->m_removed_rob_id);
    transObj->setRemovedCsm       (persObj->m_removed_csm_id);

    for (size_t i = 0; i < persObj->m_pad_hit_onlineId.size(); i++) {
      transObj->setPadHit (persObj->m_pad_hit_onlineId[i],
                           persObj->m_pad_hit_code[i],
                           persObj->m_pad_hit_x[i],
                           persObj->m_pad_hit_y[i],
                           persObj->m_pad_hit_z[i],
                           persObj->m_pad_hit_r[i],
                           persObj->m_pad_hit_p[i]);
    }

    for (size_t i = 0; i < persObj->m_tgc_Inn_rho_hit_phi.size(); i++) {
      transObj->setTgcInnRhoHit (persObj->m_tgc_Inn_rho_hit_phi[i],
                                 persObj->m_tgc_Inn_rho_hit_r[i],
                                 persObj->m_tgc_Inn_rho_hit_z[i],
                                 persObj->m_tgc_Inn_rho_hit_width[i],
                                 persObj->m_tgc_Inn_rho_hit_in_seg[i]);
    }

    for (size_t i = 0; i < persObj->m_tgc_Inn_phi_hit_phi.size(); i++) {
      transObj->setTgcInnPhiHit (persObj->m_tgc_Inn_phi_hit_phi[i],
                                 persObj->m_tgc_Inn_phi_hit_r[i],
                                 persObj->m_tgc_Inn_phi_hit_z[i],
                                 persObj->m_tgc_Inn_phi_hit_width[i],
                                 persObj->m_tgc_Inn_phi_hit_in_seg[i]);
    }

    for (size_t i = 0; i < persObj->m_tgc_Mid_rho_hit_phi.size(); i++) {
      transObj->setTgcMidRhoHit (persObj->m_tgc_Mid_rho_hit_phi[i],
                                 persObj->m_tgc_Mid_rho_hit_r[i],
                                 persObj->m_tgc_Mid_rho_hit_z[i],
                                 persObj->m_tgc_Mid_rho_hit_width[i],
                                 persObj->m_tgc_Mid_rho_hit_in_seg[i]);
    }

    for (size_t i = 0; i < persObj->m_tgc_Mid_phi_hit_phi.size(); i++) {
      transObj->setTgcMidPhiHit (persObj->m_tgc_Mid_phi_hit_phi[i],
                                 persObj->m_tgc_Mid_phi_hit_r[i],
                                 persObj->m_tgc_Mid_phi_hit_z[i],
                                 persObj->m_tgc_Mid_phi_hit_width[i],
                                 persObj->m_tgc_Mid_phi_hit_in_seg[i]);
    }

    for (size_t i = 0; i < persObj->m_mdt_onlineId.size(); i++) {
      transObj->setMdtHit (persObj->m_mdt_onlineId[i],
                           persObj->m_mdt_offlineId[i],
                           persObj->m_mdt_tube_r[i],
                           persObj->m_mdt_tube_z[i],
                           persObj->m_mdt_tube_residual[i],
                           persObj->m_mdt_tube_time[i],
                           persObj->m_mdt_tube_space[i],
                           persObj->m_mdt_tube_sigma[i]);
    }
}


void MuonFeatureDetailsCnv_p1::transToPers(const MuonFeatureDetails* transObj,
                                                 MuonFeatureDetails_p1* persObj,
                                                 MsgStream &log)
{
   log << MSG::DEBUG << "MuonFeatureDetailsCnv_p1::transToPers called " << endreq;
    
    persObj->m_id           = transObj->id();
    persObj->m_te_id        = transObj->te_id();
    persObj->m_error        = transObj->error();

    persObj->m_lvl1_id      = transObj->lvl1_id();
    persObj->m_lumi_block   = transObj->lumi_block();
    persObj->m_muondetmask  = transObj->muondetmask();
    persObj->m_RoIID        = transObj->roi_id();
    persObj->m_RoISystem    = transObj->roi_system();
    persObj->m_RoISubsystem = transObj->roi_subsystem();
    persObj->m_RoISector    = transObj->roi_sector();
    persObj->m_RoINumber    = transObj->roi_number();
    persObj->m_RoIThreshold = transObj->roi_threshold();

    persObj->m_RoIEta = transObj->roi_eta();
    persObj->m_RoIPhi = transObj->roi_phi();

    persObj->m_rpc_pad_error = transObj->rpc_pad_error();
    persObj->m_tgc_rdo_error = transObj->tgc_rdo_error();
    
    persObj->m_rpc1_x = transObj->rpc1_x();
    persObj->m_rpc1_y = transObj->rpc1_y();
    persObj->m_rpc1_z = transObj->rpc1_z();
     
    persObj->m_rpc2_x = transObj->rpc2_x();
    persObj->m_rpc2_y = transObj->rpc2_y();
    persObj->m_rpc2_z = transObj->rpc2_z();
    
    persObj->m_rpc3_x = transObj->rpc3_x();
    persObj->m_rpc3_y = transObj->rpc3_y();
    persObj->m_rpc3_z = transObj->rpc3_z();
    
    persObj->m_tgc_Mid1_eta      = transObj->tgc_Mid1_eta();
    persObj->m_tgc_Mid1_phi      = transObj->tgc_Mid1_phi();
    persObj->m_tgc_Mid1_r        = transObj->tgc_Mid1_r();
    persObj->m_tgc_Mid1_z        = transObj->tgc_Mid1_z();
    
    persObj->m_tgc_Mid2_eta      = transObj->tgc_Mid2_eta();
    persObj->m_tgc_Mid2_phi      = transObj->tgc_Mid2_phi();
    persObj->m_tgc_Mid2_r        = transObj->tgc_Mid2_r();
    persObj->m_tgc_Mid2_z        = transObj->tgc_Mid2_z();
    
    persObj->m_tgc_Mid_rho_chi2  = transObj->tgc_Mid_rho_chi2();
    persObj->m_tgc_Mid_rho_N     = transObj->tgc_Mid_rho_N();
    persObj->m_tgc_Mid_phi_chi2  = transObj->tgc_Mid_phi_chi2();
    persObj->m_tgc_Mid_phi_N     = transObj->tgc_Mid_phi_N();
    
    persObj->m_tgc_Inn_eta       = transObj->tgc_Inn_eta();
    persObj->m_tgc_Inn_phi       = transObj->tgc_Inn_phi();
    persObj->m_tgc_Inn_r         = transObj->tgc_Inn_r();
    persObj->m_tgc_Inn_z         = transObj->tgc_Inn_z();
    persObj->m_tgc_Inn_rho_std   = transObj->tgc_Inn_rho_std();
    persObj->m_tgc_Inn_rho_N     = transObj->tgc_Inn_rho_N();
    persObj->m_tgc_Inn_phi_std   = transObj->tgc_Inn_phi_std();
    persObj->m_tgc_Inn_phi_N     = transObj->tgc_Inn_phi_N();
    
    persObj->m_tgc_PT = transObj->tgc_PT();
    
    persObj->m_type           = transObj->type();
    persObj->m_t_x_center     = transObj->t_x_center();
    persObj->m_t_y_center     = transObj->t_y_center();
    persObj->m_t_radius       = transObj->t_radius();
    persObj->m_t_ap           = transObj->t_ap();
    persObj->m_t_aphi         = transObj->t_aphi();
    persObj->m_t_phi          = transObj->t_phi();
    persObj->m_extrInnerEta   = transObj->extrInnerEta();
    persObj->m_extrInnerPhi   = transObj->extrInnerPhi();
    
    persObj->m_chamber_type_1 = transObj->chamber_type_1();
    persObj->m_chamber_type_2 = transObj->chamber_type_2();
    persObj->m_pos            = transObj->pos();
    persObj->m_aw             = transObj->aw();
    persObj->m_bw             = transObj->bw();
    persObj->m_zetaMin        = transObj->zetaMin();
    persObj->m_zetaMax        = transObj->zetaMax();
    persObj->m_radMin         = transObj->radMin();
    persObj->m_radMax         = transObj->radMax();
    persObj->m_etaMin         = transObj->etaMin();
    persObj->m_etaMax         = transObj->etaMax();
    persObj->m_st_phi         = transObj->st_phi();
    persObj->m_st_roads       = transObj->st_roads();
    
    persObj->m_mdt_Inner_slope      = transObj->mdt_Inner_slope();
    persObj->m_mdt_Inner_intercept  = transObj->mdt_Inner_intercept();
    persObj->m_mdt_Inner_Z          = transObj->mdt_Inner_Z();
    persObj->m_mdt_Inner_R          = transObj->mdt_Inner_R();
    persObj->m_mdt_Inner_fit_chi    = transObj->mdt_Inner_fit_chi();
    persObj->m_mdt_Middle_slope     = transObj->mdt_Middle_slope();
    persObj->m_mdt_Middle_intercept = transObj->mdt_Middle_intercept();
    persObj->m_mdt_Middle_Z         = transObj->mdt_Middle_Z();
    persObj->m_mdt_Middle_R         = transObj->mdt_Middle_R();
    persObj->m_mdt_Middle_fit_chi   = transObj->mdt_Middle_fit_chi();
    persObj->m_mdt_Outer_slope      = transObj->mdt_Outer_slope();
    persObj->m_mdt_Outer_intercept  = transObj->mdt_Outer_intercept();
    persObj->m_mdt_Outer_Z          = transObj->mdt_Outer_Z();
    persObj->m_mdt_Outer_R          = transObj->mdt_Outer_R();
    persObj->m_mdt_Outer_fit_chi    = transObj->mdt_Outer_fit_chi();
    
    persObj->m_Address      = transObj->Address();
    persObj->m_Sagitta      = transObj->Sagitta();
    persObj->m_Radius       = transObj->Radius();
    persObj->m_Slope        = transObj->Slope();
    persObj->m_Intercept    = transObj->Intercept();
    persObj->m_Alpha        = transObj->Alpha();
    persObj->m_Beta         = transObj->Beta();
    persObj->m_DeltaR       = transObj->DeltaR();
    persObj->m_Speed_over_c = transObj->Speed_over_c();
    persObj->m_Eta          = transObj->Eta();
    persObj->m_PhiMap       = transObj->PhiMap();
    persObj->m_Phi          = transObj->Phi();
    persObj->m_PhiDir       = transObj->PhiDir();
    persObj->m_Pt           = transObj->Pt();
    persObj->m_Charge       = transObj->Charge();
    
    persObj->m_extension0   = transObj->extension0();
    persObj->m_extension1   = transObj->extension1();
    persObj->m_extension2   = transObj->extension2();
    persObj->m_extension3   = transObj->extension3();
    persObj->m_extension4   = transObj->extension4();
    persObj->m_extension5   = transObj->extension5();
    persObj->m_extension6   = transObj->extension6();
    persObj->m_extension7   = transObj->extension7();
    persObj->m_extension8   = transObj->extension8();
    persObj->m_extension9   = transObj->extension9();
    
    persObj->m_lvl1_emulation = transObj->lvl1_emulation();
    
    persObj->m_rob_id         = transObj->rob_id();
    persObj->m_csm_id         = transObj->csm_id();
    persObj->m_csm_size       = transObj->csm_size();
    persObj->m_csm_error      = transObj->csm_error();
    persObj->m_removed_rob_id = transObj->removed_rob_id();
    persObj->m_removed_csm_id = transObj->removed_csm_id();
    
    persObj->m_pad_hit_onlineId = transObj->pad_hit_onlineId();
    persObj->m_pad_hit_code     = transObj->pad_hit_code();
    persObj->m_pad_hit_x        = transObj->pad_hit_x();
    persObj->m_pad_hit_y        = transObj->pad_hit_y();
    persObj->m_pad_hit_z        = transObj->pad_hit_z();
    persObj->m_pad_hit_r        = transObj->pad_hit_r();
    persObj->m_pad_hit_p        = transObj->pad_hit_p();
    
    persObj->m_tgc_Inn_rho_hit_phi    = transObj->tgc_Inn_rho_hit_phi();
    persObj->m_tgc_Inn_rho_hit_r      = transObj->tgc_Inn_rho_hit_r();
    persObj->m_tgc_Inn_rho_hit_z      = transObj->tgc_Inn_rho_hit_z();
    persObj->m_tgc_Inn_rho_hit_width  = transObj->tgc_Inn_rho_hit_width();
    persObj->m_tgc_Inn_rho_hit_in_seg = transObj->tgc_Inn_rho_hit_in_seg();
    persObj->m_tgc_Inn_phi_hit_phi    = transObj->tgc_Inn_phi_hit_phi();
    persObj->m_tgc_Inn_phi_hit_r      = transObj->tgc_Inn_phi_hit_r();
    persObj->m_tgc_Inn_phi_hit_z      = transObj->tgc_Inn_phi_hit_z();
    persObj->m_tgc_Inn_phi_hit_width  = transObj->tgc_Inn_phi_hit_width();
    persObj->m_tgc_Inn_phi_hit_in_seg = transObj->tgc_Inn_phi_hit_in_seg();
    persObj->m_tgc_Mid_rho_hit_phi    = transObj->tgc_Mid_rho_hit_phi();
    persObj->m_tgc_Mid_rho_hit_r      = transObj->tgc_Mid_rho_hit_r();
    persObj->m_tgc_Mid_rho_hit_z      = transObj->tgc_Mid_rho_hit_z();
    persObj->m_tgc_Mid_rho_hit_width  = transObj->tgc_Mid_rho_hit_width();
    persObj->m_tgc_Mid_rho_hit_in_seg = transObj->tgc_Mid_rho_hit_in_seg();
    persObj->m_tgc_Mid_phi_hit_phi    = transObj->tgc_Mid_phi_hit_phi();
    persObj->m_tgc_Mid_phi_hit_r      = transObj->tgc_Mid_phi_hit_r();
    persObj->m_tgc_Mid_phi_hit_z      = transObj->tgc_Mid_phi_hit_z();
    persObj->m_tgc_Mid_phi_hit_width  = transObj->tgc_Mid_phi_hit_width();
    persObj->m_tgc_Mid_phi_hit_in_seg = transObj->tgc_Mid_phi_hit_in_seg();
    
    persObj->m_mdt_onlineId      = transObj->mdt_onlineId();
    persObj->m_mdt_offlineId     = transObj->mdt_offlineId();
    persObj->m_mdt_tube_r        = transObj->mdt_tube_r();
    persObj->m_mdt_tube_z        = transObj->mdt_tube_z();
    persObj->m_mdt_tube_residual = transObj->mdt_tube_residual();
    persObj->m_mdt_tube_time     = transObj->mdt_tube_time();
    persObj->m_mdt_tube_space    = transObj->mdt_tube_space();
    persObj->m_mdt_tube_sigma    = transObj->mdt_tube_sigma();
}

