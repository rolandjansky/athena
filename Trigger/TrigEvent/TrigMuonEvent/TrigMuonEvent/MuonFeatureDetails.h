/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_MUON_FEATURE_DETAILS_H
#define TRIGMUONEVENT_MUON_FEATURE_DETAILS_H


#include <vector>
#include <string>
#include <iostream>
#include <stdint.h>

// Gaudi/Athena include(s):
#include "AthenaKernel/CLASS_DEF.h"
#include "AthContainers/DataVector.h"
#include "GaudiKernel/MsgStream.h"

//class MsgStream;

class MuonFeatureDetails {

public:
    enum AlgoId{GEV900ID=0,   MUONID=1,     HALOID=2,    COSMICID=3, 
                LOOSE_HM=10,  MEDIUM_HM=11, TIGHT_HM=12, LOOSE_LM=13, 
		MEDIUM_LM=14, TIGHT_LM=15,
		NULLID=99999};
    
    MuonFeatureDetails();
    MuonFeatureDetails(AlgoId id, 
                       uint32_t te_id,
                       uint32_t lvl1_id,
		       uint32_t lumi_block,
		       uint32_t muondetmask,
		       uint32_t RoIID, 
		       uint32_t RoISystem,
		       uint32_t RoISubsystem, 
		       uint32_t RoISector,
		       uint32_t RoINumber,
		       uint32_t RoIThreshold,
		       float    RoIEta,
		       float    RoIPhi,
		       int extension_capacity = 0,
		       int max_rob_capacity = 10,
		       int max_csm_capacity = 30,
		       int max_lvl1_emu_capacity = 20,
		       int max_rpc_hits_capacity = 100,
		       int max_tgc_hits_capacity = 50,
		       int max_mdt_hits_capacity = 100);
    

    MuonFeatureDetails(const MuonFeatureDetails& details);
    MuonFeatureDetails& operator=(const MuonFeatureDetails& source);


    // helper methods
    std::string str(void) const;
    bool isEqual(const MuonFeatureDetails& d) const;
    bool hasSameContentOf(const MuonFeatureDetails& d) const;
    bool hasSameRoIOf(const MuonFeatureDetails& d) const;
    
    bool hasSameTrackReco(const MuonFeatureDetails& d) const;
    bool operator==(const MuonFeatureDetails& d) const;


    // access methods
    int extension_capacity(void)    const {return m_extension_capacity;}
    int max_rob_capacity(void)      const {return m_max_rob_capacity;}
    int max_csm_capacity(void)      const {return m_max_csm_capacity;}
    int max_lvl1_emu_capacity(void) const {return m_max_lvl1_emu_capacity;}
    int max_rpc_hits_capacity(void) const {return m_max_rpc_hits_capacity;}
    int max_tgc_hits_capacity(void) const {return m_max_tgc_hits_capacity;}
    int max_mdt_hits_capacity(void) const {return m_max_mdt_hits_capacity;}
    
    AlgoId   id(void)    const {return m_id;}
    uint32_t te_id(void) const {return m_te_id;}
    uint32_t error(void) const {return m_error;}


    uint32_t lvl1_id(void)       const {return m_lvl1_id;}
    uint32_t lumi_block(void)    const {return m_lumi_block;}
    uint32_t muondetmask(void)   const {return m_muondetmask;} 
    uint32_t roi_id(void)        const {return m_RoIID;}
    uint32_t roi_system(void)    const {return m_RoISystem;}
    uint32_t roi_subsystem(void) const {return m_RoISubsystem;} 
    uint32_t roi_sector(void)    const {return m_RoISector;}
    uint32_t roi_number(void)    const {return m_RoINumber;}
    uint32_t roi_threshold(void) const {return m_RoIThreshold;}
    float    roi_eta(void)       const {return m_RoIEta;}
    float    roi_phi(void)       const {return m_RoIPhi;}
    
    uint32_t rpc_pad_error(void)     const {return m_rpc_pad_error;}
    uint32_t tgc_rdo_error(void)     const {return m_tgc_rdo_error;}
    
