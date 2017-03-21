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
  
  void fillResponse(const xAOD::Jet& jet, const xAOD::Jet& truth);
  void fillPseudoResponse(const xAOD::Jet& jet, const xAOD::Jet& calo);
  
  void fillMoments(const xAOD::Jet& jet);
  void fillMoments(const xAOD::Jet& jet, const xAOD::Jet& calo);
  
  void fill2Leadings(std::vector<const xAOD::Jet*>& jets, std::vector<const xAOD::Jet*>& truths);
  
  void fillMoments2Leadings(std::vector<const xAOD::Jet*>& jets, std::vector<const xAOD::Jet*>& calo);
  void fillPseudoResponse2Leadings(std::vector<const xAOD::Jet*>& jets, std::vector<const xAOD::Jet*>& calo);

private:
  double m_minMass;
  double m_maxMass;
  
  TH1* m_jet_n                             ;
  TH1* m_jet_pt                            ;
  TH1* m_jet_eta                           ;
  TH1* m_jet_phi                           ;
  TH1* m_jet_m                             ;
  
  TH1* m_jet_m_2leadings                   ;
  TH1* m_jet_m_leading                     ;
  TH1* m_jet_m_subleading                  ;
  
  TH1* m_jet_pt_2leadings                  ;
  TH1* m_jet_pt_leading                    ;
  TH1* m_jet_pt_subleading                 ;
  
  TH1* m_jet_deltaR                        ;
  TH1* m_jet_deltaR_matched                ;
  
  TH1* m_jet_response_m                    ;
  TH1* m_jet_response_pt                   ;
  
  TH1* m_jet_response_m_2leadings          ;
  TH1* m_jet_response_pt_2leadings         ;
  
  TH1* m_jet_response_m_leading            ;
  TH1* m_jet_response_pt_leading           ;
  
  TH1* m_jet_response_m_subleading         ;
  TH1* m_jet_response_pt_subleading        ;
  
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
  
   
  // plot base has nop default implementation of this; we use it to book the histos
  void initializePlots();
  void finalizePlots();
};




#endif // TRACKCALOCLUSTERREC_TRACKCALOCLUSTERRECVALIDATION_TCCPLOTS_H