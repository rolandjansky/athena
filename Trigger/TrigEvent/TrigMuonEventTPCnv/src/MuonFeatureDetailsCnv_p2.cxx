/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEvent/MuonFeatureDetails.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetails_p2.h"
#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p2.h"

void MuonFeatureDetailsCnv_p2::persToTrans(const MuonFeatureDetails_p2* persObj, MuonFeatureDetails* transObj, MsgStream &log) {
   log << MSG::DEBUG << "MuonFeatureDetailsCnv_p2::persToTrans called " << endreq;

    *transObj = MuonFeatureDetails (persObj->m_id,
                                    persObj->m_allTheUInts[0], // m_te_id
                                    persObj->m_allTheUInts[2], // m_lvl1_id
                                    persObj->m_allTheUInts[3], // m_lumi_block
                                    persObj->m_allTheUInts[4], // m_muondetmask,
                                    persObj->m_allTheUInts[5], // m_RoIID
                                    persObj->m_allTheUInts[6], // m_RoISystem
                                    persObj->m_allTheUInts[7], // m_RoISubsystem
                                    persObj->m_allTheUInts[8], // m_RoISector
                                    persObj->m_allTheUInts[9], // m_RoINumber
                                    persObj->m_allTheUInts[10],// m_RoIThreshold
                                    persObj->m_allTheFloats[0],// >m_RoIEta,
                                    persObj->m_allTheFloats[1],// >m_RoIPhi,
                                    -1, // m_extension_capacity
                                    -1, // m_max_rob_capacity
                                    -1, // m_max_csm_capacity
                                    -1, // m_max_lvl1_emu_capacity
                                    -1, // m_max_rpc_hits_capacity
                                    -1, // m_max_tgc_hits_capacity
                                    -1); // m_max_mdt_hits_capacity

    transObj->setError       (persObj->m_allTheUInts[1]);
    transObj->setRpcPadError (persObj->m_allTheUInts[11]);
    transObj->setTgcPadError (persObj->m_allTheUInts[12]);

    transObj->setRpc1 (persObj->m_allTheFloats[2], // x
                       persObj->m_allTheFloats[3], // y
                       persObj->m_allTheFloats[4]);// z
    transObj->setRpc2 (persObj->m_allTheFloats[5], // x
                       persObj->m_allTheFloats[6], // y
                       persObj->m_allTheFloats[7]);// z
    transObj->setRpc3 (persObj->m_allTheFloats[8], // x
                       persObj->m_allTheFloats[9], // y
                       persObj->m_allTheFloats[10]);// z
   
    transObj->setTgcMid1 (persObj->m_allTheFloats[11],  // eta
                          persObj->m_allTheFloats[12],  // phi
                          persObj->m_allTheFloats[13],  // r
                          persObj->m_allTheFloats[14]); // z
    transObj->setTgcMid2 (persObj->m_allTheFloats[15],  // eta
                          persObj->m_allTheFloats[16],  // phi
                          persObj->m_allTheFloats[17],  // r
                          persObj->m_allTheFloats[18]); // z
    transObj->setTgcMidF (persObj->m_allTheFloats[19],  // rho_chi2
                          persObj->m_allTheLongs[0],    // rho_N
                          persObj->m_allTheFloats[20],  // phi_chi2
                          persObj->m_allTheLongs[1]);   // phi_N
    transObj->setTgcInn (persObj->m_allTheFloats[21],   // eta
                         persObj->m_allTheFloats[22],   // phi
                         persObj->m_allTheFloats[23],   // r
                         persObj->m_allTheFloats[24]);  // z
    transObj->setTgcInnF (persObj->m_allTheFloats[25],  // rho_std
                          persObj->m_allTheLongs[2],    // rho_N
                          persObj->m_allTheFloats[26],  // phi_std
                          persObj->m_allTheLongs[3]);   // phi_N
    
    transObj->setTgcPt (persObj->m_allTheFloats[27]);

    transObj->setType           (persObj->m_allTheLongs[4]);
    transObj->setT_x_center     (persObj->m_allTheFloats[28]);
    transObj->setT_y_center     (persObj->m_allTheFloats[29]);
    transObj->setT_radius       (persObj->m_allTheFloats[30]);
    transObj->setT_ap           (persObj->m_allTheFloats[31]);
    transObj->setT_aphi         (persObj->m_allTheFloats[32]);
    transObj->setT_phi          (persObj->m_allTheFloats[33]);
    transObj->setExtrInnerEta   (persObj->m_allTheFloats[34]);
    transObj->setExtrInnerPhi   (persObj->m_allTheFloats[35]);

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

    transObj->setSP1 (persObj->m_allTheFloats[36],   // slope
                      persObj->m_allTheFloats[37],   // intercept
                      persObj->m_allTheFloats[38],   // z
                      persObj->m_allTheFloats[39],   // r
                      persObj->m_allTheFloats[40]);  // fit_chi2
    transObj->setSP2 (persObj->m_allTheFloats[41],   // slope
                      persObj->m_allTheFloats[42],   // intercept
                      persObj->m_allTheFloats[43],   // z
                      persObj->m_allTheFloats[44],   // r
                      persObj->m_allTheFloats[45]);  // fit_chi2
    transObj->setSP3 (persObj->m_allTheFloats[46],   // slope
                      persObj->m_allTheFloats[47],   // intercept
                      persObj->m_allTheFloats[48],   // z
                      persObj->m_allTheFloats[49],   // r
                      persObj->m_allTheFloats[50]);  // fit_chi2

    transObj->setAddress      (persObj->m_Address);
    transObj->setSagitta      (persObj->m_allTheFloats[51]);
    transObj->setRadius       (persObj->m_allTheFloats[52]);
    transObj->setSlope        (persObj->m_allTheFloats[53]);
    transObj->setIntercept    (persObj->m_allTheFloats[54]);
    transObj->setAlpha        (persObj->m_allTheFloats[55]);
    transObj->setBeta         (persObj->m_allTheFloats[56]);
    transObj->setDeltaR       (persObj->m_allTheFloats[57]);
    transObj->setSpeed_over_c (persObj->m_allTheFloats[58]);
    transObj->setEta          (persObj->m_allTheFloats[59]);
    transObj->setPhiMap       (persObj->m_allTheFloats[60]);
    transObj->setPhi          (persObj->m_allTheFloats[61]);
    transObj->setPhiDir       (persObj->m_allTheFloats[62]);
    transObj->setPt           (persObj->m_allTheFloats[63]);
    transObj->setCharge       (persObj->m_allTheFloats[64]);
    
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

    for (const padhit& p : persObj->padv) {
      transObj->setPadHit (p.m_pad_hit_onlineId,
                           p.m_pad_hit_code,
                           p.m_pad_hit_x,
                           p.m_pad_hit_y,
                           p.m_pad_hit_z,
                           p.m_pad_hit_r,
                           p.m_pad_hit_p);
    }

    for (const tgchit& p : persObj->tgcInnVrho) {
      transObj->setTgcInnRhoHit (p.m_tgc_rho_hit_phi,
                                 p.m_tgc_rho_hit_r,
                                 p.m_tgc_rho_hit_z,
                                 p.m_tgc_rho_hit_width,
                                 p.m_tgc_rho_hit_in_seg);
    }

    for (const tgchit& p : persObj->tgcInnVphi) {
      transObj->setTgcInnPhiHit (p.m_tgc_phi_hit_phi,
                                 p.m_tgc_phi_hit_r,
                                 p.m_tgc_phi_hit_z,
                                 p.m_tgc_phi_hit_width,
                                 p.m_tgc_phi_hit_in_seg);
    }

    for (const tgchit& p : persObj->tgcMidVrho) {
      transObj->setTgcMidRhoHit (p.m_tgc_rho_hit_phi,
                                 p.m_tgc_rho_hit_r,
                                 p.m_tgc_rho_hit_z,
                                 p.m_tgc_rho_hit_width,
                                 p.m_tgc_rho_hit_in_seg);
    }

    for (const tgchit& p : persObj->tgcMidVphi) {
      transObj->setTgcMidPhiHit (p.m_tgc_phi_hit_phi,
                                 p.m_tgc_phi_hit_r,
                                 p.m_tgc_phi_hit_z,
                                 p.m_tgc_phi_hit_width,
                                 p.m_tgc_phi_hit_in_seg);
    }

    for (const mdthit& p : persObj->mdhv) {
      transObj->setMdtHit (p.m_mdt_onlineId,
                           p.m_mdt_offlineId,
                           p.m_mdt_tube_r,
                           p.m_mdt_tube_z,
                           p.m_mdt_tube_residual,
                           p.m_mdt_tube_time,
                           p.m_mdt_tube_space,
                           p.m_mdt_tube_sigma);
    }
}