    float    rpc1_x(void) const {return m_rpc1_x;}
    float    rpc1_y(void) const {return m_rpc1_y;}
    float    rpc1_z(void) const {return m_rpc1_z;}
     
    float    rpc2_x(void) const {return m_rpc2_x;}
    float    rpc2_y(void) const {return m_rpc2_y;}
    float    rpc2_z(void) const {return m_rpc2_z;}
    
    float    rpc3_x(void) const {return m_rpc3_x;}
    float    rpc3_y(void) const {return m_rpc3_y;}
    float    rpc3_z(void) const {return m_rpc3_z;} 
    
    float    tgc_Mid1_eta(void)     const {return m_tgc_Mid1_eta;}
    float    tgc_Mid1_phi(void)     const {return m_tgc_Mid1_phi;}
    float    tgc_Mid1_r(void)       const {return m_tgc_Mid1_r;}
    float    tgc_Mid1_z(void)       const {return m_tgc_Mid1_z;}
    
    float    tgc_Mid2_eta(void)     const {return m_tgc_Mid2_eta;}
    float    tgc_Mid2_phi(void)     const {return m_tgc_Mid2_phi;}
    float    tgc_Mid2_r(void)       const {return m_tgc_Mid2_r;}
    float    tgc_Mid2_z(void)       const {return m_tgc_Mid2_z;}
    
    float    tgc_Mid_rho_chi2(void) const {return m_tgc_Mid_rho_chi2;}
    long     tgc_Mid_rho_N(void)    const {return m_tgc_Mid_rho_N;}
    float    tgc_Mid_phi_chi2(void) const {return m_tgc_Mid_phi_chi2;}
    long     tgc_Mid_phi_N(void)    const {return m_tgc_Mid_phi_N;}
    
    float    tgc_Inn_eta(void)      const {return m_tgc_Inn_eta;}
    float    tgc_Inn_phi(void)      const {return m_tgc_Inn_phi;}
    float    tgc_Inn_r(void)        const {return m_tgc_Inn_r;}
    float    tgc_Inn_z (void)       const {return m_tgc_Inn_z;}
    float    tgc_Inn_rho_std(void)  const {return m_tgc_Inn_rho_std;}
    long     tgc_Inn_rho_N(void)    const {return m_tgc_Inn_rho_N;}
    float    tgc_Inn_phi_std(void)  const {return m_tgc_Inn_phi_std;}
    long     tgc_Inn_phi_N(void)    const {return m_tgc_Inn_phi_N;}
    
    float    tgc_PT(void)           const {return m_tgc_PT;}
    
    long  type(void)         const {return m_type;}
    float t_x_center(void)   const {return m_t_x_center;}
    float t_y_center(void)   const {return m_t_y_center;}
    float t_radius(void)     const {return m_t_radius;}
    float t_ap(void)         const {return m_t_ap;}
    float t_aphi(void)       const {return m_t_aphi;}
    float t_phi(void)        const {return m_t_phi;}
    float extrInnerEta(void) const {return m_extrInnerEta;}
    float extrInnerPhi(void) const {return m_extrInnerPhi;}
    
    const std::vector< std::vector<int> >&   chamber_type_1(void) const {return m_chamber_type_1;}
    const std::vector< std::vector<int> >&   chamber_type_2(void) const {return m_chamber_type_2;}
    const std::vector< std::vector<int> >&   pos(void)            const {return m_pos;}
    const std::vector< std::vector<float> >& aw(void)             const {return m_aw;}
    const std::vector< std::vector<float> >& bw(void)             const {return m_bw;}
    const std::vector< std::vector<float> >& zetaMin(void)        const {return m_zetaMin;}
    const std::vector< std::vector<float> >& zetaMax(void)        const {return m_zetaMax;}
    const std::vector< std::vector<float> >& radMin(void)         const {return m_radMin;}
    const std::vector< std::vector<float> >& radMax(void)         const {return m_radMax;}
    const std::vector< std::vector<float> >& etaMin(void)         const {return m_etaMin;}
    const std::vector< std::vector<float> >& etaMax(void)         const {return m_etaMax;}
    const std::vector< std::vector<float> >& st_phi(void)         const {return m_st_phi;}
    const std::vector< std::vector<float> >& st_roads(void)       const {return m_st_roads;}
    
    
     
