/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define private public
#define protected public
#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetails_p2.h"
#undef private
#undef protected

#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p2.h"

void MuonFeatureDetailsCnv_p2::persToTrans(const MuonFeatureDetails_p2* persObj, MuonFeatureDetails* transObj, MsgStream &log) {
   log << MSG::DEBUG << "MuonFeatureDetailsCnv_p2::persToTrans called " << endreq;

    transObj->m_extension_capacity    = -1;
    transObj->m_max_rob_capacity      = -1;
    transObj->m_max_csm_capacity      = -1;
    transObj->m_max_lvl1_emu_capacity = -1;
    transObj->m_max_rpc_hits_capacity = -1;
    transObj->m_max_tgc_hits_capacity = -1;
    transObj->m_max_mdt_hits_capacity = -1;

    transObj->m_id                     = persObj->m_id                                      ;   
    transObj->m_Address                = persObj->m_Address                                 ;
                                                                                                                    
    transObj->m_te_id                  = persObj->m_allTheUInts[0]                          ;
    transObj->m_error                  = persObj->m_allTheUInts[1]                          ;
    transObj->m_lvl1_id                = persObj->m_allTheUInts[2]                          ;
    transObj->m_lumi_block             = persObj->m_allTheUInts[3]                          ;
    transObj->m_muondetmask            = persObj->m_allTheUInts[4]                          ;
    transObj->m_RoIID                  = persObj->m_allTheUInts[5]                          ;
    transObj->m_RoISystem              = persObj->m_allTheUInts[6]                          ;
    transObj->m_RoISubsystem           = persObj->m_allTheUInts[7]                          ;
    transObj->m_RoISector              = persObj->m_allTheUInts[8]                          ;
    transObj->m_RoINumber              = persObj->m_allTheUInts[9]                          ;
    transObj->m_RoIThreshold           = persObj->m_allTheUInts[10]                         ;
    transObj->m_rpc_pad_error          = persObj->m_allTheUInts[11]                         ;
    transObj->m_tgc_rdo_error          = persObj->m_allTheUInts[12]                         ;
                                                                                                                  
    transObj->m_tgc_Mid_rho_N          = persObj->m_allTheLongs[0]                          ;
    transObj->m_tgc_Mid_phi_N          = persObj->m_allTheLongs[1]                          ;
    transObj->m_tgc_Inn_rho_N          = persObj->m_allTheLongs[2]                          ;
    transObj->m_tgc_Inn_phi_N          = persObj->m_allTheLongs[3]                          ;
    transObj->m_type                   = persObj->m_allTheLongs[4]                          ;
                                                                                                             
    transObj->m_RoIEta                 = persObj->m_allTheFloats[0]                         ;
    transObj->m_RoIPhi                 = persObj->m_allTheFloats[1]                         ;
    transObj->m_rpc1_x                 = persObj->m_allTheFloats[2]                         ;
    transObj->m_rpc1_y                 = persObj->m_allTheFloats[3]                         ;
    transObj->m_rpc1_z                 = persObj->m_allTheFloats[4]                         ;
    transObj->m_rpc2_x                 = persObj->m_allTheFloats[5]                         ;
    transObj->m_rpc2_y                 = persObj->m_allTheFloats[6]                         ;
    transObj->m_rpc2_z                 = persObj->m_allTheFloats[7]                         ;
    transObj->m_rpc3_x                 = persObj->m_allTheFloats[8]                         ;
    transObj->m_rpc3_y                 = persObj->m_allTheFloats[9]                         ;
    transObj->m_rpc3_z                 = persObj->m_allTheFloats[10]                        ;
    transObj->m_tgc_Mid1_eta           = persObj->m_allTheFloats[11]                        ;
    transObj->m_tgc_Mid1_phi           = persObj->m_allTheFloats[12]                        ;
    transObj->m_tgc_Mid1_r             = persObj->m_allTheFloats[13]                        ;
    transObj->m_tgc_Mid1_z             = persObj->m_allTheFloats[14]                        ;
    transObj->m_tgc_Mid2_eta           = persObj->m_allTheFloats[15]                        ;
    transObj->m_tgc_Mid2_phi           = persObj->m_allTheFloats[16]                        ;
    transObj->m_tgc_Mid2_r             = persObj->m_allTheFloats[17]                        ;
    transObj->m_tgc_Mid2_z             = persObj->m_allTheFloats[18]                        ;
    transObj->m_tgc_Mid_rho_chi2       = persObj->m_allTheFloats[19]                        ;
    transObj->m_tgc_Mid_phi_chi2       = persObj->m_allTheFloats[20]                        ;
    transObj->m_tgc_Inn_eta            = persObj->m_allTheFloats[21]                        ;
    transObj->m_tgc_Inn_phi            = persObj->m_allTheFloats[22]                        ;
    transObj->m_tgc_Inn_r              = persObj->m_allTheFloats[23]                        ;
    transObj->m_tgc_Inn_z              = persObj->m_allTheFloats[24]                        ;
    transObj->m_tgc_Inn_rho_std        = persObj->m_allTheFloats[25]                        ;
    transObj->m_tgc_Inn_phi_std        = persObj->m_allTheFloats[26]                        ;
    transObj->m_tgc_PT                 = persObj->m_allTheFloats[27]                        ;
    transObj->m_t_x_center             = persObj->m_allTheFloats[28]                        ;
    transObj->m_t_y_center             = persObj->m_allTheFloats[29]                        ;
    transObj->m_t_radius               = persObj->m_allTheFloats[30]                        ;
    transObj->m_t_ap                   = persObj->m_allTheFloats[31]                        ;
    transObj->m_t_aphi                 = persObj->m_allTheFloats[32]                        ;
    transObj->m_t_phi                  = persObj->m_allTheFloats[33]                        ;
    transObj->m_extrInnerEta           = persObj->m_allTheFloats[34]                        ;
    transObj->m_extrInnerPhi           = persObj->m_allTheFloats[35]                        ;
    transObj->m_mdt_Inner_slope        = persObj->m_allTheFloats[36]                        ;
    transObj->m_mdt_Inner_intercept    = persObj->m_allTheFloats[37]                        ;
    transObj->m_mdt_Inner_Z            = persObj->m_allTheFloats[38]                        ;
    transObj->m_mdt_Inner_R            = persObj->m_allTheFloats[39]                        ;
    transObj->m_mdt_Inner_fit_chi      = persObj->m_allTheFloats[40]                        ;
    transObj->m_mdt_Middle_slope       = persObj->m_allTheFloats[41]                        ;
    transObj->m_mdt_Middle_intercept   = persObj->m_allTheFloats[42]                        ;
    transObj->m_mdt_Middle_Z           = persObj->m_allTheFloats[43]                        ;
    transObj->m_mdt_Middle_R           = persObj->m_allTheFloats[44]                        ;
    transObj->m_mdt_Middle_fit_chi     = persObj->m_allTheFloats[45]                        ;
    transObj->m_mdt_Outer_slope        = persObj->m_allTheFloats[46]                        ;
    transObj->m_mdt_Outer_intercept    = persObj->m_allTheFloats[47]                        ;
    transObj->m_mdt_Outer_Z            = persObj->m_allTheFloats[48]                        ;
    transObj->m_mdt_Outer_R            = persObj->m_allTheFloats[49]                        ;
    transObj->m_mdt_Outer_fit_chi      = persObj->m_allTheFloats[50]                        ;
    transObj->m_Sagitta                = persObj->m_allTheFloats[51]                        ;
    transObj->m_Radius                 = persObj->m_allTheFloats[52]                        ;
    transObj->m_Slope                  = persObj->m_allTheFloats[53]                        ;
    transObj->m_Intercept              = persObj->m_allTheFloats[54]                        ;
    transObj->m_Alpha                  = persObj->m_allTheFloats[55]                        ;
    transObj->m_Beta                   = persObj->m_allTheFloats[56]                        ;
    transObj->m_DeltaR                 = persObj->m_allTheFloats[57]                        ;
    transObj->m_Speed_over_c           = persObj->m_allTheFloats[58]                        ;
    transObj->m_Eta                    = persObj->m_allTheFloats[59]                        ;
    transObj->m_PhiMap                 = persObj->m_allTheFloats[60]                        ;
    transObj->m_Phi                    = persObj->m_allTheFloats[61]                        ;
    transObj->m_PhiDir                 = persObj->m_allTheFloats[62]                        ;
    transObj->m_Pt                     = persObj->m_allTheFloats[63]                        ;
    transObj->m_Charge                 = persObj->m_allTheFloats[64]                        ;
                                                                                                                      
    transObj->m_extension0             = persObj->m_extension0                              ;
    transObj->m_extension1             = persObj->m_extension1                              ;
    transObj->m_extension2             = persObj->m_extension2                              ;
    transObj->m_extension3             = persObj->m_extension3                              ;
    transObj->m_extension4             = persObj->m_extension4                              ;
    transObj->m_extension5             = persObj->m_extension5                              ;
    transObj->m_extension6             = persObj->m_extension6                              ;
    transObj->m_extension7             = persObj->m_extension7                              ;
    transObj->m_extension8             = persObj->m_extension8                              ;
    transObj->m_extension9             = persObj->m_extension9                              ;
    transObj->m_lvl1_emulation         = persObj->m_lvl1_emulation                          ;
                                                                                                                 
    transObj->m_rob_id                 = persObj->m_rob_id                                  ;
    transObj->m_csm_id                 = persObj->m_csm_id                                  ;
    transObj->m_csm_size               = persObj->m_csm_size                                ;
    transObj->m_csm_error              = persObj->m_csm_error                               ;
    transObj->m_removed_rob_id         = persObj->m_removed_rob_id                          ;
    transObj->m_removed_csm_id         = persObj->m_removed_csm_id                          ;
    
    unsigned int padsize=persObj->padv.size();
    transObj->m_pad_hit_onlineId.resize(padsize);
    transObj->m_pad_hit_code    .resize(padsize);
    transObj->m_pad_hit_x       .resize(padsize);
    transObj->m_pad_hit_y       .resize(padsize);
    transObj->m_pad_hit_z       .resize(padsize);
    transObj->m_pad_hit_r       .resize(padsize);
    transObj->m_pad_hit_p       .resize(padsize);
    
    for (unsigned int i=0;i<padsize;i++){
        transObj->m_pad_hit_onlineId[i]= persObj->padv[i].m_pad_hit_onlineId;
        transObj->m_pad_hit_code    [i]= persObj->padv[i].m_pad_hit_code    ;
        transObj->m_pad_hit_x       [i]= persObj->padv[i].m_pad_hit_x       ;
        transObj->m_pad_hit_y       [i]= persObj->padv[i].m_pad_hit_y       ; 
        transObj->m_pad_hit_z       [i]= persObj->padv[i].m_pad_hit_z       ;
        transObj->m_pad_hit_r       [i]= persObj->padv[i].m_pad_hit_r       ;
        transObj->m_pad_hit_p       [i]= persObj->padv[i].m_pad_hit_p       ;
        // std::cout<<i<<"\t"<<transObj->m_pad_hit_onlineId[i]<<"\t"<< transObj->m_pad_hit_code[i] <<"\t"<< transObj->m_pad_hit_x[i] <<"\t"<< transObj->m_pad_hit_y[i] <<"\t"<< transObj->m_pad_hit_p[i] << std::endl;
    }
    
    
    unsigned int tgcInnSizePhi=persObj->tgcInnVphi.size();
    transObj->m_tgc_Inn_phi_hit_in_seg .resize(tgcInnSizePhi)                ;
    transObj->m_tgc_Inn_phi_hit_r      .resize(tgcInnSizePhi)                ;
    transObj->m_tgc_Inn_phi_hit_z      .resize(tgcInnSizePhi)                ;
    transObj->m_tgc_Inn_phi_hit_phi    .resize(tgcInnSizePhi)                ;
    transObj->m_tgc_Inn_phi_hit_width  .resize(tgcInnSizePhi)                ;
    
    unsigned int tgcInnSizeRho=persObj->tgcInnVrho.size();
    transObj->m_tgc_Inn_rho_hit_in_seg .resize(tgcInnSizeRho)                ;
    transObj->m_tgc_Inn_rho_hit_r      .resize(tgcInnSizeRho)                ;
    transObj->m_tgc_Inn_rho_hit_z      .resize(tgcInnSizeRho)                ;
    transObj->m_tgc_Inn_rho_hit_phi    .resize(tgcInnSizeRho)                ;
    transObj->m_tgc_Inn_rho_hit_width  .resize(tgcInnSizeRho)                ;

    unsigned int tgcMidSizePhi=persObj->tgcMidVphi.size();
    transObj->m_tgc_Mid_phi_hit_in_seg .resize(tgcMidSizePhi)                ;
    transObj->m_tgc_Mid_phi_hit_r      .resize(tgcMidSizePhi)                ;
    transObj->m_tgc_Mid_phi_hit_z      .resize(tgcMidSizePhi)                ;
    transObj->m_tgc_Mid_phi_hit_phi    .resize(tgcMidSizePhi)                ;
    transObj->m_tgc_Mid_phi_hit_width  .resize(tgcMidSizePhi)                ;
    
    unsigned int tgcMidSizeRho=persObj->tgcMidVrho.size();
    transObj->m_tgc_Mid_rho_hit_in_seg .resize(tgcMidSizeRho)                ;
    transObj->m_tgc_Mid_rho_hit_r      .resize(tgcMidSizeRho)                ;
    transObj->m_tgc_Mid_rho_hit_z      .resize(tgcMidSizeRho)                ;
    transObj->m_tgc_Mid_rho_hit_phi    .resize(tgcMidSizeRho)                ;
    transObj->m_tgc_Mid_rho_hit_width  .resize(tgcMidSizeRho)                ;
    
    for (unsigned int i=0;i<tgcInnSizePhi;i++){
        transObj->m_tgc_Inn_phi_hit_in_seg[i]= persObj->tgcInnVphi[i].m_tgc_phi_hit_in_seg                  ;
        transObj->m_tgc_Inn_phi_hit_r     [i]= persObj->tgcInnVphi[i].m_tgc_phi_hit_r                       ;
        transObj->m_tgc_Inn_phi_hit_z     [i]= persObj->tgcInnVphi[i].m_tgc_phi_hit_z                       ;
        transObj->m_tgc_Inn_phi_hit_phi   [i]= persObj->tgcInnVphi[i].m_tgc_phi_hit_phi                     ;
        transObj->m_tgc_Inn_phi_hit_width [i]= persObj->tgcInnVphi[i].m_tgc_phi_hit_width                   ;    
        // std::cout<<i<<"\t"<< transObj->m_tgc_Inn_phi_hit_in_seg[i] <<"\t"<< transObj->m_tgc_Inn_phi_hit_width[i] << std::endl;
    }
    
    for (unsigned int i=0;i<tgcInnSizeRho;i++){
        transObj->m_tgc_Inn_rho_hit_in_seg[i]= persObj->tgcInnVrho[i].m_tgc_rho_hit_in_seg                  ;
        transObj->m_tgc_Inn_rho_hit_r     [i]= persObj->tgcInnVrho[i].m_tgc_rho_hit_r                       ;
        transObj->m_tgc_Inn_rho_hit_z     [i]= persObj->tgcInnVrho[i].m_tgc_rho_hit_z                       ;
        transObj->m_tgc_Inn_rho_hit_phi   [i]= persObj->tgcInnVrho[i].m_tgc_rho_hit_phi                     ;
        transObj->m_tgc_Inn_rho_hit_width [i]= persObj->tgcInnVrho[i].m_tgc_rho_hit_width                   ;  
        // std::cout<<i<<"\t"<<transObj->m_tgc_Inn_rho_hit_in_seg[i] <<"\t"<< transObj->m_tgc_Inn_rho_hit_phi[i]  << std::endl;
    }
    
    
    for (unsigned int i=0;i<tgcMidSizePhi;i++){
        transObj->m_tgc_Mid_phi_hit_in_seg[i]= persObj->tgcMidVphi[i].m_tgc_phi_hit_in_seg                  ;
        transObj->m_tgc_Mid_phi_hit_r     [i]= persObj->tgcMidVphi[i].m_tgc_phi_hit_r                       ;
        transObj->m_tgc_Mid_phi_hit_z     [i]= persObj->tgcMidVphi[i].m_tgc_phi_hit_z                       ;
        transObj->m_tgc_Mid_phi_hit_phi   [i]= persObj->tgcMidVphi[i].m_tgc_phi_hit_phi                     ;
        transObj->m_tgc_Mid_phi_hit_width [i]= persObj->tgcMidVphi[i].m_tgc_phi_hit_width                   ;
        // std::cout<<i<<"\t"<< transObj->m_tgc_Mid_phi_hit_in_seg[i] <<"\t"<< transObj->m_tgc_Mid_phi_hit_width[i] << std::endl;
    }
    
    for (unsigned int i=0;i<tgcMidSizeRho;i++){
        transObj->m_tgc_Mid_rho_hit_in_seg[i]= persObj->tgcMidVrho[i].m_tgc_rho_hit_in_seg                  ;
        transObj->m_tgc_Mid_rho_hit_r     [i]= persObj->tgcMidVrho[i].m_tgc_rho_hit_r                       ;
        transObj->m_tgc_Mid_rho_hit_z     [i]= persObj->tgcMidVrho[i].m_tgc_rho_hit_z                       ;
        transObj->m_tgc_Mid_rho_hit_phi   [i]= persObj->tgcMidVrho[i].m_tgc_rho_hit_phi                     ;
        transObj->m_tgc_Mid_rho_hit_width [i]= persObj->tgcMidVrho[i].m_tgc_rho_hit_width                   ;
        // std::cout<<i<<"\t"<<transObj->m_tgc_Mid_rho_hit_in_seg[i] <<"\t"<< transObj->m_tgc_Mid_rho_hit_phi[i] << std::endl;
    }
    
    
    
    
    
    unsigned int mdhsize=persObj->mdhv.size();
    transObj->m_mdt_onlineId     .resize(mdhsize);
    transObj->m_mdt_offlineId    .resize(mdhsize);
    transObj->m_mdt_tube_r       .resize(mdhsize);
    transObj->m_mdt_tube_z       .resize(mdhsize);
    transObj->m_mdt_tube_residual.resize(mdhsize);
    transObj->m_mdt_tube_time    .resize(mdhsize);
    transObj->m_mdt_tube_space   .resize(mdhsize);
    transObj->m_mdt_tube_sigma   .resize(mdhsize);
    
    for (unsigned int i=0;i<mdhsize;i++){
        transObj->m_mdt_onlineId[i]      = persObj->mdhv[i].m_mdt_onlineId      ;
        transObj->m_mdt_offlineId[i]     = persObj->mdhv[i].m_mdt_offlineId     ;
        transObj->m_mdt_tube_r[i]        = persObj->mdhv[i].m_mdt_tube_r        ;
        transObj->m_mdt_tube_z[i]        = persObj->mdhv[i].m_mdt_tube_z        ; 
        transObj->m_mdt_tube_residual[i] = persObj->mdhv[i].m_mdt_tube_residual ;
        transObj->m_mdt_tube_time[i]     = persObj->mdhv[i].m_mdt_tube_time     ;
        transObj->m_mdt_tube_space[i]    = persObj->mdhv[i].m_mdt_tube_space    ;
        transObj->m_mdt_tube_sigma[i]    = persObj->mdhv[i].m_mdt_tube_sigma    ;
        // std::cout<<i<<"\t"<<transObj->m_mdt_onlineId[i]<<"\t"<< transObj->m_mdt_offlineId[i] <<"\t"<< transObj->m_mdt_tube_r[i] <<"\t"<< transObj->m_mdt_tube_z[i] <<"\t"<< transObj->m_mdt_tube_sigma[i] << std::endl;
    }
    
                                                                                                                   
    transObj->m_chamber_type_1         = persObj->m_chamber_type_1                          ;
    transObj->m_chamber_type_2         = persObj->m_chamber_type_2                          ;
    transObj->m_pos                    = persObj->m_pos                                     ;
                                                                                                                   
    transObj->m_aw                     = persObj->m_aw                                      ;
    transObj->m_bw                     = persObj->m_bw                                      ;
    transObj->m_zetaMin                = persObj->m_zetaMin                                 ;
    transObj->m_zetaMax                = persObj->m_zetaMax                                 ;
    transObj->m_radMin                 = persObj->m_radMin                                  ;
    transObj->m_radMax                 = persObj->m_radMax                                  ;
    transObj->m_etaMin                 = persObj->m_etaMin                                  ;
    transObj->m_etaMax                 = persObj->m_etaMax                                  ;
    transObj->m_st_phi                 = persObj->m_st_phi                                  ;
    transObj->m_st_roads               = persObj->m_st_roads                                ;
                                                            


}