void MuonFeatureDetailsCnv_p2::transToPers(const MuonFeatureDetails* transObj, MuonFeatureDetails_p2* persObj, MsgStream &log) {
   log << MSG::DEBUG << "MuonFeatureDetailsCnv_p2::transToPers called " << endreq;
   
    persObj->m_id      = transObj->id();
    persObj->m_Address = transObj->Address();
    
    persObj->m_allTheUInts[0] = transObj->te_id();
    persObj->m_allTheUInts[1] = transObj->error();
    persObj->m_allTheUInts[2] = transObj->lvl1_id();
    persObj->m_allTheUInts[3] = transObj->lumi_block();
    persObj->m_allTheUInts[4] = transObj->muondetmask();
    persObj->m_allTheUInts[5] = transObj->roi_id();
    persObj->m_allTheUInts[6] = transObj->roi_system();
    persObj->m_allTheUInts[7] = transObj->roi_subsystem();
    persObj->m_allTheUInts[8] = transObj->roi_sector();
    persObj->m_allTheUInts[9] = transObj->roi_number();
    persObj->m_allTheUInts[10] = transObj->roi_threshold();
    persObj->m_allTheUInts[11] = transObj->rpc_pad_error();
    persObj->m_allTheUInts[12] = transObj->tgc_rdo_error();


    persObj->m_allTheLongs[0] = transObj->tgc_Mid_rho_N();
    persObj->m_allTheLongs[1] = transObj->tgc_Mid_phi_N();
    persObj->m_allTheLongs[2] = transObj->tgc_Inn_rho_N();
    persObj->m_allTheLongs[3] = transObj->tgc_Inn_phi_N();
    persObj->m_allTheLongs[4] = transObj->type();



    persObj->m_allTheFloats[0] = transObj->roi_eta();
    persObj->m_allTheFloats[1] = transObj->roi_phi();
    persObj->m_allTheFloats[2] = transObj->rpc1_x();
    persObj->m_allTheFloats[3] = transObj->rpc1_y();
    persObj->m_allTheFloats[4] = transObj->rpc1_z();
    persObj->m_allTheFloats[5] = transObj->rpc2_x();
    persObj->m_allTheFloats[6] = transObj->rpc2_y();
    persObj->m_allTheFloats[7] = transObj->rpc2_z();
    persObj->m_allTheFloats[8] = transObj->rpc3_x();
    persObj->m_allTheFloats[9] = transObj->rpc3_y();
    persObj->m_allTheFloats[10] = transObj->rpc3_z(); 
    persObj->m_allTheFloats[11] = transObj->tgc_Mid1_eta();
    persObj->m_allTheFloats[12] = transObj->tgc_Mid1_phi();
    persObj->m_allTheFloats[13] = transObj->tgc_Mid1_r();
    persObj->m_allTheFloats[14] = transObj->tgc_Mid1_z();
    persObj->m_allTheFloats[15] = transObj->tgc_Mid2_eta();
    persObj->m_allTheFloats[16] = transObj->tgc_Mid2_phi();
    persObj->m_allTheFloats[17] = transObj->tgc_Mid2_r();
    persObj->m_allTheFloats[18] = transObj->tgc_Mid2_z();
    persObj->m_allTheFloats[19] = transObj->tgc_Mid_rho_chi2();
    persObj->m_allTheFloats[20] = transObj->tgc_Mid_phi_chi2();
    persObj->m_allTheFloats[21] = transObj->tgc_Inn_eta();
    persObj->m_allTheFloats[22] = transObj->tgc_Inn_phi();
    persObj->m_allTheFloats[23] = transObj->tgc_Inn_r();
    persObj->m_allTheFloats[24] = transObj->tgc_Inn_z();
    persObj->m_allTheFloats[25] = transObj->tgc_Inn_rho_std();
    persObj->m_allTheFloats[26] = transObj->tgc_Inn_phi_std();
    persObj->m_allTheFloats[27] = transObj->tgc_PT();
    persObj->m_allTheFloats[28] = transObj->t_x_center();
    persObj->m_allTheFloats[29] = transObj->t_y_center();
    persObj->m_allTheFloats[30] = transObj->t_radius();
    persObj->m_allTheFloats[31] = transObj->t_ap();
    persObj->m_allTheFloats[32] = transObj->t_aphi();
    persObj->m_allTheFloats[33] = transObj->t_phi();
    persObj->m_allTheFloats[34] = transObj->extrInnerEta();
    persObj->m_allTheFloats[35] = transObj->extrInnerPhi();
    persObj->m_allTheFloats[36] = transObj->mdt_Inner_slope();
    persObj->m_allTheFloats[37] = transObj->mdt_Inner_intercept();
    persObj->m_allTheFloats[38] = transObj->mdt_Inner_Z();
    persObj->m_allTheFloats[39] = transObj->mdt_Inner_R();
    persObj->m_allTheFloats[40] = transObj->mdt_Inner_fit_chi();
    persObj->m_allTheFloats[41] = transObj->mdt_Middle_slope();
    persObj->m_allTheFloats[42] = transObj->mdt_Middle_intercept();
    persObj->m_allTheFloats[43] = transObj->mdt_Middle_Z();
    persObj->m_allTheFloats[44] = transObj->mdt_Middle_R();
    persObj->m_allTheFloats[45] = transObj->mdt_Middle_fit_chi();
    persObj->m_allTheFloats[46] = transObj->mdt_Outer_slope();
    persObj->m_allTheFloats[47] = transObj->mdt_Outer_intercept();
    persObj->m_allTheFloats[48] = transObj->mdt_Outer_Z();
    persObj->m_allTheFloats[49] = transObj->mdt_Outer_R();
    persObj->m_allTheFloats[50] = transObj->mdt_Outer_fit_chi();    
    persObj->m_allTheFloats[51] = transObj->Sagitta();
    persObj->m_allTheFloats[52] = transObj->Radius();
    persObj->m_allTheFloats[53] = transObj->Slope();
    persObj->m_allTheFloats[54] = transObj->Intercept();
    persObj->m_allTheFloats[55] = transObj->Alpha();
    persObj->m_allTheFloats[56] = transObj->Beta();
    persObj->m_allTheFloats[57] = transObj->DeltaR();
    persObj->m_allTheFloats[58] = transObj->Speed_over_c();
    persObj->m_allTheFloats[59] = transObj->Eta();
    persObj->m_allTheFloats[60] = transObj->PhiMap();
    persObj->m_allTheFloats[61] = transObj->Phi();
    persObj->m_allTheFloats[62] = transObj->PhiDir();
    persObj->m_allTheFloats[63] = transObj->Pt();
    persObj->m_allTheFloats[64] = transObj->Charge();    
    
    persObj->m_extension0     = transObj->extension0();
    persObj->m_extension1     = transObj->extension1();
    persObj->m_extension2     = transObj->extension2();
    persObj->m_extension3     = transObj->extension3();
    persObj->m_extension4     = transObj->extension4();
    persObj->m_extension5     = transObj->extension5();
    persObj->m_extension6     = transObj->extension6();
    persObj->m_extension7     = transObj->extension7();
    persObj->m_extension8     = transObj->extension8();
    persObj->m_extension9     = transObj->extension9();
    persObj->m_lvl1_emulation = transObj->lvl1_emulation();
    

    persObj->m_rob_id           = transObj->rob_id(); // m_max_rob_capacity
    persObj->m_removed_rob_id   = transObj->removed_rob_id();
    
    
    persObj->m_csm_id           = transObj->csm_id(); // m_max_csm_capacity
    persObj->m_csm_size         = transObj->csm_size();
    persObj->m_csm_error        = transObj->csm_error();
    persObj->m_removed_csm_id   = transObj->removed_csm_id();
    
    
    // std::cout<<transObj->m_pad_hit_onlineId.size()<<"\t"<< transObj->m_pad_hit_code.size() <<"\t"<< transObj->m_pad_hit_x.size() <<"\t"<< transObj->m_pad_hit_y.size() <<"\t"<< transObj->m_pad_hit_z.size() << "\t"<<transObj->m_pad_hit_r.size()<<"\t"<< transObj->m_pad_hit_p.size()<< std::endl;
    // std::cout<<"I\n"<< transObj->m_pad_hit_onlineId.size()<<std::endl;// m_max_rpc_hits_capacity
    // std::cout<< transObj->m_pad_hit_code.size()<<std::endl;    
    // std::cout<< transObj->m_pad_hit_x.size()<<std::endl;
    // std::cout<< transObj->m_pad_hit_y.size()<<std::endl;
    // std::cout<< transObj->m_pad_hit_z.size()<<std::endl;
    // std::cout<< transObj->m_pad_hit_r.size()<<std::endl;
    // std::cout<< transObj->m_pad_hit_p.size()<<std::endl;
    
    persObj->padv.reserve(transObj->pad_hit_onlineId().size());
    for (unsigned int i=0;i<transObj->pad_hit_onlineId().size();i++){    
        padhit pah;
        pah.m_pad_hit_onlineId = transObj->pad_hit_onlineId()[i];// m_max_rpc_hits_capacity
        pah.m_pad_hit_code     = transObj->pad_hit_code()[i];    
        pah.m_pad_hit_x        = transObj->pad_hit_x()[i];
        pah.m_pad_hit_y        = transObj->pad_hit_y()[i];
        pah.m_pad_hit_z        = transObj->pad_hit_z()[i];
        pah.m_pad_hit_r        = transObj->pad_hit_r()[i];
        pah.m_pad_hit_p        = transObj->pad_hit_p()[i];
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
    persObj->tgcInnVphi.reserve(transObj->tgc_Inn_phi_hit_in_seg().size());
    for (unsigned int i=0;i<transObj->tgc_Inn_phi_hit_in_seg().size();i++){
        tgc.m_tgc_phi_hit_in_seg = transObj->tgc_Inn_phi_hit_in_seg()[i];
        tgc.m_tgc_phi_hit_r      = transObj->tgc_Inn_phi_hit_r()[i];
        tgc.m_tgc_phi_hit_z      = transObj->tgc_Inn_phi_hit_z()[i];
        tgc.m_tgc_phi_hit_phi    = transObj->tgc_Inn_phi_hit_phi()[i];
        tgc.m_tgc_phi_hit_width  = transObj->tgc_Inn_phi_hit_width()[i];
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
      
    persObj->tgcInnVrho.reserve(transObj->tgc_Inn_rho_hit_in_seg().size());
    for (unsigned int i=0;i<transObj->tgc_Inn_rho_hit_in_seg().size();i++){
        tgc.m_tgc_rho_hit_in_seg = transObj->tgc_Inn_rho_hit_in_seg()[i]; // m_max_tgc_hits_capacity
        tgc.m_tgc_rho_hit_r      = transObj->tgc_Inn_rho_hit_r()[i];
        tgc.m_tgc_rho_hit_z      = transObj->tgc_Inn_rho_hit_z()[i];
        tgc.m_tgc_rho_hit_phi    = transObj->tgc_Inn_rho_hit_phi()[i];
        tgc.m_tgc_rho_hit_width  = transObj->tgc_Inn_rho_hit_width()[i];
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
     
    persObj->tgcMidVphi.reserve(transObj->tgc_Mid_phi_hit_in_seg().size());
    for (unsigned int i=0;i<transObj->tgc_Mid_phi_hit_in_seg().size();i++){
        tgc.m_tgc_phi_hit_in_seg = transObj->tgc_Mid_phi_hit_in_seg()[i];
        tgc.m_tgc_phi_hit_r      = transObj->tgc_Mid_phi_hit_r()[i];
        tgc.m_tgc_phi_hit_z      = transObj->tgc_Mid_phi_hit_z()[i];
        tgc.m_tgc_phi_hit_phi    = transObj->tgc_Mid_phi_hit_phi()[i];
        tgc.m_tgc_phi_hit_width  = transObj->tgc_Mid_phi_hit_width()[i];
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
    
    persObj->tgcMidVrho.reserve(transObj->tgc_Mid_rho_hit_in_seg().size());
    for (unsigned int i=0;i<transObj->tgc_Mid_rho_hit_in_seg().size();i++){
        tgc.m_tgc_rho_hit_in_seg = transObj->tgc_Mid_rho_hit_in_seg()[i];
        tgc.m_tgc_rho_hit_r      = transObj->tgc_Mid_rho_hit_r()[i];
        tgc.m_tgc_rho_hit_z      = transObj->tgc_Mid_rho_hit_z()[i];
        tgc.m_tgc_rho_hit_phi    = transObj->tgc_Mid_rho_hit_phi()[i];
        tgc.m_tgc_rho_hit_width  = transObj->tgc_Mid_rho_hit_width()[i];
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
    persObj->mdhv.reserve(transObj->mdt_onlineId().size());
    for (unsigned int i=0;i<transObj->mdt_onlineId().size();i++){
        mdthit mdh;
        mdh.m_mdt_onlineId      = transObj->mdt_onlineId()[i];// m_max_mdt_hits_capacity
        mdh.m_mdt_offlineId     = transObj->mdt_offlineId()[i];
        mdh.m_mdt_tube_r        = transObj->mdt_tube_r()[i];
        mdh.m_mdt_tube_z        = transObj->mdt_tube_z()[i];
        mdh.m_mdt_tube_residual = transObj->mdt_tube_residual()[i];
        mdh.m_mdt_tube_time     = transObj->mdt_tube_time()[i];
        mdh.m_mdt_tube_space    = transObj->mdt_tube_space()[i];
        mdh.m_mdt_tube_sigma    = transObj->mdt_tube_sigma()[i];
        persObj->mdhv.push_back(mdh);
        // std::cout<<i<<"\t"<<transObj->m_mdt_onlineId[i]<<"\t"<< transObj->m_mdt_offlineId[i] <<"\t"<< transObj->m_mdt_tube_r[i] <<"\t"<< transObj->m_mdt_tube_z[i] <<"\t"<< transObj->m_mdt_tube_sigma[i] << std::endl;
    }
   
    persObj->m_chamber_type_1 = transObj->chamber_type_1();
    persObj->m_chamber_type_2 = transObj->chamber_type_2();
    persObj->m_pos            = transObj->pos();
    
    persObj->m_aw       = transObj->aw();
    persObj->m_bw       = transObj->bw();
    persObj->m_zetaMin  = transObj->zetaMin();
    persObj->m_zetaMax  = transObj->zetaMax();
    persObj->m_radMin   = transObj->radMin();
    persObj->m_radMax   = transObj->radMax();
    persObj->m_etaMin   = transObj->etaMin();
    persObj->m_etaMax   = transObj->etaMax();
    persObj->m_st_phi   = transObj->st_phi();
    persObj->m_st_roads = transObj->st_roads();


}