    float mdt_Inner_slope(void)      const {return m_mdt_Inner_slope;}
    float mdt_Inner_intercept(void)  const {return m_mdt_Inner_intercept;}
    float mdt_Inner_Z(void)          const {return m_mdt_Inner_Z;}
    float mdt_Inner_R(void)          const {return m_mdt_Inner_R;}
    float mdt_Inner_fit_chi(void)    const {return m_mdt_Inner_fit_chi;}
    float mdt_Middle_slope(void)     const {return m_mdt_Middle_slope;}
    float mdt_Middle_intercept(void) const {return m_mdt_Middle_intercept;}
    float mdt_Middle_Z(void)         const {return m_mdt_Middle_Z;}
    float mdt_Middle_R(void)         const {return m_mdt_Middle_R;}
    float mdt_Middle_fit_chi(void)   const {return m_mdt_Middle_fit_chi;}
    float mdt_Outer_slope(void)      const {return m_mdt_Outer_slope;}
    float mdt_Outer_intercept(void)  const {return m_mdt_Outer_intercept;}
    float mdt_Outer_Z(void)          const {return m_mdt_Outer_Z;}
    float mdt_Outer_R(void)          const {return m_mdt_Outer_R;}
    float mdt_Outer_fit_chi(void)    const {return m_mdt_Outer_fit_chi;}
    
    int   Address(void)      const {return m_Address;}  
    float Sagitta(void)      const {return m_Sagitta;}
    float Radius(void)       const {return m_Radius;}
    float Slope(void)        const {return m_Slope;}
    float Intercept(void)    const {return m_Intercept;}
    float Alpha(void)        const {return m_Alpha;}
    float Beta(void)         const {return m_Beta;}
    float DeltaR(void)       const {return m_DeltaR;}
    float Speed_over_c(void) const {return m_Speed_over_c;}
    float Eta(void)          const {return m_Eta;}
    float PhiMap(void)       const {return m_PhiMap;}
    float Phi(void)          const {return m_Phi;}
    float PhiDir(void)       const {return m_PhiDir;}
    float Pt(void)           const {return m_Pt;}
    float Charge(void)       const {return m_Charge;}
      
    const std::vector<uint32_t>& rob_id(void)         const {return m_rob_id;}
    const std::vector<uint32_t>& csm_id(void)         const {return m_csm_id;}
    const std::vector<uint32_t>& csm_size(void)       const {return m_csm_size;}
    const std::vector<uint32_t>& csm_error(void)      const {return m_csm_error;}
    const std::vector<uint32_t>& removed_rob_id(void) const {return m_removed_rob_id;}
    const std::vector<uint32_t>& removed_csm_id(void) const {return m_removed_csm_id;}
    
      
    uint32_t eta_pivot_lay0(void)  const;
    uint32_t eta_pivot_lay1(void)  const;
    uint32_t eta_low_0_lay0(void)  const;
    uint32_t eta_low_1_lay0(void)  const;
    uint32_t eta_low_0_lay1(void)  const;
    uint32_t eta_low_1_lay1(void)  const;
    uint32_t eta_high_0_lay0(void) const;
    uint32_t eta_high_1_lay0(void) const;
    uint32_t eta_high_0_lay1(void) const;
    uint32_t eta_high_1_lay1(void) const;
    uint32_t phi_pivot_lay0(void)  const;
    uint32_t phi_pivot_lay1(void)  const;
    uint32_t phi_low_0_lay0(void)  const;
    uint32_t phi_low_1_lay0(void)  const;
    uint32_t phi_low_0_lay1(void)  const;
    uint32_t phi_low_1_lay1(void)  const;
    uint32_t phi_high_0_lay0(void) const;
    uint32_t phi_high_1_lay0(void) const;
    uint32_t phi_high_0_lay1(void) const;
    uint32_t phi_high_1_lay1(void) const;

    
    const std::vector<uint32_t>& lvl1_emulation(void)   const {return m_lvl1_emulation;}
    
