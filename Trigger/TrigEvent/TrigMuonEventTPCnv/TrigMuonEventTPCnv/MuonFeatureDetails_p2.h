/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENTTPCNV_MUONFEATUREDETAILS_P2_H
#define TRIGMUONEVENTTPCNV_MUONFEATUREDETAILS_P2_H

#include "TrigMuonEvent/MuonFeatureDetails.h"
#include <stdint.h>

// transient side of this class is completely wrongly organized. it's all in separate vectors.
// persistant side is now a bit more optimal (less small vectors) but not much was gained in speed as it again fills all the 
// vectors on the transient side. Hopefully in time this class will dissapear.


struct padhit{ 
    unsigned int m_pad_hit_onlineId;
    unsigned int m_pad_hit_code;    
    float    m_pad_hit_x;
    float    m_pad_hit_y;
    float    m_pad_hit_z;
    float    m_pad_hit_r;
    float    m_pad_hit_p;
};

struct mdthit{
    unsigned int m_mdt_onlineId;
    unsigned int m_mdt_offlineId;
    float m_mdt_tube_r;
    float m_mdt_tube_z;
    float m_mdt_tube_residual;
    float m_mdt_tube_time;
    float m_mdt_tube_space;
    float m_mdt_tube_sigma;
};

struct tgchit{
    unsigned int m_tgc_rho_hit_in_seg;
    unsigned int m_tgc_phi_hit_in_seg;
    float    m_tgc_rho_hit_phi;
    float    m_tgc_rho_hit_r;
    float    m_tgc_rho_hit_z;
    float    m_tgc_rho_hit_width;
    float    m_tgc_phi_hit_phi;
    float    m_tgc_phi_hit_r;
    float    m_tgc_phi_hit_z;
    float    m_tgc_phi_hit_width;
};

class MuonFeatureDetails_p2
{
  friend class MuonFeatureDetailsCnv;
 
 public:
 
  MuonFeatureDetails_p2() {}
  ~MuonFeatureDetails_p2(){}

  // private:

/**********************************************
* very important! don't change anything in this file.
* not even comments. Variables will be stored in the order in which they are listed in comments.
* If you want to change something make a new version of MuonFeatureDetailsContainer and corresponding MuonFeatureDetails and their converters.
**********************************************/



    MuonFeatureDetails::AlgoId   m_id;
    int   m_Address;
    
    // uint32_t m_te_id;
    // uint32_t m_error;
    // 
    // uint32_t m_lvl1_id;
    // uint32_t m_lumi_block;
    // uint32_t m_muondetmask; 
    // uint32_t m_RoIID;
    // uint32_t m_RoISystem;
    // uint32_t m_RoISubsystem; 
    // uint32_t m_RoISector;
    // uint32_t m_RoINumber;
    // uint32_t m_RoIThreshold;
    // 
    // uint32_t m_rpc_pad_error;
    // uint32_t m_tgc_rdo_error;
    
    uint32_t m_allTheUInts[13];
    
    // long  m_tgc_Mid_rho_N;
    // long  m_tgc_Mid_phi_N;
    // long  m_tgc_Inn_rho_N;
    // long  m_tgc_Inn_phi_N;
    // long  m_type;
    
    long  m_allTheLongs[5];
    
    // float    m_RoIEta;
    // float    m_RoIPhi;  
    // 
    // float    m_rpc1_x;
    // float    m_rpc1_y;
    // float    m_rpc1_z;
    //  
    // float    m_rpc2_x;
    // float    m_rpc2_y;
    // float    m_rpc2_z;
    // 
    // float    m_rpc3_x;
    // float    m_rpc3_y;
    // float    m_rpc3_z; 
    // 
    // float    m_tgc_Mid1_eta;
    // float    m_tgc_Mid1_phi;
    // float    m_tgc_Mid1_r;
    // float    m_tgc_Mid1_z;
    // 
    // float    m_tgc_Mid2_eta;
    // float    m_tgc_Mid2_phi;
    // float    m_tgc_Mid2_r;
    // float    m_tgc_Mid2_z;
    // 
    // float    m_tgc_Mid_rho_chi2;
    // float    m_tgc_Mid_phi_chi2;
    // 
    // float    m_tgc_Inn_eta;
    // float    m_tgc_Inn_phi;
    // float    m_tgc_Inn_r;
    // float    m_tgc_Inn_z;
    // float    m_tgc_Inn_rho_std;
    // float    m_tgc_Inn_phi_std;
    // 
    // float    m_tgc_PT;
    // 
    // float m_t_x_center;
    // float m_t_y_center;
    // float m_t_radius;
    // float m_t_ap;
    // float m_t_aphi;
    // float m_t_phi;
    // float m_extrInnerEta;
    // float m_extrInnerPhi;
    // 
    // float m_mdt_Inner_slope;
    // float m_mdt_Inner_intercept;
    // float m_mdt_Inner_Z;
    // float m_mdt_Inner_R;
    // float m_mdt_Inner_fit_chi;
    // float m_mdt_Middle_slope;
    // float m_mdt_Middle_intercept;
    // float m_mdt_Middle_Z;
    // float m_mdt_Middle_R;
    // float m_mdt_Middle_fit_chi;
    // float m_mdt_Outer_slope;
    // float m_mdt_Outer_intercept;
    // float m_mdt_Outer_Z;
    // float m_mdt_Outer_R;
    // float m_mdt_Outer_fit_chi;
    // 
    // float m_Sagitta;
    // float m_Radius;
    // float m_Slope;
    // float m_Intercept;
    // float m_Alpha;
    // float m_Beta;
    // float m_DeltaR;
    // float m_Speed_over_c;
    // float m_Eta;
    // float m_PhiMap;
    // float m_Phi;
    // float m_PhiDir;
    // float m_Pt;
    // float m_Charge;
    
