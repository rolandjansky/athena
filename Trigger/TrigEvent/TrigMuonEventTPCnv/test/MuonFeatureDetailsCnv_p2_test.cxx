/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file TrigMuonEventTPCnv/test/MuonFeatureDetailsCnv_p2_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Feb, 2016
 * @brief Tests for MuonFeatureDetailsCnv_p2.
 */


#undef NDEBUG
#include "TrigMuonEventTPCnv/MuonFeatureDetailsCnv_p2.h"
#include "GaudiKernel/MsgStream.h"
#include <cassert>
#include <iostream>


void compare (const MuonFeatureDetails& p1,
              const MuonFeatureDetails& p2)
{
  //assert (p1.extension_capacity() == p2.extension_capacity());
  //assert (p1.max_rob_capacity() == p2.max_rob_capacity());
  //assert (p1.max_csm_capacity() == p2.  max_csm_capacity());
  //assert (p1.max_lvl1_emu_capacity() == p2.max_lvl1_emu_capacity());
  //assert (p1.max_rpc_hits_capacity() == p2.max_rpc_hits_capacity());
  //assert (p1.max_tgc_hits_capacity() == p2.max_tgc_hits_capacity());
  //assert (p1.max_mdt_hits_capacity() == p2.max_mdt_hits_capacity());

  assert (p1.id() == p2.id());
  assert (p1.te_id() == p2.te_id());
  assert (p1.error() == p2.error());

  assert (p1.lvl1_id() == p2.lvl1_id());
  assert (p1.lumi_block() == p2.lumi_block());
  assert (p1.muondetmask() == p2.muondetmask());
  assert (p1.roi_id() == p2.roi_id());
  assert (p1.roi_system() == p2.roi_system());
  assert (p1.roi_subsystem() == p2.roi_subsystem());
  assert (p1.roi_sector() == p2.roi_sector());
  assert (p1.roi_number() == p2.roi_number());
  assert (p1.roi_threshold() == p2.roi_threshold());
  assert (p1.roi_eta() == p2.roi_eta());
  assert (p1.roi_phi() == p2.roi_phi());
    
  assert (p1.rpc_pad_error() == p2.rpc_pad_error());
  assert (p1.tgc_rdo_error() == p2.tgc_rdo_error());
    
  assert (p1.rpc1_x() == p2.rpc1_x());
  assert (p1.rpc1_y() == p2.rpc1_y());
  assert (p1.rpc1_z() == p2.rpc1_z());
     
  assert (p1.rpc2_x() == p2.rpc2_x());
  assert (p1.rpc2_y() == p2.rpc2_y());
  assert (p1.rpc2_z() == p2.rpc2_z());
    
  assert (p1.rpc3_x() == p2.rpc3_x());
  assert (p1.rpc3_y() == p2.rpc3_y());
  assert (p1.rpc3_z() == p2.rpc3_z());
    
  assert (p1.tgc_Mid1_eta() == p2.tgc_Mid1_eta());
  assert (p1.tgc_Mid1_phi() == p2.tgc_Mid1_phi());
  assert (p1.tgc_Mid1_r() == p2.tgc_Mid1_r());
  assert (p1.tgc_Mid1_z() == p2.tgc_Mid1_z());
    
  assert (p1.tgc_Mid2_eta() == p2.tgc_Mid2_eta());
  assert (p1.tgc_Mid2_phi() == p2.tgc_Mid2_phi());
  assert (p1.tgc_Mid2_r() == p2.tgc_Mid2_r());
  assert (p1.tgc_Mid2_z() == p2.tgc_Mid2_z());
    
  assert (p1.tgc_Mid_rho_chi2() == p2.tgc_Mid_rho_chi2());
  assert (p1.tgc_Mid_rho_N() == p2.tgc_Mid_rho_N());
  assert (p1.tgc_Mid_phi_chi2() == p2.tgc_Mid_phi_chi2());
  assert (p1.tgc_Mid_phi_N() == p2.tgc_Mid_phi_N());
    
  assert (p1.tgc_Inn_eta() == p2.tgc_Inn_eta());
  assert (p1.tgc_Inn_phi() == p2.tgc_Inn_phi());
  assert (p1.tgc_Inn_r() == p2.tgc_Inn_r());
  assert (p1.tgc_Inn_z() == p2.tgc_Inn_z());
  assert (p1.tgc_Inn_rho_std() == p2.tgc_Inn_rho_std());
  assert (p1.tgc_Inn_rho_N() == p2.tgc_Inn_rho_N());
  assert (p1.tgc_Inn_phi_std() == p2.tgc_Inn_phi_std());
  assert (p1.tgc_Inn_phi_N() == p2.tgc_Inn_phi_N());
    
  assert (p1.tgc_PT() == p2.tgc_PT());
    
  assert (p1.type() == p2.type());
  assert (p1.t_x_center() == p2.t_x_center());
  assert (p1.t_y_center() == p2.t_y_center());
  assert (p1.t_radius() == p2.t_radius());
  assert (p1.t_ap() == p2.t_ap());
  assert (p1.t_aphi() == p2.t_aphi());
  assert (p1.t_phi() == p2.t_phi());
  assert (p1.extrInnerEta() == p2.extrInnerEta());
  assert (p1.extrInnerPhi() == p2.extrInnerPhi());
    
  assert (p1.chamber_type_1() == p2.chamber_type_1());
  assert (p1.chamber_type_2() == p2.chamber_type_2());
  assert (p1.pos() == p2.pos());
  assert (p1.aw() == p2.aw());
  assert (p1.bw() == p2.bw());
  assert (p1.zetaMin() == p2.zetaMin());
  assert (p1.zetaMax() == p2.zetaMax());
  assert (p1.radMin() == p2.radMin());
  assert (p1.radMax() == p2.radMax());
  assert (p1.etaMin() == p2.etaMin());
  assert (p1.etaMax() == p2.etaMax());
  assert (p1.st_phi() == p2.st_phi());
  assert (p1.st_roads() == p2.st_roads());
    
  assert (p1.mdt_Inner_slope() == p2.mdt_Inner_slope());
  assert (p1.mdt_Inner_intercept() == p2.mdt_Inner_intercept());
  assert (p1.mdt_Inner_Z() == p2.mdt_Inner_Z());
  assert (p1.mdt_Inner_R() == p2.mdt_Inner_R());
  assert (p1.mdt_Inner_fit_chi() == p2.mdt_Inner_fit_chi());
  assert (p1.mdt_Middle_slope() == p2.mdt_Middle_slope());
  assert (p1.mdt_Middle_intercept() == p2.mdt_Middle_intercept());
  assert (p1.mdt_Middle_Z() == p2.mdt_Middle_Z());
  assert (p1.mdt_Middle_R() == p2.mdt_Middle_R());
  assert (p1.mdt_Middle_fit_chi() == p2.mdt_Middle_fit_chi());
  assert (p1.mdt_Outer_slope() == p2.mdt_Outer_slope());
  assert (p1.mdt_Outer_intercept() == p2.mdt_Outer_intercept());
  assert (p1.mdt_Outer_Z() == p2.mdt_Outer_Z());
  assert (p1.mdt_Outer_R() == p2.mdt_Outer_R());
  assert (p1.mdt_Outer_fit_chi() == p2.mdt_Outer_fit_chi());
    
  assert (p1.Address() == p2.Address());
  assert (p1.Sagitta() == p2.Sagitta());
  assert (p1.Radius() == p2.Radius());
  assert (p1.Slope() == p2.Slope());
  assert (p1.Intercept() == p2.Intercept());
  assert (p1.Alpha() == p2.Alpha());
  assert (p1.Beta() == p2.Beta());
  assert (p1.DeltaR() == p2.DeltaR());
  assert (p1.Speed_over_c() == p2.Speed_over_c());
  assert (p1.Eta() == p2.Eta());
  assert (p1.PhiMap() == p2.PhiMap());
  assert (p1.Phi() == p2.Phi());
  assert (p1.PhiDir() == p2.PhiDir());
  assert (p1.Pt() == p2.Pt());
  assert (p1.Charge() == p2.Charge());
    
  assert (p1.extension0() == p2.extension0());
  assert (p1.extension1() == p2.extension1());
  assert (p1.extension2() == p2.extension2());
  assert (p1.extension3() == p2.extension3());
  assert (p1.extension4() == p2.extension4());
  assert (p1.extension5() == p2.extension5());
  assert (p1.extension6() == p2.extension6());
  assert (p1.extension7() == p2.extension7());
  assert (p1.extension8() == p2.extension8());
  assert (p1.extension9() == p2.extension9());
    
  assert (p1.lvl1_emulation() == p2.lvl1_emulation());
    
  assert (p1.rob_id() == p2.rob_id());
  assert (p1.csm_id() == p2.csm_id());
  assert (p1.csm_size() == p2.csm_size());
  assert (p1.csm_error() == p2.csm_error());
  assert (p1.removed_rob_id() == p2.removed_rob_id());
  assert (p1.removed_csm_id() == p2.removed_csm_id());
    
  assert (p1.pad_hit_onlineId() == p2.pad_hit_onlineId());
  assert (p1.pad_hit_code() == p2.pad_hit_code());
  assert (p1.pad_hit_x() == p2.pad_hit_x());
  assert (p1.pad_hit_y() == p2.pad_hit_y());
  assert (p1.pad_hit_z() == p2.pad_hit_z());
  assert (p1.pad_hit_r() == p2.pad_hit_r());
  assert (p1.pad_hit_p() == p2.pad_hit_p());
    
  assert (p1.tgc_Inn_rho_hit_phi() == p2.tgc_Inn_rho_hit_phi());
  assert (p1.tgc_Inn_rho_hit_r() == p2.tgc_Inn_rho_hit_r());
  assert (p1.tgc_Inn_rho_hit_z() == p2.tgc_Inn_rho_hit_z());
  assert (p1.tgc_Inn_rho_hit_width() == p2.tgc_Inn_rho_hit_width());
  assert (p1.tgc_Inn_rho_hit_in_seg() == p2.tgc_Inn_rho_hit_in_seg());
  assert (p1.tgc_Inn_phi_hit_phi() == p2.tgc_Inn_phi_hit_phi());
  assert (p1.tgc_Inn_phi_hit_r() == p2.tgc_Inn_phi_hit_r());
  assert (p1.tgc_Inn_phi_hit_z() == p2.tgc_Inn_phi_hit_z());
  assert (p1.tgc_Inn_phi_hit_width() == p2.tgc_Inn_phi_hit_width());
  assert (p1.tgc_Inn_phi_hit_in_seg() == p2.tgc_Inn_phi_hit_in_seg());
  assert (p1.tgc_Mid_rho_hit_phi() == p2.tgc_Mid_rho_hit_phi());
  assert (p1.tgc_Mid_rho_hit_r() == p2.tgc_Mid_rho_hit_r());
  assert (p1.tgc_Mid_rho_hit_z() == p2.tgc_Mid_rho_hit_z());
  assert (p1.tgc_Mid_rho_hit_width() == p2.tgc_Mid_rho_hit_width());
  assert (p1.tgc_Mid_rho_hit_in_seg() == p2.tgc_Mid_rho_hit_in_seg());
  assert (p1.tgc_Mid_phi_hit_phi() == p2.tgc_Mid_phi_hit_phi());
  assert (p1.tgc_Mid_phi_hit_r() == p2.tgc_Mid_phi_hit_r());
  assert (p1.tgc_Mid_phi_hit_z() == p2.tgc_Mid_phi_hit_z());
  assert (p1.tgc_Mid_phi_hit_width() == p2.tgc_Mid_phi_hit_width());
  assert (p1.tgc_Mid_phi_hit_in_seg() == p2.tgc_Mid_phi_hit_in_seg());
    
  assert (p1.mdt_onlineId() == p2.mdt_onlineId());
  assert (p1.mdt_offlineId() == p2.mdt_offlineId());
  assert (p1.mdt_tube_r() == p2.mdt_tube_r());
  assert (p1.mdt_tube_z() == p2.mdt_tube_z());
  assert (p1.mdt_tube_residual() == p2.mdt_tube_residual());
  assert (p1.mdt_tube_time() == p2.mdt_tube_time());
  assert (p1.mdt_tube_space() == p2.mdt_tube_space());
  assert (p1.mdt_tube_sigma() == p2.mdt_tube_sigma());
}