    const std::vector<uint32_t>& pad_hit_onlineId(void) const {return m_pad_hit_onlineId;}
    const std::vector<uint32_t>& pad_hit_code(void)     const {return m_pad_hit_code;}
    const std::vector<float>&    pad_hit_x(void)        const {return m_pad_hit_x;}
    const std::vector<float>&    pad_hit_y(void)        const {return m_pad_hit_y;}
    const std::vector<float>&    pad_hit_z(void)        const {return m_pad_hit_z;}
    const std::vector<float>&    pad_hit_r(void)        const {return m_pad_hit_r;}
    const std::vector<float>&    pad_hit_p(void)        const {return m_pad_hit_p;}
    
    const std::vector<float>&    tgc_Inn_rho_hit_phi(void)    const {return m_tgc_Inn_rho_hit_phi;}
    const std::vector<float>&    tgc_Inn_rho_hit_r(void)      const {return m_tgc_Inn_rho_hit_r;}
    const std::vector<float>&    tgc_Inn_rho_hit_z(void)      const {return m_tgc_Inn_rho_hit_z;}
    const std::vector<float>&    tgc_Inn_rho_hit_width(void)  const {return m_tgc_Inn_rho_hit_width;}
    const std::vector<uint32_t>& tgc_Inn_rho_hit_in_seg(void) const {return m_tgc_Inn_rho_hit_in_seg;}
    const std::vector<float>&    tgc_Inn_phi_hit_phi(void)    const {return m_tgc_Inn_phi_hit_phi;}
    const std::vector<float>&    tgc_Inn_phi_hit_r(void)      const {return m_tgc_Inn_phi_hit_r;}
    const std::vector<float>&    tgc_Inn_phi_hit_z(void)      const {return m_tgc_Inn_phi_hit_z;}
    const std::vector<float>&    tgc_Inn_phi_hit_width(void)  const {return m_tgc_Inn_phi_hit_width;}
    const std::vector<uint32_t>& tgc_Inn_phi_hit_in_seg(void) const {return m_tgc_Inn_phi_hit_in_seg;}
    const std::vector<float>&    tgc_Mid_rho_hit_phi(void)    const {return m_tgc_Mid_rho_hit_phi;}
    const std::vector<float>&    tgc_Mid_rho_hit_r(void)      const {return m_tgc_Mid_rho_hit_r;}
    const std::vector<float>&    tgc_Mid_rho_hit_z(void)      const {return m_tgc_Mid_rho_hit_z;}
    const std::vector<float>&    tgc_Mid_rho_hit_width(void)  const {return m_tgc_Mid_rho_hit_width;}
    const std::vector<uint32_t>& tgc_Mid_rho_hit_in_seg(void) const {return m_tgc_Mid_rho_hit_in_seg;}
    const std::vector<float>&    tgc_Mid_phi_hit_phi(void)    const {return m_tgc_Mid_phi_hit_phi;}
    const std::vector<float>&    tgc_Mid_phi_hit_r(void)      const {return m_tgc_Mid_phi_hit_r;}
    const std::vector<float>&    tgc_Mid_phi_hit_z(void)      const {return m_tgc_Mid_phi_hit_z;}
    const std::vector<float>&    tgc_Mid_phi_hit_width(void)  const {return m_tgc_Mid_phi_hit_width;}
    const std::vector<uint32_t>& tgc_Mid_phi_hit_in_seg(void) const {return m_tgc_Mid_phi_hit_in_seg;}  
    
    const std::vector<uint32_t>& mdt_onlineId(void)      const {return m_mdt_onlineId;}
    const std::vector<uint32_t>& mdt_offlineId(void)     const {return m_mdt_offlineId;}
    const std::vector<float>&    mdt_tube_r(void)        const {return m_mdt_tube_r;}
    const std::vector<float>&    mdt_tube_z(void)        const {return m_mdt_tube_z;}
    const std::vector<float>&    mdt_tube_residual(void) const {return m_mdt_tube_residual;}
    const std::vector<float>&    mdt_tube_time(void)     const {return m_mdt_tube_time;}
    const std::vector<float>&    mdt_tube_space(void)    const {return m_mdt_tube_space;}
    const std::vector<float>&    mdt_tube_sigma(void)    const {return m_mdt_tube_sigma;}  