    float m_allTheFloats[65];
    
    std::vector<uint32_t> m_extension0;
    std::vector<uint32_t> m_extension1;
    std::vector<uint32_t> m_extension2;
    std::vector<uint32_t> m_extension3;
    std::vector<uint32_t> m_extension4;
    std::vector<uint32_t> m_extension5;
    std::vector<uint32_t> m_extension6;
    std::vector<uint32_t> m_extension7;
    std::vector<uint32_t> m_extension8;
    std::vector<uint32_t> m_extension9;
    std::vector<uint32_t> m_lvl1_emulation;
    std::vector<uint32_t> m_rob_id;
    std::vector<uint32_t> m_csm_id;
    std::vector<uint32_t> m_csm_size;
    std::vector<uint32_t> m_csm_error;
    std::vector<uint32_t> m_removed_rob_id;
    std::vector<uint32_t> m_removed_csm_id;
    
    // std::vector<uint32_t> m_pad_hit_onlineId; replaced by vector<padhit>
    // std::vector<uint32_t> m_pad_hit_code;    
    // std::vector<float>    m_pad_hit_x;
    // std::vector<float>    m_pad_hit_y;
    // std::vector<float>    m_pad_hit_z;
    // std::vector<float>    m_pad_hit_r;
    // std::vector<float>    m_pad_hit_p;
    
    std::vector<padhit> padv;
    
    // std::vector<uint32_t> m_tgc_Inn_rho_hit_in_seg; replaced by vector<tgchit>
    // std::vector<float>    m_tgc_Inn_rho_hit_phi;
    // std::vector<float>    m_tgc_Inn_rho_hit_r;
    // std::vector<float>    m_tgc_Inn_rho_hit_z;
    // std::vector<float>    m_tgc_Inn_rho_hit_width;
    
    std::vector<tgchit> tgcInnVrho;
    
    // std::vector<uint32_t> m_tgc_Inn_phi_hit_in_seg;
    // std::vector<float>    m_tgc_Inn_phi_hit_phi;
    // std::vector<float>    m_tgc_Inn_phi_hit_r;
    // std::vector<float>    m_tgc_Inn_phi_hit_z;
    // std::vector<float>    m_tgc_Inn_phi_hit_width;
    
    std::vector<tgchit> tgcInnVphi;
    
    // std::vector<uint32_t> m_tgc_Mid_rho_hit_in_seg;
    // std::vector<float>    m_tgc_Mid_rho_hit_phi;
    // std::vector<float>    m_tgc_Mid_rho_hit_r;
    // std::vector<float>    m_tgc_Mid_rho_hit_z;
    // std::vector<float>    m_tgc_Mid_rho_hit_width;
    
    std::vector<tgchit> tgcMidVrho;
    
    // std::vector<uint32_t> m_tgc_Mid_phi_hit_in_seg;
    // std::vector<float>    m_tgc_Mid_phi_hit_phi;
    // std::vector<float>    m_tgc_Mid_phi_hit_r;
    // std::vector<float>    m_tgc_Mid_phi_hit_z;
    // std::vector<float>    m_tgc_Mid_phi_hit_width;
    
    std::vector<tgchit> tgcMidVphi;
    
    // std::vector<uint32_t> m_mdt_onlineId; replaced by vector<mdthit>
    // std::vector<uint32_t> m_mdt_offlineId;
    // std::vector<float>    m_mdt_tube_r;
    // std::vector<float>    m_mdt_tube_z;
    // std::vector<float>    m_mdt_tube_residual;
    // std::vector<float>    m_mdt_tube_time;
    // std::vector<float>    m_mdt_tube_space;
    // std::vector<float>    m_mdt_tube_sigma;
    
    std::vector<mdthit> mdhv;

    std::vector< std::vector<int> >   m_chamber_type_1;
    std::vector< std::vector<int> >   m_chamber_type_2;
    std::vector< std::vector<int> >   m_pos;
    std::vector< std::vector<float> > m_aw;
    std::vector< std::vector<float> > m_bw;
    std::vector< std::vector<float> > m_zetaMin;
    std::vector< std::vector<float> > m_zetaMax;
    std::vector< std::vector<float> > m_radMin;
    std::vector< std::vector<float> > m_radMax;
    std::vector< std::vector<float> > m_etaMin;
    std::vector< std::vector<float> > m_etaMax;
    std::vector< std::vector<float> > m_st_phi;
    std::vector< std::vector<float> > m_st_roads;

};

#endif