void MuonFeatureDetailsCnv_p2::transToPers(const MuonFeatureDetails* transObj, MuonFeatureDetails_p2* persObj, MsgStream &log) {
   log << MSG::DEBUG << "MuonFeatureDetailsCnv_p2::transToPers called " << endreq;
   
    persObj->m_id      = transObj->m_id;
    persObj->m_Address = transObj->m_Address;
    
    persObj->m_allTheUInts[0] = transObj->m_te_id;
    persObj->m_allTheUInts[1] = transObj->m_error;
    persObj->m_allTheUInts[2] = transObj->m_lvl1_id;
    persObj->m_allTheUInts[3] = transObj->m_lumi_block;
    persObj->m_allTheUInts[4] = transObj->m_muondetmask;
    persObj->m_allTheUInts[5] = transObj->m_RoIID;
    persObj->m_allTheUInts[6] = transObj->m_RoISystem;
    persObj->m_allTheUInts[7] = transObj->m_RoISubsystem;
    persObj->m_allTheUInts[8] = transObj->m_RoISector;
    persObj->m_allTheUInts[9] = transObj->m_RoINumber;
    persObj->m_allTheUInts[10] = transObj->m_RoIThreshold;
    persObj->m_allTheUInts[11]  = transObj->m_rpc_pad_error;
    persObj->m_allTheUInts[12]  = transObj->m_tgc_rdo_error;


    persObj->m_allTheLongs[0] = transObj->m_tgc_Mid_rho_N;
    persObj->m_allTheLongs[1] = transObj->m_tgc_Mid_phi_N;
    persObj->m_allTheLongs[2] = transObj->m_tgc_Inn_rho_N;
    persObj->m_allTheLongs[3] = transObj->m_tgc_Inn_phi_N;
    persObj->m_allTheLongs[4] = transObj->m_type;



    persObj->m_allTheFloats[0] = transObj->m_RoIEta;
    persObj->m_allTheFloats[1] = transObj->m_RoIPhi;
    persObj->m_allTheFloats[2] = transObj->m_rpc1_x;
    persObj->m_allTheFloats[3] = transObj->m_rpc1_y;
    persObj->m_allTheFloats[4] = transObj->m_rpc1_z;
    persObj->m_allTheFloats[5] = transObj->m_rpc2_x;
    persObj->m_allTheFloats[6] = transObj->m_rpc2_y;
    persObj->m_allTheFloats[7] = transObj->m_rpc2_z;
    persObj->m_allTheFloats[8] = transObj->m_rpc3_x;
    persObj->m_allTheFloats[9] = transObj->m_rpc3_y;
    persObj->m_allTheFloats[10] = transObj->m_rpc3_z; 
    persObj->m_allTheFloats[11] = transObj->m_tgc_Mid1_eta;
    persObj->m_allTheFloats[12] = transObj->m_tgc_Mid1_phi;
    persObj->m_allTheFloats[13] = transObj->m_tgc_Mid1_r;
    persObj->m_allTheFloats[14] = transObj->m_tgc_Mid1_z;
    persObj->m_allTheFloats[15] = transObj->m_tgc_Mid2_eta;
    persObj->m_allTheFloats[16] = transObj->m_tgc_Mid2_phi;
    persObj->m_allTheFloats[17] = transObj->m_tgc_Mid2_r;
    persObj->m_allTheFloats[18] = transObj->m_tgc_Mid2_z;
    persObj->m_allTheFloats[19] = transObj->m_tgc_Mid_rho_chi2;
    persObj->m_allTheFloats[20] = transObj->m_tgc_Mid_phi_chi2;
    persObj->m_allTheFloats[21] = transObj->m_tgc_Inn_eta;
    persObj->m_allTheFloats[22] = transObj->m_tgc_Inn_phi;
    persObj->m_allTheFloats[23] = transObj->m_tgc_Inn_r;
    persObj->m_allTheFloats[24] = transObj->m_tgc_Inn_z;
    persObj->m_allTheFloats[25] = transObj->m_tgc_Inn_rho_std;
    persObj->m_allTheFloats[26] = transObj->m_tgc_Inn_phi_std;
    persObj->m_allTheFloats[27] = transObj->m_tgc_PT;
    persObj->m_allTheFloats[28] = transObj->m_t_x_center;
    persObj->m_allTheFloats[29] = transObj->m_t_y_center;
    persObj->m_allTheFloats[30] = transObj->m_t_radius;
    persObj->m_allTheFloats[31] = transObj->m_t_ap;
    persObj->m_allTheFloats[32] = transObj->m_t_aphi;
    persObj->m_allTheFloats[33] = transObj->m_t_phi;
    persObj->m_allTheFloats[34] = transObj->m_extrInnerEta;
    persObj->m_allTheFloats[35] = transObj->m_extrInnerPhi;
    persObj->m_allTheFloats[36] = transObj->m_mdt_Inner_slope;
    persObj->m_allTheFloats[37] = transObj->m_mdt_Inner_intercept;
    persObj->m_allTheFloats[38] = transObj->m_mdt_Inner_Z;
    persObj->m_allTheFloats[39] = transObj->m_mdt_Inner_R;
    persObj->m_allTheFloats[40] = transObj->m_mdt_Inner_fit_chi;
    persObj->m_allTheFloats[41] = transObj->m_mdt_Middle_slope;
    persObj->m_allTheFloats[42] = transObj->m_mdt_Middle_intercept;
    persObj->m_allTheFloats[43] = transObj->m_mdt_Middle_Z;
    persObj->m_allTheFloats[44] = transObj->m_mdt_Middle_R;
    persObj->m_allTheFloats[45] = transObj->m_mdt_Middle_fit_chi;
    persObj->m_allTheFloats[46] = transObj->m_mdt_Outer_slope;
    persObj->m_allTheFloats[47] = transObj->m_mdt_Outer_intercept;
    persObj->m_allTheFloats[48] = transObj->m_mdt_Outer_Z;
    persObj->m_allTheFloats[49] = transObj->m_mdt_Outer_R;
    persObj->m_allTheFloats[50] = transObj->m_mdt_Outer_fit_chi;    
    persObj->m_allTheFloats[51] = transObj->m_Sagitta;
    persObj->m_allTheFloats[52] = transObj->m_Radius;
    persObj->m_allTheFloats[53] = transObj->m_Slope;
    persObj->m_allTheFloats[54] = transObj->m_Intercept;
    persObj->m_allTheFloats[55] = transObj->m_Alpha;
    persObj->m_allTheFloats[56] = transObj->m_Beta;
    persObj->m_allTheFloats[57] = transObj->m_DeltaR;
    persObj->m_allTheFloats[58] = transObj->m_Speed_over_c;
    persObj->m_allTheFloats[59] = transObj->m_Eta;
    persObj->m_allTheFloats[60] = transObj->m_PhiMap;
    persObj->m_allTheFloats[61] = transObj->m_Phi;
    persObj->m_allTheFloats[62] = transObj->m_PhiDir;
    persObj->m_allTheFloats[63] = transObj->m_Pt;
    persObj->m_allTheFloats[64] = transObj->m_Charge;    
    
    persObj->m_extension0     = transObj->m_extension0;
    persObj->m_extension1     = transObj->m_extension1;
    persObj->m_extension2     = transObj->m_extension2;
    persObj->m_extension3     = transObj->m_extension3;
    persObj->m_extension4     = transObj->m_extension4;
    persObj->m_extension5     = transObj->m_extension5;
    persObj->m_extension6     = transObj->m_extension6;
    persObj->m_extension7     = transObj->m_extension7;
    persObj->m_extension8     = transObj->m_extension8;
    persObj->m_extension9     = transObj->m_extension9;
    persObj->m_lvl1_emulation = transObj->m_lvl1_emulation;
    

    persObj->m_rob_id           = transObj->m_rob_id; // m_max_rob_capacity
    persObj->m_removed_rob_id   = transObj->m_removed_rob_id;
    
    
    persObj->m_csm_id           = transObj->m_csm_id; // m_max_csm_capacity
    persObj->m_csm_size         = transObj->m_csm_size;
    persObj->m_csm_error        = transObj->m_csm_error;
    persObj->m_removed_csm_id   = transObj->m_removed_csm_id;
    
    
    // std::cout<<transObj->m_pad_hit_onlineId.size()<<"\t"<< transObj->m_pad_hit_code.size() <<"\t"<< transObj->m_pad_hit_x.size() <<"\t"<< transObj->m_pad_hit_y.size() <<"\t"<< transObj->m_pad_hit_z.size() << "\t"<<transObj->m_pad_hit_r.size()<<"\t"<< transObj->m_pad_hit_p.size()<< std::endl;
    // std::cout<<"I\n"<< transObj->m_pad_hit_onlineId.size()<<std::endl;// m_max_rpc_hits_capacity
    // std::cout<< transObj->m_pad_hit_code.size()<<std::endl;    
    // std::cout<< transObj->m_pad_hit_x.size()<<std::endl;
    // std::cout<< transObj->m_pad_hit_y.size()<<std::endl;
    // std::cout<< transObj->m_pad_hit_z.size()<<std::endl;
    // std::cout<< transObj->m_pad_hit_r.size()<<std::endl;
    // std::cout<< transObj->m_pad_hit_p.size()<<std::endl;
    
    persObj->padv.reserve(transObj->m_pad_hit_onlineId.size());
    for (unsigned int i=0;i<transObj->m_pad_hit_onlineId.size();i++){    
        padhit pah;
        pah.m_pad_hit_onlineId = transObj->m_pad_hit_onlineId[i];// m_max_rpc_hits_capacity
        pah.m_pad_hit_code     = transObj->m_pad_hit_code[i];    
        pah.m_pad_hit_x        = transObj->m_pad_hit_x[i];
        pah.m_pad_hit_y        = transObj->m_pad_hit_y[i];
        pah.m_pad_hit_z        = transObj->m_pad_hit_z[i];
        pah.m_pad_hit_r        = transObj->m_pad_hit_r[i];
        pah.m_pad_hit_p        = transObj->m_pad_hit_p[i];
        persObj->padv.push_back(pah);
        // std::cout<<i<<"\t"<<transObj->m_pad_hit_onlineId[i]<<"\t"<< transObj->m_pad_hit_code[i] <<"\t"<< transObj->m_pad_hit_x[i] <<"\t"<< transObj->m_pad_hit_y[i] <<"\t"<< transObj->m_pad_hit_p[i] << std::endl;
    }
    
    // std::cout<<transObj->m_tgc_Inn_rho_hit_in_seg.size()<<"\t"<< transObj->m_tgc_Inn_phi_hit_in_seg.size() <<"\t"<< transObj->m_tgc_Inn_rho_hit_phi.size() <<"\t"<< transObj->m_tgc_Inn_rho_hit_phi.size() <<"\t"<< transObj->m_tgc_Inn_rho_hit_r.size() << "\t"<<transObj->m_tgc_Inn_rho_hit_z.size()<<"\t"<< transObj->m_tgc_Inn_rho_hit_width.size()<< "\t";
    // std::cout<<transObj->m_tgc_Inn_phi_hit_phi.size()<<"\t"<< transObj->m_tgc_Inn_phi_hit_r.size() <<"\t"<< transObj->m_tgc_Inn_phi_hit_z.size() <<"\t"<< transObj->m_tgc_Inn_phi_hit_width.size() << std::endl;
    

    // std::cout<<"II1\n"<< transObj->m_tgc_Inn_phi_hit_in_seg.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Inn_phi_hit_r.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Inn_phi_hit_z.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Inn_phi_hit_phi.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Inn_phi_hit_width.size()<<std::endl;
    
    tgchit tgc;
    persObj->tgcInnVphi.reserve(transObj->m_tgc_Inn_phi_hit_in_seg.size());
    for (unsigned int i=0;i<transObj->m_tgc_Inn_phi_hit_in_seg.size();i++){
        tgc.m_tgc_phi_hit_in_seg = transObj->m_tgc_Inn_phi_hit_in_seg[i];
        tgc.m_tgc_phi_hit_r      = transObj->m_tgc_Inn_phi_hit_r[i];
        tgc.m_tgc_phi_hit_z      = transObj->m_tgc_Inn_phi_hit_z[i];
        tgc.m_tgc_phi_hit_phi    = transObj->m_tgc_Inn_phi_hit_phi[i];
        tgc.m_tgc_phi_hit_width  = transObj->m_tgc_Inn_phi_hit_width[i];
	//
	tgc.m_tgc_rho_hit_in_seg = 0;
	tgc.m_tgc_rho_hit_r      = 0;
	tgc.m_tgc_rho_hit_z      = 0;
	tgc.m_tgc_rho_hit_phi    = 0;
	tgc.m_tgc_rho_hit_width  = 0;
	//
        persObj->tgcInnVphi.push_back(tgc);    
        // std::cout<<i<<"\t"<< transObj->m_tgc_Inn_phi_hit_in_seg[i] << <<"\t"<< transObj->m_tgc_Inn_phi_hit_width[i] << std::endl;
    }   
    
    // std::cout<<"II2\n"<< transObj->m_tgc_Inn_rho_hit_in_seg.size()<<std::endl; // m_max_tgc_hits_capacity
    // std::cout<< transObj->m_tgc_Inn_rho_hit_r.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Inn_rho_hit_z.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Inn_rho_hit_phi.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Inn_rho_hit_width.size()<<std::endl;  
      
    persObj->tgcInnVrho.reserve(transObj->m_tgc_Inn_rho_hit_in_seg.size());
    for (unsigned int i=0;i<transObj->m_tgc_Inn_rho_hit_in_seg.size();i++){
        tgc.m_tgc_rho_hit_in_seg = transObj->m_tgc_Inn_rho_hit_in_seg[i]; // m_max_tgc_hits_capacity
        tgc.m_tgc_rho_hit_r      = transObj->m_tgc_Inn_rho_hit_r[i];
        tgc.m_tgc_rho_hit_z      = transObj->m_tgc_Inn_rho_hit_z[i];
        tgc.m_tgc_rho_hit_phi    = transObj->m_tgc_Inn_rho_hit_phi[i];
        tgc.m_tgc_rho_hit_width  = transObj->m_tgc_Inn_rho_hit_width[i];
	//
	tgc.m_tgc_phi_hit_in_seg = 0;
	tgc.m_tgc_phi_hit_r      = 0;
	tgc.m_tgc_phi_hit_z      = 0;
	tgc.m_tgc_phi_hit_phi    = 0;
	tgc.m_tgc_phi_hit_width  = 0;
	//
        persObj->tgcInnVrho.push_back(tgc);    
        // std::cout<<i<<"\t"<<transObj->m_tgc_Inn_rho_hit_in_seg[i] <<"\t"<< transObj->m_tgc_Inn_rho_hit_phi[i] << std::endl;
    }
    
    
    
    // std::cout<<"III1\n"<< transObj->m_tgc_Mid_phi_hit_in_seg.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Mid_phi_hit_r.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Mid_phi_hit_z.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Mid_phi_hit_phi.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Mid_phi_hit_width.size()<<std::endl;  
     
    persObj->tgcMidVphi.reserve(transObj->m_tgc_Mid_phi_hit_in_seg.size());
    for (unsigned int i=0;i<transObj->m_tgc_Mid_phi_hit_in_seg.size();i++){
        tgc.m_tgc_phi_hit_in_seg = transObj->m_tgc_Mid_phi_hit_in_seg[i];
        tgc.m_tgc_phi_hit_r      = transObj->m_tgc_Mid_phi_hit_r[i];
        tgc.m_tgc_phi_hit_z      = transObj->m_tgc_Mid_phi_hit_z[i];
        tgc.m_tgc_phi_hit_phi    = transObj->m_tgc_Mid_phi_hit_phi[i];
        tgc.m_tgc_phi_hit_width  = transObj->m_tgc_Mid_phi_hit_width[i];
	//
	tgc.m_tgc_rho_hit_in_seg = 0;
	tgc.m_tgc_rho_hit_r      = 0;
	tgc.m_tgc_rho_hit_z      = 0;
	tgc.m_tgc_rho_hit_phi    = 0;
	tgc.m_tgc_rho_hit_width  = 0;
	//
        persObj->tgcMidVphi.push_back(tgc);
        // std::cout<<i<<"\t"<< transObj->m_tgc_Mid_phi_hit_in_seg[i] <<"\t"<< transObj->m_tgc_Mid_phi_hit_width[i] << std::endl;
    }
 
    // std::cout<<"III2\n"<< transObj->m_tgc_Mid_rho_hit_in_seg.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Mid_rho_hit_r.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Mid_rho_hit_z.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Mid_rho_hit_phi.size()<<std::endl;
    // std::cout<< transObj->m_tgc_Mid_rho_hit_width.size()<<std::endl;
    
    persObj->tgcMidVrho.reserve(transObj->m_tgc_Mid_rho_hit_in_seg.size());
    for (unsigned int i=0;i<transObj->m_tgc_Mid_rho_hit_in_seg.size();i++){
        tgc.m_tgc_rho_hit_in_seg = transObj->m_tgc_Mid_rho_hit_in_seg[i];
        tgc.m_tgc_rho_hit_r      = transObj->m_tgc_Mid_rho_hit_r[i];
        tgc.m_tgc_rho_hit_z      = transObj->m_tgc_Mid_rho_hit_z[i];
        tgc.m_tgc_rho_hit_phi    = transObj->m_tgc_Mid_rho_hit_phi[i];
        tgc.m_tgc_rho_hit_width  = transObj->m_tgc_Mid_rho_hit_width[i];
	//
	tgc.m_tgc_phi_hit_in_seg = 0;
	tgc.m_tgc_phi_hit_r      = 0;
	tgc.m_tgc_phi_hit_z      = 0;
	tgc.m_tgc_phi_hit_phi    = 0;
	tgc.m_tgc_phi_hit_width  = 0;
	//
        persObj->tgcMidVrho.push_back(tgc);
        // std::cout<<i<<"\t"<<transObj->m_tgc_Mid_rho_hit_in_seg[i] <<"\t"<< transObj->m_tgc_Mid_rho_hit_phi[i] << std::endl;
    }
    
    
    
    
    
    
    // std::cout<<"IV\n"<<transObj->m_mdt_onlineId.size()<<std::endl;// m_max_mdt_hits_capacity
    // std::cout<<transObj->m_mdt_offlineId.size()<<std::endl;
    // std::cout<<transObj->m_mdt_tube_r.size()<<std::endl;
    // std::cout<<transObj->m_mdt_tube_z.size()<<std::endl;
    // std::cout<<transObj->m_mdt_tube_residual.size()<<std::endl;
    // std::cout<<transObj->m_mdt_tube_time.size()<<std::endl;
    // std::cout<<transObj->m_mdt_tube_space.size()<<std::endl;
    // std::cout<<transObj->m_mdt_tube_sigma.size()<<std::endl; 
    persObj->mdhv.reserve(transObj->m_mdt_onlineId.size());
    for (unsigned int i=0;i<transObj->m_mdt_onlineId.size();i++){
        mdthit mdh;
        mdh.m_mdt_onlineId      = transObj->m_mdt_onlineId[i];// m_max_mdt_hits_capacity
        mdh.m_mdt_offlineId     = transObj->m_mdt_offlineId[i];
        mdh.m_mdt_tube_r        = transObj->m_mdt_tube_r[i];
        mdh.m_mdt_tube_z        = transObj->m_mdt_tube_z[i];
        mdh.m_mdt_tube_residual = transObj->m_mdt_tube_residual[i];
        mdh.m_mdt_tube_time     = transObj->m_mdt_tube_time[i];
        mdh.m_mdt_tube_space    = transObj->m_mdt_tube_space[i];
        mdh.m_mdt_tube_sigma    = transObj->m_mdt_tube_sigma[i];
        persObj->mdhv.push_back(mdh);
        // std::cout<<i<<"\t"<<transObj->m_mdt_onlineId[i]<<"\t"<< transObj->m_mdt_offlineId[i] <<"\t"<< transObj->m_mdt_tube_r[i] <<"\t"<< transObj->m_mdt_tube_z[i] <<"\t"<< transObj->m_mdt_tube_sigma[i] << std::endl;
    }
   
    persObj->m_chamber_type_1 = transObj->m_chamber_type_1;
    persObj->m_chamber_type_2 = transObj->m_chamber_type_2;
    persObj->m_pos            = transObj->m_pos;
    
    persObj->m_aw       = transObj->m_aw;
    persObj->m_bw       = transObj->m_bw;
    persObj->m_zetaMin  = transObj->m_zetaMin;
    persObj->m_zetaMax  = transObj->m_zetaMax;
    persObj->m_radMin   = transObj->m_radMin;
    persObj->m_radMax   = transObj->m_radMax;
    persObj->m_etaMin   = transObj->m_etaMin;
    persObj->m_etaMax   = transObj->m_etaMax;
    persObj->m_st_phi   = transObj->m_st_phi;
    persObj->m_st_roads = transObj->m_st_roads;


}