    const std::vector<uint32_t>& extension0(void) const {return m_extension0;}
    const std::vector<uint32_t>& extension1(void) const {return m_extension1;}
    const std::vector<uint32_t>& extension2(void) const {return m_extension2;}
    const std::vector<uint32_t>& extension3(void) const {return m_extension3;}
    const std::vector<uint32_t>& extension4(void) const {return m_extension4;}
    const std::vector<uint32_t>& extension5(void) const {return m_extension5;}
    const std::vector<uint32_t>& extension6(void) const {return m_extension6;}
    const std::vector<uint32_t>& extension7(void) const {return m_extension7;}
    const std::vector<uint32_t>& extension8(void) const {return m_extension8;}
    const std::vector<uint32_t>& extension9(void) const {return m_extension9;}
    
 
 
    // fill methods
    void setError(uint32_t);
    void setRpcPadError(uint32_t);
    void setTgcPadError(uint32_t);
    
    void setRpc1(float x, float y, float z);
    void setRpc2(float x, float y, float z);
    void setRpc3(float x, float y, float z);
    
    void setEtaPivot(uint32_t ly0, uint32_t ly1);
    void setEtaLow (uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11);
    void setEtaHigh(uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11);
    void setPhiPivot(uint32_t ly0, uint32_t ly1);
    void setPhiLow (uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11);
    void setPhiHigh(uint32_t ly00, uint32_t ly01, uint32_t ly10, uint32_t ly11);
    void setLvl1Emulation (const std::vector<uint32_t>& v);
    
    void setTgcMid1(float Mid1_eta, float Mid1_phi, float Mid1_r,   float Mid1_z);
    void setTgcMid2(float Mid2_eta, float Mid2_phi, float Mid2_r,   float Mid2_z);
    void setTgcMidF(float rho_chi2, long rho_n,     float phi_chi2, long phi_n);
    void setTgcInn (float Inn_eta,  float Inn_phi,  float Inn_r,    float Inn_z);
    void setTgcInnF(float rho_std,  long rho_n,     float phi_std,  long phi_n);
    
    void setTgcPt(float tgc_pt);
    
    void setType(int t);
    void setT_x_center(float x);
    void setT_y_center(float y);
    void setT_radius(float r);
    void setT_ap(float ap);
    void setT_aphi(float aphi);
    void setT_phi(float phi);
    void setExtrInnerEta(float extEta);
    void setExtrInnerPhi(float extPhi);
    
    void setChamber_type_1(int tr, int st, int ty);
    void setChamber_type_2(int tr, int st, int ty);
    void setPos(int tr, int st, int pos);
    void setAw(int tr, int st, float aw);
    void setBw(int tr, int st, float bw);
    void setZetaMin(int tr, int st, float min);
    void setZetaMax(int tr, int st, float max);
    void setRadMin (int tr, int st, float min);
    void setRadMax (int tr, int st, float max);
    void setEtaMin (int tr, int st, float min);
    void setEtaMax (int tr, int st, float max);
    void setSt_phi (int tr, int st, float phi);
    void setSt_roads(int tr, int st, float road);

    void setChamber_type_1(const std::vector< std::vector<int> >& v);
    void setChamber_type_2(const std::vector< std::vector<int> >& v);
    void setPos(const std::vector< std::vector<int> >& v);
    void setAw(const std::vector< std::vector<float> >& v);
    void setBw(const std::vector< std::vector<float> >& v);
    void setZetaMin(const std::vector< std::vector<float> >& v);
    void setZetaMax(const std::vector< std::vector<float> >& v);
    void setRadMin (const std::vector< std::vector<float> >& v);
    void setRadMax (const std::vector< std::vector<float> >& v);
    void setEtaMin (const std::vector< std::vector<float> >& v);
    void setEtaMax (const std::vector< std::vector<float> >& v);
    void setSt_phi (const std::vector< std::vector<float> >& v);
    void setSt_roads(const std::vector< std::vector<float> >& v);

