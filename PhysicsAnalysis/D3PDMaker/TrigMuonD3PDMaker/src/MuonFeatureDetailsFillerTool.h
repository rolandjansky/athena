// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
#ifndef TRIGMUOND3PDMAKER_MUONFEATUREDETAILSFILLERTOOL_H
#define TRIGMUOND3PDMAKER_MUONFEATUREDETAILSFILLERTOOL_H

// EDM include(s):
#include "TrigMuonEvent/MuonFeatureDetails.h"

// D3PD include(s):
#include "D3PDMakerUtils/BlockFillerTool.h"

// STL include
#include <vector>


#include <stdint.h>

namespace D3PD {

   /**
    *  @short Tool filling information about MuonFeatureDetails objects
    *
    * @author Takayuki Kanno
    *
    * $Revision: 332700 $
    * $Date$
    */
   class MuonFeatureDetailsFillerTool : public BlockFillerTool< MuonFeatureDetails > {

   public:
      /// Regular AlgTool constructor
      MuonFeatureDetailsFillerTool( const std::string& type, const std::string& name,
                                    const IInterface* parent );

      /// Function declaring the output variables
      virtual StatusCode book();
      /// Function filling the output variables
      virtual StatusCode fill( const MuonFeatureDetails& muon );

   private:
      //bool    m_writeRecoInfo;
      //bool    m_writeHitsInfo;

      //Followed MufastESD_Info.h in Trigger/TrigAnalysis/TrigMuonAnalysis/TrigMuonNtuple/
      unsigned int*  m_id;

      uint32_t* m_te_id;
      uint32_t* m_error;


      uint32_t* m_lvl1_id;
      uint32_t* m_lumi_block;
      uint32_t* m_muondetmask;
      uint32_t* m_RoIID;
      uint32_t* m_RoISystem;
      uint32_t* m_RoISubsystem;
      uint32_t* m_RoISector;
      uint32_t* m_RoINumber;
      uint32_t* m_RoIThreshold;
      float*    m_RoIEta;
      float*    m_RoIPhi;

      uint32_t* m_rpc_pad_error;
      uint32_t* m_tgc_rdo_error;

      float*    m_rpc1_x;
      float*    m_rpc1_y;
      float*    m_rpc1_z;

      float*    m_rpc2_x;
      float*    m_rpc2_y;
      float*    m_rpc2_z;

      float*    m_rpc3_x;
      float*    m_rpc3_y;
      float*    m_rpc3_z;

      float*    m_tgc_Mid1_eta;
      float*    m_tgc_Mid1_phi;
      float*    m_tgc_Mid1_r;
      float*    m_tgc_Mid1_z;

      float*    m_tgc_Mid2_eta;
      float*    m_tgc_Mid2_phi;
      float*    m_tgc_Mid2_r;
      float*    m_tgc_Mid2_z;

      float*    m_tgc_Mid_rho_chi2;
      long*     m_tgc_Mid_rho_N;
      float*    m_tgc_Mid_phi_chi2;
      long*     m_tgc_Mid_phi_N;

      float*    m_tgc_Inn_eta;
      float*    m_tgc_Inn_phi;
      float*    m_tgc_Inn_r;
      float*    m_tgc_Inn_z;
      float*    m_tgc_Inn_rho_std;
      long*     m_tgc_Inn_rho_N;
      float*    m_tgc_Inn_phi_std;
      long*     m_tgc_Inn_phi_N;

      float*    m_tgc_PT;

      float* m_mdt_Inner_slope;
      float* m_mdt_Inner_intercept;
      float* m_mdt_Inner_Z;
      float* m_mdt_Inner_R;
      float* m_mdt_Inner_fit_chi;
      float* m_mdt_Middle_slope;
      float* m_mdt_Middle_intercept;
      float* m_mdt_Middle_Z;
      float* m_mdt_Middle_R;
      float* m_mdt_Middle_fit_chi;
      float* m_mdt_Outer_slope;
      float* m_mdt_Outer_intercept;
      float* m_mdt_Outer_Z;
      float* m_mdt_Outer_R;
      float* m_mdt_Outer_fit_chi;

      int*   m_Address;
      float* m_Sagitta;
      float* m_Radius;
      float* m_Slope;
      float* m_Intercept;
      float* m_Alpha;
      float* m_Beta;
      float* m_DeltaR;
      float* m_Speed_over_c;
      float* m_Eta;
      float* m_PhiMap;
      float* m_Phi;
      float* m_PhiDir;
      float* m_Pt;
      float* m_Charge;

      long*  m_type;
      float* m_t_x_center;
      float* m_t_y_center;
      float* m_t_radius;
      float* m_t_ap;
      float* m_t_aphi;
      float* m_t_phi;
      float* m_extrInnerEta;
      float* m_extrInnerPhi;

