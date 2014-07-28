/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TrigDiMuonExtrapolator_H
#define TrigDiMuonExtrapolator_H

#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/AlgTool.h"

#include "TrigDiMuon/Candidate.h"

static const InterfaceID IID_ITrigDiMuonExtrapolator("ITrigDiMuonExtrapolator", 1 , 0);

class TrigDiMuonExtrapolator : public AlgTool
{

  public:
    static const InterfaceID& interfaceID() {
      return IID_ITrigDiMuonExtrapolator;
    }

    TrigDiMuonExtrapolator( const std::string&, const std::string&, const IInterface* );
    virtual ~TrigDiMuonExtrapolator(){ };

    virtual StatusCode initialize();
    virtual StatusCode finalize();

    void propagationInnerBarrel(Candidate& t);
    void propagationInnerEndCap(Candidate& t);
    void propagationMiddleBarrel(Candidate& t);
    void propagationMiddleEndCap(Candidate& t);

    // hit matching methods
    bool matchMdtInner(double r, double z,
                       Candidate& t);            // check MDT inner hit matching

    bool matchMdtMiddle(double r, double z,
                        Candidate& t);           // check MDT inner hit matching
    bool matchRpcEtaPad(double eta_hit,
                        Candidate& t);           // check RPC middle eta hit matching
    bool matchRpcPhiPad(double phi_hit,
                        Candidate& t);           // check RPC middle phi hit matching
    bool matchInnerTgcPhiHit(double phi_hit,
                             Candidate& t);      // check TGC inner phi hit matching
    bool matchInnerTgcRhoHit(double eta_hit,
                             double phi_hit,
                             Candidate& t);      // check TGC inner rho hit matching
    bool matchMiddleTgcPhiHit(double phi_hit,
                              Candidate& t);      // check TGC middle phi hit matching
    bool matchMiddleTgcRhoHit(double eta_hit,
                              double phi_hit,
                              Candidate& t);     // check TGC middle rho hit matching


  private:
    BooleanProperty m_tilted_B_field;

    DoubleProperty m_RpcEtaFactor;
    DoubleProperty m_RpcPhiFactor;
    DoubleProperty m_TgcEtaFactor;
    DoubleProperty m_TgcPhiFactor;
    DoubleProperty m_MdtEtaFactor;

    //extrapolator constants
    double m_ib_c0;
    double m_ib_c1;

    double m_ie_etaBin1_c0;
    double m_ie_etaBin1_c1;

    double m_ie_etaBin2_c0;
    double m_ie_etaBin2_c1;

    double m_ie_etaBin3_c0;
    double m_ie_etaBin3_c1;

    double m_mb_c0;
    double m_mb_c1;
    double m_mb_c2;

    double m_me_etaBin1_phiBin1_c0;
    double m_me_etaBin1_phiBin1_c1;
    double m_me_etaBin1_phiBin1_c2;

    double m_me_etaBin1_phiBin2_c0;
    double m_me_etaBin1_phiBin2_c1;
    double m_me_etaBin1_phiBin2_c2;

    double m_me_etaBin1_phiBin3_c0;
    double m_me_etaBin1_phiBin3_c1;
    double m_me_etaBin1_phiBin3_c2;


    double m_me_etaBin2_phiBin1_c0;
    double m_me_etaBin2_phiBin1_c1;
    double m_me_etaBin2_phiBin1_c2;

    double m_me_etaBin2_phiBin2_c0;
    double m_me_etaBin2_phiBin2_c1;
    double m_me_etaBin2_phiBin2_c2;

    double m_me_etaBin2_phiBin3_c0;
    double m_me_etaBin2_phiBin3_c1;
    double m_me_etaBin2_phiBin3_c2;


    double m_me_etaBin3_phiBin1_c0;
    double m_me_etaBin3_phiBin1_c1;
    double m_me_etaBin3_phiBin1_c2;

    double m_me_etaBin3_phiBin2_c0;
    double m_me_etaBin3_phiBin2_c1;
    double m_me_etaBin3_phiBin2_c2;

    double m_me_etaBin3_phiBin3_c0;
    double m_me_etaBin3_phiBin3_c1;
    double m_me_etaBin3_phiBin3_c2;


    double m_me_etaBin4_phiBin1_c0;
    double m_me_etaBin4_phiBin1_c1;
    double m_me_etaBin4_phiBin1_c2;

    double m_me_etaBin4_phiBin2_c0;
    double m_me_etaBin4_phiBin2_c1;
    double m_me_etaBin4_phiBin2_c2;

    double m_me_etaBin4_phiBin3_c0;
    double m_me_etaBin4_phiBin3_c1;
    double m_me_etaBin4_phiBin3_c2;


    double m_me_etaBin5_phiBin1_c0;
    double m_me_etaBin5_phiBin1_c1;
    double m_me_etaBin5_phiBin1_c2;

    double m_me_etaBin5_phiBin2_c0;
    double m_me_etaBin5_phiBin2_c1;
    double m_me_etaBin5_phiBin2_c2;

    double m_me_etaBin5_phiBin3_c0;
    double m_me_etaBin5_phiBin3_c1;
    double m_me_etaBin5_phiBin3_c2;

    double m_me_c0;
    double m_me_c1;

    // cut-cone constants
    double m_cone_Endcap_mdt_inner_dEta_c0;
    double m_cone_Endcap_mdt_inner_dEta_c1;

    double m_cone_Endcap_mdt_middle_dEta_c0;
    double m_cone_Endcap_mdt_middle_dEta_c1;

    double m_cone_Barrel_mdt_inner_dEta_c0;
    double m_cone_Barrel_mdt_inner_dEta_c1;

    double m_cone_Barrel_mdt_middle_dEta_c0;
    double m_cone_Barrel_mdt_middle_dEta_c1;

    double m_cone_rpc_middle_dEta_c0;
    double m_cone_rpc_middle_dEta_c1;

    double m_cone_rpc_middle_dPhi_c0;
    double m_cone_rpc_middle_dPhi_c1;

    double m_cone_tgc_wire_inner_dEta_c0;
    double m_cone_tgc_wire_inner_dEta_c1;

    double m_cone_tgc_wire_middle_dEta_c0;
    double m_cone_tgc_wire_middle_dEta_c1;

    double m_cone_tgc_strip_inner_dPhi_c0;
    double m_cone_tgc_strip_inner_dPhi_c1;

    double m_cone_tgc_strip_middle_dPhi_c0;
    double m_cone_tgc_strip_middle_dPhi_c1;
    double m_cone_tgc_strip_middle2_dPhi_c0;
    double m_cone_tgc_strip_middle2_dPhi_c1;

};

// some useful functions
double dAngle(double phi1, double phi2);
double getEta(double r, double z);

#endif