    void setPadHit(uint32_t onlineId, uint32_t code, float x, float y, float z,
                   float r, float p);
    
    void setTgcMidRhoHit(float phi,float r,float z,float width,uint32_t in_seg);
    void setTgcMidPhiHit(float phi,float r,float z,float width,uint32_t in_seg);
    void setTgcInnRhoHit(float phi,float r,float z,float width,uint32_t in_seg);
    void setTgcInnPhiHit(float phi,float r,float z,float width,uint32_t in_seg);
    
    void setMdtHit(uint32_t onlineId, uint32_t offlineId, float r, float z,
                   float residual, float time, float space, float sigma);

    void setSP1(float slope,float intercept,float z,float r,float chisquare);
    void setSP2(float slope,float intercept,float z,float r,float chisquare);
    void setSP3(float slope,float intercept,float z,float r,float chisquare);
    
    void setAddress(int address);
    void setSagitta(float sagitta);
    void setRadius(float radius);
    void setSlope(float slope);
    void setIntercept(float intercept);
    void setAlpha(float alpha);
    void setBeta(float beta);
    void setDeltaR(float deltaR);
    void setSpeed_over_c(float speed);
    void setEta(float eta);
    void setPhiMap(float phimap);
    void setPhi(float phi);
    void setPhiDir(float dir_phi);
    void setPt(float pt);
    void setCharge(float charge);
    
    void setExtension0(uint32_t);
    void setExtension1(uint32_t);
    void setExtension2(uint32_t);
    void setExtension3(uint32_t);
    void setExtension4(uint32_t);
    void setExtension5(uint32_t);
    void setExtension6(uint32_t);
    void setExtension7(uint32_t);
    void setExtension8(uint32_t);
    void setExtension9(uint32_t);
    
    void setExtension0(const std::vector<uint32_t>& v);
    void setExtension1(const std::vector<uint32_t>& v);
    void setExtension2(const std::vector<uint32_t>& v);
    void setExtension3(const std::vector<uint32_t>& v);
    void setExtension4(const std::vector<uint32_t>& v);
    void setExtension5(const std::vector<uint32_t>& v);
    void setExtension6(const std::vector<uint32_t>& v);
    void setExtension7(const std::vector<uint32_t>& v);
    void setExtension8(const std::vector<uint32_t>& v);
    void setExtension9(const std::vector<uint32_t>& v);
    
    void setRequestedRob(uint32_t);
    void setConvertedCsm(uint32_t,uint32_t);
    void setCsmError(uint32_t);
    void setRemovedRob(uint32_t);
    void setRemovedCsm(uint32_t);

    void setRequestedRob(const std::vector<uint32_t>& v);
    void setConvertedCsm(const std::vector<uint32_t>& v);
    void setConvertedCsmSize(const std::vector<uint32_t>& v);
    void setCsmError(const std::vector<uint32_t>& v);
    void setRemovedRob(const std::vector<uint32_t>& v);
    void setRemovedCsm(const std::vector<uint32_t>& v);
    
    
private:

    int m_extension_capacity;
    int m_max_rob_capacity;
    int m_max_csm_capacity;
    int m_max_lvl1_emu_capacity;
    int m_max_rpc_hits_capacity;
    int m_max_tgc_hits_capacity;
    int m_max_mdt_hits_capacity;


    AlgoId   m_id;
    uint32_t m_te_id;
    uint32_t m_error;


    uint32_t m_lvl1_id;
    uint32_t m_lumi_block;
    uint32_t m_muondetmask; 
    uint32_t m_RoIID;
    uint32_t m_RoISystem;
    uint32_t m_RoISubsystem; 
    uint32_t m_RoISector;
    uint32_t m_RoINumber;
    uint32_t m_RoIThreshold;
    float    m_RoIEta;
    float    m_RoIPhi;  
    
    uint32_t m_rpc_pad_error;
    uint32_t m_tgc_rdo_error;
    
    float    m_rpc1_x;
    float    m_rpc1_y;
    float    m_rpc1_z;
     
    float    m_rpc2_x;
    float    m_rpc2_y;
    float    m_rpc2_z;
    