void testit (const MuonFeatureDetails& trans1)
{
  MsgStream log (0, "test");
  MuonFeatureDetailsCnv_p2 cnv;
  MuonFeatureDetails_p2 pers;
  cnv.transToPers (&trans1, &pers, log);
  MuonFeatureDetails trans2;
  cnv.persToTrans (&pers, &trans2, log);

  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  MuonFeatureDetails trans1 (MuonFeatureDetails::MUONID,
                             1, // te_id
                             2, // lvl1_id
                             3, // lumi_block
                             4, // muondetmask
                             5, // RoIID
                             6, // RoISystem
                             7, // RoISubsystem
                             8, // RoISector
                             9, // RoINumber
                             10, // RoIThreshold
                             11.5, // RoIEta
                             12.5, // RoIPhi
                             13, // extension_capacity
                             14, // max_rob_capacity
                             15, // max_csm_capacity
                             16, // max_lvl1_emu_capacity
                             17, // max_rpc_hits_capacity
                             18, // max_tgc_hits_capacity
                             19  // max_mdt_hit_capacity
                             );
  trans1.setError(20);
  trans1.setRpcPadError(21);
  trans1.setTgcPadError(22);
    
  trans1.setRpc1(23.5, 24.5, 25.5);
  trans1.setRpc2(26.5, 27.5, 28.5);
  trans1.setRpc3(29.5, 30.5, 31.5);
    
  trans1.setTgcMid1(32.5, 33.5, 34.5, 35.5);
  trans1.setTgcMid1(36.5, 37.5, 36.5, 37.5);
  trans1.setTgcMidF(38.5, 39  , 40.5, 41);
  trans1.setTgcInn (42.5, 43.5, 44.5, 45.5);
  trans1.setTgcInnF(46.5, 47  , 48.5, 49);
  trans1.setTgcPt(50.5);
    
  trans1.setType(51);
  trans1.setT_x_center(52.5);
  trans1.setT_y_center(53.5);
  trans1.setT_radius(54.5);
  trans1.setT_ap(55.5);
  trans1.setT_aphi(56.5);
  trans1.setT_phi(57.5);
  trans1.setExtrInnerEta(58.5);
  trans1.setExtrInnerPhi(59.5);

  for (int i=0; i<2; i++) {
    for (int j=0; j<4; j++) {
      trans1.setChamber_type_1 (i, j, 60 + (j+i*5)*500);
      trans1.setChamber_type_2 (i, j, 61 + (j+i*5)*500);
      trans1.setPos (i, j, 62 + (j+i*5)*100);
      trans1.setAw (i, j, 63.5 + (j+i*5)*100);
      trans1.setBw (i, j, 64.5 + (j+i*5)*100);
      trans1.setZetaMin (i, j, 65.5 + (j+i*5)*100);
      trans1.setZetaMax (i, j, 66.5 + (j+i*5)*100);
      trans1.setRadMin (i, j, 67.5 + (j+i*5)*100);
      trans1.setRadMax (i, j, 68.5 + (j+i*5)*100);
      trans1.setEtaMin (i, j, 69.5 + (j+i*5)*100);
      trans1.setEtaMax (i, j, 70.5 + (j+i*5)*100);
      trans1.setSt_phi (i, j, 71.5 + (j+i*5)*100);
      trans1.setSt_roads (i, j, 72.5 + (j+i*5)*100);
    }
  }

  trans1.setSP1(73.5, 74.5, 75.5, 76.5, 77.5);
  trans1.setSP2(78.5, 79.5, 80.5, 81.5, 82.5);
  trans1.setSP3(83.5, 84.5, 85.5, 86.5, 87.5);
    
  trans1.setAddress(88);
  trans1.setSagitta(89.5);
  trans1.setRadius(90.5);
  trans1.setSlope(91.5);
  trans1.setIntercept(92.5);
  trans1.setAlpha(93.5);
  trans1.setBeta(94.5);
  trans1.setDeltaR(95.5);
  trans1.setSpeed_over_c(96.5);
  trans1.setEta(97.5);
  trans1.setPhiMap(98.5);
  trans1.setPhi(99.5);
  trans1.setPhiDir(100.5);
  trans1.setPt(101.5);
  trans1.setCharge(102.5);
  
  trans1.setExtension0(std::vector<uint32_t> {103});
  trans1.setExtension1(std::vector<uint32_t> {104, 105});
  trans1.setExtension2(std::vector<uint32_t> {106, 107, 108});
  trans1.setExtension3(std::vector<uint32_t> {109, 110});
  trans1.setExtension4(std::vector<uint32_t> {111});
  trans1.setExtension5(std::vector<uint32_t> {112, 113});
  trans1.setExtension6(std::vector<uint32_t> {114, 115, 116});
  trans1.setExtension7(std::vector<uint32_t> {117, 118, 119});
  trans1.setExtension8(std::vector<uint32_t> {120, 121});
  trans1.setExtension9(std::vector<uint32_t> {122});

  {
    std::vector<uint32_t> v;
    for (int i=0; i<20; i++)
      v.push_back(123+i);
    trans1.setLvl1Emulation(std::vector<uint32_t> (v));
  }


  for (int i=0; i < 10; i++) {
    trans1.setPadHit (124 + i*100,
                      125 + i*100,
                      126.5 + i*100,
                      127.5 + i*100,
                      128.5 + i*100,
                      129.5 + i*100,
                      130.5 + i*100);
    trans1.setTgcMidRhoHit (131.5 + i*100,
                            132.5 + i*100,
                            133.5 + i*100,
                            134.5 + i*100,
                            135 + i*100);
    trans1.setTgcMidPhiHit (136.5 + i*100,
                            137.5 + i*100,
                            138.5 + i*100,
                            139.5 + i*100,
                            140 + i*100);
    trans1.setTgcInnRhoHit(141.5 + i*100,
                           142.5 + i*100,
                           143.5 + i*100,
                           144.5 + i*100,
                           145 + i*100);
    trans1.setTgcInnPhiHit(146.5 + i*100,
                           147.5 + i*100,
                           148.5 + i*100,
                           149.5 + i*100,
                           150 + i*100);
    trans1.setMdtHit(151 + i*100,
                     152 + i*100,
                     153.5 + i*100,
                     154.5 + i*100,
                     155.5 + i*100,
                     156.5 + i*100,
                     157.5 + i*100,
                     158.5 + i*100);

    trans1.setRequestedRob(159 + i*100);
    trans1.setConvertedCsm(160 + i*100, 161 + i*100);
    trans1.setCsmError(162 + i*100);
    trans1.setRemovedRob(163 + i*100);
    trans1.setRemovedCsm(164 + i*100);
  }
    
  testit (trans1);
}


int main()
{
  test1();
  return 0;
}