      std::vector<int>*   m_tr_chamber_type_1;
      std::vector<int>*   m_tr_chamber_type_2;
      std::vector<int>*   m_tr_pos;
      std::vector<float>* m_tr_aw;
      std::vector<float>* m_tr_bw;
      std::vector<float>* m_tr_zetaMin;
      std::vector<float>* m_tr_zetaMax;
      std::vector<float>* m_tr_radMin;
      std::vector<float>* m_tr_radMax;
      std::vector<float>* m_tr_etaMin;
      std::vector<float>* m_tr_etaMax;
      std::vector<float>* m_tr_st_phi;
      std::vector<float>* m_tr_st_roads;

      std::vector<int>*   m_ov_chamber_type_1;
      std::vector<int>*   m_ov_chamber_type_2;
      std::vector<int>*   m_ov_pos;
      std::vector<float>* m_ov_aw;
      std::vector<float>* m_ov_bw;
      std::vector<float>* m_ov_zetaMin;
      std::vector<float>* m_ov_zetaMax;
      std::vector<float>* m_ov_radMin;
      std::vector<float>* m_ov_radMax;
      std::vector<float>* m_ov_etaMin;
      std::vector<float>* m_ov_etaMax;
      std::vector<float>* m_ov_st_phi;
      std::vector<float>* m_ov_st_roads;

      std::vector<uint32_t>* m_extension0;
      std::vector<uint32_t>* m_extension1;
      std::vector<uint32_t>* m_extension2;
      std::vector<uint32_t>* m_extension3;
      std::vector<uint32_t>* m_extension4;
      std::vector<uint32_t>* m_extension5;
      std::vector<uint32_t>* m_extension6;
      std::vector<uint32_t>* m_extension7;
      std::vector<uint32_t>* m_extension8;
      std::vector<uint32_t>* m_extension9;

      std::vector<uint32_t>* m_lvl1_emulation;

      std::vector<uint32_t>* m_rob_id;
      std::vector<uint32_t>* m_csm_id;
      std::vector<uint32_t>* m_csm_size;
      std::vector<uint32_t>* m_csm_error;
      std::vector<uint32_t>* m_removed_rob_id;
      std::vector<uint32_t>* m_removed_csm_id;

      std::vector<uint32_t>* m_pad_hit_onlineId;
      std::vector<uint32_t>* m_pad_hit_code;
      std::vector<float>*    m_pad_hit_x;
      std::vector<float>*    m_pad_hit_y;
      std::vector<float>*    m_pad_hit_z;
      std::vector<float>*    m_pad_hit_r;
      std::vector<float>*    m_pad_hit_p;

      std::vector<float>*    m_tgc_Inn_rho_hit_phi;
      std::vector<float>*    m_tgc_Inn_rho_hit_r;
      std::vector<float>*    m_tgc_Inn_rho_hit_z;
      std::vector<float>*    m_tgc_Inn_rho_hit_width;
      std::vector<uint32_t>* m_tgc_Inn_rho_hit_in_seg;
      std::vector<float>*    m_tgc_Inn_phi_hit_phi;
      std::vector<float>*    m_tgc_Inn_phi_hit_r;
      std::vector<float>*    m_tgc_Inn_phi_hit_z;
      std::vector<float>*    m_tgc_Inn_phi_hit_width;
      std::vector<uint32_t>* m_tgc_Inn_phi_hit_in_seg;
      std::vector<float>*    m_tgc_Mid_rho_hit_phi;
      std::vector<float>*    m_tgc_Mid_rho_hit_r;
      std::vector<float>*    m_tgc_Mid_rho_hit_z;
      std::vector<float>*    m_tgc_Mid_rho_hit_width;
      std::vector<uint32_t>* m_tgc_Mid_rho_hit_in_seg;
      std::vector<float>*    m_tgc_Mid_phi_hit_phi;
      std::vector<float>*    m_tgc_Mid_phi_hit_r;
      std::vector<float>*    m_tgc_Mid_phi_hit_z;
      std::vector<float>*    m_tgc_Mid_phi_hit_width;
      std::vector<uint32_t>* m_tgc_Mid_phi_hit_in_seg;

      std::vector<uint32_t>* m_mdt_onlineId;
      std::vector<uint32_t>* m_mdt_offlineId;
      std::vector<float>*    m_mdt_tube_r;
      std::vector<float>*    m_mdt_tube_z;
      std::vector<float>*    m_mdt_tube_residual;
      std::vector<float>*    m_mdt_tube_time;
      std::vector<float>*    m_mdt_tube_space;
      std::vector<float>*    m_mdt_tube_sigma;

   }; // class MuonFeatureDetailsFillerTool

} // namespace D3PD

#endif // TRIGMUOND3PDMAKER_MUONFEATUREDETAILSFILLERTOOL_H