    float    m_rpc3_x;
    float    m_rpc3_y;
    float    m_rpc3_z; 
    
    float    m_tgc_Mid1_eta;
    float    m_tgc_Mid1_phi;
    float    m_tgc_Mid1_r;
    float    m_tgc_Mid1_z;
    
    float    m_tgc_Mid2_eta;
    float    m_tgc_Mid2_phi;
    float    m_tgc_Mid2_r;
    float    m_tgc_Mid2_z;
    
    float    m_tgc_Mid_rho_chi2;
    long     m_tgc_Mid_rho_N;
    float    m_tgc_Mid_phi_chi2;
    long     m_tgc_Mid_phi_N;
    
    float    m_tgc_Inn_eta;
    float    m_tgc_Inn_phi;
    float    m_tgc_Inn_r;
    float    m_tgc_Inn_z;
    float    m_tgc_Inn_rho_std;
    long     m_tgc_Inn_rho_N;
    float    m_tgc_Inn_phi_std;
    long     m_tgc_Inn_phi_N;
    
    float    m_tgc_PT;
    
    long  m_type;
    float m_t_x_center;
    float m_t_y_center;
    float m_t_radius;
    float m_t_ap;
    float m_t_aphi;
    float m_t_phi;
    float m_extrInnerEta;
    float m_extrInnerPhi;
    
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
    
    float m_mdt_Inner_slope;
    float m_mdt_Inner_intercept;
    float m_mdt_Inner_Z;
    float m_mdt_Inner_R;
    float m_mdt_Inner_fit_chi;
    float m_mdt_Middle_slope;
    float m_mdt_Middle_intercept;
    float m_mdt_Middle_Z;
    float m_mdt_Middle_R;
    float m_mdt_Middle_fit_chi;
    float m_mdt_Outer_slope;
    float m_mdt_Outer_intercept;
    float m_mdt_Outer_Z;
    float m_mdt_Outer_R;
    float m_mdt_Outer_fit_chi;
    
    int   m_Address;  
    float m_Sagitta;
    float m_Radius;
    float m_Slope;
    float m_Intercept;
    float m_Alpha;
    float m_Beta;
    float m_DeltaR;
    float m_Speed_over_c;
    float m_Eta;
    float m_PhiMap;
    float m_Phi;
    float m_PhiDir;
    float m_Pt;
    float m_Charge;
    
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
    
    std::vector<uint32_t> m_pad_hit_onlineId;
    std::vector<uint32_t> m_pad_hit_code;
    std::vector<float>    m_pad_hit_x;
    std::vector<float>    m_pad_hit_y;
    std::vector<float>    m_pad_hit_z;
    std::vector<float>    m_pad_hit_r;
    std::vector<float>    m_pad_hit_p;
    
    std::vector<float>    m_tgc_Inn_rho_hit_phi;
    std::vector<float>    m_tgc_Inn_rho_hit_r;
    std::vector<float>    m_tgc_Inn_rho_hit_z;
    std::vector<float>    m_tgc_Inn_rho_hit_width;
    std::vector<uint32_t> m_tgc_Inn_rho_hit_in_seg;
    std::vector<float>    m_tgc_Inn_phi_hit_phi;
    std::vector<float>    m_tgc_Inn_phi_hit_r;
    std::vector<float>    m_tgc_Inn_phi_hit_z;
    std::vector<float>    m_tgc_Inn_phi_hit_width;
    std::vector<uint32_t> m_tgc_Inn_phi_hit_in_seg;
    std::vector<float>    m_tgc_Mid_rho_hit_phi;
    std::vector<float>    m_tgc_Mid_rho_hit_r;
    std::vector<float>    m_tgc_Mid_rho_hit_z;
    std::vector<float>    m_tgc_Mid_rho_hit_width;
    std::vector<uint32_t> m_tgc_Mid_rho_hit_in_seg;
    std::vector<float>    m_tgc_Mid_phi_hit_phi;
    std::vector<float>    m_tgc_Mid_phi_hit_r;
    std::vector<float>    m_tgc_Mid_phi_hit_z;
    std::vector<float>    m_tgc_Mid_phi_hit_width;
    std::vector<uint32_t> m_tgc_Mid_phi_hit_in_seg;
    
