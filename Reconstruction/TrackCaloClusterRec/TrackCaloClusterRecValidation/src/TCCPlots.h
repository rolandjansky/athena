/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTS_H
#define TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTS_H

// std includes
#include <string>
#include <array>

// local includes
#include "TCCPlotsBase.h"
#include "xAODJet/JetContainer.h"

class TCCPlots: public TCCPlotsBase {
public:
  TCCPlots(TCCPlotsBase* pParent, const std::string& dirName);
  void fill(const xAOD::JetContainer& jets);
  void fill(const xAOD::Jet& jet);
  void fill(const xAOD::Jet& jet, const xAOD::Jet& truth);
  
  void fillLeading(const xAOD::Jet& jet);
  void fillSubLeading(const xAOD::Jet& jet);
  
  void fillResponse(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseLeading(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseSubLeading(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseNoPtNoMassCuts(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseNoPtNoMassCutsLeading(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillResponseNoPtNoMassCutsSubLeading(const xAOD::Jet& jet, const xAOD::Jet& truth);
  
  void fillPseudoResponse(const xAOD::Jet& jet, const xAOD::Jet& calo);
  void fillPseudoResponseLeading(const xAOD::Jet& jet, const xAOD::Jet& calo);
  void fillPseudoResponseSubLeading(const xAOD::Jet& jet, const xAOD::Jet& calo);
    
  void fillMoments(const xAOD::Jet& jet);
  void fillMomentsLeading(const xAOD::Jet& jet);
  void fillMomentsSubLeading(const xAOD::Jet& jet);
  void fillMomentsWithMassCut(const xAOD::Jet& jet);
  void fillMomentsLeadingWithMassCut(const xAOD::Jet& jet);
  void fillMomentsSubLeadingWithMassCut(const xAOD::Jet& jet);
  
  void make_median(TH2* h2_response, TH1* h1_resolution);
  void make_median(TH3* h3_response, TH2* h2_resolution);

private:  
  TH1* m_jet_n                             ;
  TH1* m_jet_pt                            ;
  TH1* m_jet_eta                           ;
  TH1* m_jet_phi                           ;
  TH1* m_jet_m                             ;
  
  TH1* m_jet_m_2leadings                   ;
  TH1* m_jet_pt_2leadings                  ;
  TH1* m_jet_eta_2leadings                 ;
  TH1* m_jet_phi_2leadings                 ;
  
  TH1* m_jet_m_leading                     ;
  TH1* m_jet_pt_leading                    ;
  TH1* m_jet_eta_leading                   ;
  TH1* m_jet_phi_leading                   ;
  
  TH1* m_jet_m_subleading                  ;
  TH1* m_jet_pt_subleading                 ;
  TH1* m_jet_eta_subleading                ;
  TH1* m_jet_phi_subleading                ;
  
  TH1* m_jet_deltaR                        ;
  TH1* m_jet_deltaR_matched                ;
  
  TH1* m_jet_response_m                    ;
  TH2* m_jet_response_m_pt                 ;
  TH1* m_jet_response_pt                   ;
  TH1* m_jet_response_eta                  ;
  TH1* m_jet_response_phi                  ;
  
  TH1* m_jet_response_m_2leadings          ;
  TH2* m_jet_response_m_pt_2leadings       ;
  TH1* m_jet_response_pt_2leadings         ;
  TH1* m_jet_response_eta_2leadings        ;
  TH1* m_jet_response_phi_2leadings        ;
  
  TH1* m_jet_response_m_leading            ;
  TH2* m_jet_response_m_pt_leading         ;
  TH1* m_jet_response_pt_leading           ;
  TH1* m_jet_response_eta_leading          ;
  TH1* m_jet_response_phi_leading          ;
  
  TH1* m_jet_response_m_subleading         ;
  TH2* m_jet_response_m_pt_subleading      ;
  TH1* m_jet_response_pt_subleading        ;
  TH1* m_jet_response_eta_subleading       ;
  TH1* m_jet_response_phi_subleading       ;
  
  TH1* m_jet_pseudoresponse_m              ;
  TH1* m_jet_pseudoresponse_pt             ;
  
  TH1* m_jet_pseudoresponse_m_2leadings    ;
  TH1* m_jet_pseudoresponse_pt_2leadings   ;
  
  TH1* m_jet_pseudoresponse_m_leading      ;
  TH1* m_jet_pseudoresponse_pt_leading     ;
  
  TH1* m_jet_pseudoresponse_m_subleading   ;
  TH1* m_jet_pseudoresponse_pt_subleading  ;
  
  TH1* m_jet_d2                            ;
  TH1* m_jet_d2_2leadings                  ;
  TH1* m_jet_d2_leading                    ;
  TH1* m_jet_d2_subleading                 ;
  
  TH1* m_jet_d2_massCut                    ;
  TH1* m_jet_d2_2leadings_massCut          ;
  TH1* m_jet_d2_leading_massCut            ;
  TH1* m_jet_d2_subleading_massCut         ;
  
  TH1* m_jet_response_d2                   ;
  TH1* m_jet_response_d2_2leadings         ;
  TH1* m_jet_response_d2_leading           ;
  TH1* m_jet_response_d2_subleading        ;  
   
  TH1* m_jet_resolution_m                  ;
  TH1* m_jet_resolution_m_2leadings        ;
  TH1* m_jet_resolution_m_leading          ;
  TH1* m_jet_resolution_m_subleading       ;  
  
  TH3* m_jet_mopt_pt_response_m                  ;
  TH3* m_jet_mopt_pt_response_m_2leadings        ;
  TH3* m_jet_mopt_pt_response_m_leading          ;
  TH3* m_jet_mopt_pt_response_m_subleading       ;
  TH3* m_jet_mopt_pt_response_pt                 ;
  TH3* m_jet_mopt_pt_response_pt_2leadings       ;
  TH3* m_jet_mopt_pt_response_pt_leading         ;
  TH3* m_jet_mopt_pt_response_pt_subleading      ;
  TH3* m_jet_mopt_pt_response_phi                ;
  TH3* m_jet_mopt_pt_response_phi_2leadings      ;
  TH3* m_jet_mopt_pt_response_phi_leading        ;
  TH3* m_jet_mopt_pt_response_phi_subleading     ;
  TH3* m_jet_mopt_pt_response_eta                ;
  TH3* m_jet_mopt_pt_response_eta_2leadings      ;
  TH3* m_jet_mopt_pt_response_eta_leading        ;
  TH3* m_jet_mopt_pt_response_eta_subleading     ;
  TH3* m_jet_mopt_pt_response_d2                 ;
  TH3* m_jet_mopt_pt_response_d2_2leadings       ;
  TH3* m_jet_mopt_pt_response_d2_leading         ;
  TH3* m_jet_mopt_pt_response_d2_subleading      ;
  
  TH2* m_jet_resolution_mopt_pt_m                ;
  TH2* m_jet_resolution_mopt_pt_m_2leadings      ;
  TH2* m_jet_resolution_mopt_pt_m_leading        ;
  TH2* m_jet_resolution_mopt_pt_m_subleading     ;
  TH2* m_jet_resolution_mopt_pt_pt               ;
  TH2* m_jet_resolution_mopt_pt_pt_2leadings     ;
  TH2* m_jet_resolution_mopt_pt_pt_leading       ;
  TH2* m_jet_resolution_mopt_pt_pt_subleading    ;
  TH2* m_jet_resolution_mopt_pt_phi              ;
  TH2* m_jet_resolution_mopt_pt_phi_2leadings    ;
  TH2* m_jet_resolution_mopt_pt_phi_leading      ;
  TH2* m_jet_resolution_mopt_pt_phi_subleading   ;
  TH2* m_jet_resolution_mopt_pt_eta              ;
  TH2* m_jet_resolution_mopt_pt_eta_2leadings    ;
  TH2* m_jet_resolution_mopt_pt_eta_leading      ;
  TH2* m_jet_resolution_mopt_pt_eta_subleading   ;
  TH2* m_jet_resolution_mopt_pt_d2               ;
  TH2* m_jet_resolution_mopt_pt_d2_2leadings     ;
  TH2* m_jet_resolution_mopt_pt_d2_leading       ;
  TH2* m_jet_resolution_mopt_pt_d2_subleading    ;
  
  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
};




#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTS_H