    std::vector<uint32_t> m_mdt_onlineId;
    std::vector<uint32_t> m_mdt_offlineId;
    std::vector<float>    m_mdt_tube_r;
    std::vector<float>    m_mdt_tube_z;
    std::vector<float>    m_mdt_tube_residual;
    std::vector<float>    m_mdt_tube_time;
    std::vector<float>    m_mdt_tube_space;
    std::vector<float>    m_mdt_tube_sigma;
    
    void init_variables();
    void init_vectors();
    void copy_variables(MuonFeatureDetails& des, const MuonFeatureDetails& sor); 
    template <class T> void copy_vector(std::vector<T>& d,
                                       const std::vector<T>& s,  
                                       int resize=0);

    uint32_t lvl1_emulation(size_t i)  const;
};


inline uint32_t MuonFeatureDetails::lvl1_emulation(size_t i)  const {
  if (m_lvl1_emulation.size() > i)
    return m_lvl1_emulation[i];
  return 0;
}


inline uint32_t MuonFeatureDetails::eta_pivot_lay0(void)  const {
  return lvl1_emulation(0);
}

inline uint32_t MuonFeatureDetails::eta_pivot_lay1(void)  const {
  return lvl1_emulation(1);
}
  
inline uint32_t MuonFeatureDetails::eta_low_0_lay0(void)  const {
  return lvl1_emulation(2);
}

inline uint32_t MuonFeatureDetails::eta_low_1_lay0(void)  const {
  return lvl1_emulation(3);
}

inline uint32_t MuonFeatureDetails::eta_low_0_lay1(void)  const {
  return lvl1_emulation(4);
}

inline uint32_t MuonFeatureDetails::eta_low_1_lay1(void)  const {
  return lvl1_emulation(5);
}

inline uint32_t MuonFeatureDetails::eta_high_0_lay0(void) const {
  return lvl1_emulation(6);
}

inline uint32_t MuonFeatureDetails::eta_high_1_lay0(void) const {
  return lvl1_emulation(7);
}

inline uint32_t MuonFeatureDetails::eta_high_0_lay1(void) const {
  return lvl1_emulation(8);
}

inline uint32_t MuonFeatureDetails::eta_high_1_lay1(void) const {
  return lvl1_emulation(9);
}

inline uint32_t MuonFeatureDetails::phi_pivot_lay0(void)  const {
  return lvl1_emulation(10);
}

inline uint32_t MuonFeatureDetails::phi_pivot_lay1(void)  const {
  return lvl1_emulation(11);
}

inline uint32_t MuonFeatureDetails::phi_low_0_lay0(void)  const {
  return lvl1_emulation(12);
}

inline uint32_t MuonFeatureDetails::phi_low_1_lay0(void)  const {
  return lvl1_emulation(13);
}

inline uint32_t MuonFeatureDetails::phi_low_0_lay1(void)  const {
  return lvl1_emulation(14);
}

inline uint32_t MuonFeatureDetails::phi_low_1_lay1(void)  const {
  return lvl1_emulation(15);
}

inline uint32_t MuonFeatureDetails::phi_high_0_lay0(void) const {
  return lvl1_emulation(16);
}

inline uint32_t MuonFeatureDetails::phi_high_1_lay0(void) const {
  return lvl1_emulation(17);
}

inline uint32_t MuonFeatureDetails::phi_high_0_lay1(void) const {
  return lvl1_emulation(18);
}

inline uint32_t MuonFeatureDetails::phi_high_1_lay1(void) const {
  return lvl1_emulation(19);
}




// printouts operators

MsgStream& operator<< ( MsgStream& m, const MuonFeatureDetails& d );
std::ostream& operator<< (std::ostream& s, const MuonFeatureDetails& d );

CLASS_DEF(MuonFeatureDetails, 262323491, 1)
CLASS_DEF(DataVector<MuonFeatureDetails>, 3346390, 1)

#include "TrigMuonEvent/MuonFeatureDetailsContainer.h"

#endif
