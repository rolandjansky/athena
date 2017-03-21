/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TCCPlots.h"
#include <cmath>

using CLHEP::GeV;

TCCPlots::TCCPlots(TCCPlotsBase* pParent, const std::string& sDir) :
  TCCPlotsBase(pParent, sDir),
  m_minMass(50*GeV),
  m_maxMass(150*GeV),
  m_jet_n                           (nullptr),
  m_jet_pt                          (nullptr),
  m_jet_eta                         (nullptr),
  m_jet_phi                         (nullptr),
  m_jet_m                           (nullptr),
  m_jet_m_2leadings                 (nullptr),
  m_jet_m_leading                   (nullptr),
  m_jet_m_subleading                (nullptr),
  m_jet_pt_2leadings                (nullptr),
  m_jet_pt_leading                  (nullptr),
  m_jet_pt_subleading               (nullptr),
  m_jet_deltaR                      (nullptr),
  m_jet_deltaR_matched              (nullptr),
  m_jet_response_m                  (nullptr),
  m_jet_response_pt                 (nullptr),
  m_jet_response_m_2leadings        (nullptr),
  m_jet_response_pt_2leadings       (nullptr),
  m_jet_response_m_leading          (nullptr),
  m_jet_response_pt_leading         (nullptr),
  m_jet_response_m_subleading       (nullptr),
  m_jet_response_pt_subleading      (nullptr),
  m_jet_pseudoresponse_m            (nullptr),
  m_jet_pseudoresponse_pt           (nullptr),
  m_jet_pseudoresponse_m_2leadings  (nullptr),
  m_jet_pseudoresponse_pt_2leadings (nullptr),
  m_jet_pseudoresponse_m_leading    (nullptr),
  m_jet_pseudoresponse_pt_leading   (nullptr),
  m_jet_pseudoresponse_m_subleading (nullptr),
  m_jet_pseudoresponse_pt_subleading(nullptr),
  m_jet_d2                          (nullptr),
  m_jet_d2_2leadings                (nullptr),
  m_jet_d2_leading                  (nullptr),
  m_jet_d2_subleading               (nullptr),
  m_jet_d2_massCut                  (nullptr),
  m_jet_d2_2leadings_massCut        (nullptr),
  m_jet_d2_leading_massCut          (nullptr),
  m_jet_d2_subleading_massCut       (nullptr){
}

void TCCPlots::initializePlots() {
  book(m_jet_n                             , "jet_n"                            );
  book(m_jet_pt                            , "jet_pt"                           );
  book(m_jet_eta                           , "jet_eta"                          );
  book(m_jet_phi                           , "jet_phi"                          );
  book(m_jet_m                             , "jet_m"                            );
  book(m_jet_m_2leadings                   , "jet_m_2leadings"                  );
  book(m_jet_m_leading                     , "jet_m_leading"                    );
  book(m_jet_m_subleading                  , "jet_m_subleading"                 );
  book(m_jet_pt_2leadings                  , "jet_pt_2leadings"                 );
  book(m_jet_pt_leading                    , "jet_pt_leading"                   );
  book(m_jet_pt_subleading                 , "jet_pt_subleading"                );
  book(m_jet_deltaR                        , "jet_deltaR"                       );
  book(m_jet_deltaR_matched                , "jet_deltaR_matched"               );
  book(m_jet_response_m                    , "jet_response_m"                   );
  book(m_jet_response_pt                   , "jet_response_pt"                  );
  book(m_jet_response_m_2leadings          , "jet_response_m_2leadings"         );
  book(m_jet_response_pt_2leadings         , "jet_response_pt_2leadings"        );
  book(m_jet_response_m_leading            , "jet_response_m_leading"           );
  book(m_jet_response_pt_leading           , "jet_response_pt_leading"          );
  book(m_jet_response_m_subleading         , "jet_response_m_subleading"        );
  book(m_jet_response_pt_subleading        , "jet_response_pt_subleading"       );
  book(m_jet_pseudoresponse_m              , "jet_pseudoresponse_m"             );
  book(m_jet_pseudoresponse_pt             , "jet_pseudoresponse_pt"            );
  book(m_jet_pseudoresponse_m_2leadings    , "jet_pseudoresponse_m_2leadings"   );
  book(m_jet_pseudoresponse_pt_2leadings   , "jet_pseudoresponse_pt_2leadings"  );
  book(m_jet_pseudoresponse_m_leading      , "jet_pseudoresponse_m_leading"     );
  book(m_jet_pseudoresponse_pt_leading     , "jet_pseudoresponse_pt_leading"    );
  book(m_jet_pseudoresponse_m_subleading   , "jet_pseudoresponse_m_subleading"  );
  book(m_jet_pseudoresponse_pt_subleading  , "jet_pseudoresponse_pt_subleading" );
  book(m_jet_d2                            , "jet_d2"                           );
  book(m_jet_d2_2leadings                  , "jet_d2_2leadings"                 );
  book(m_jet_d2_leading                    , "jet_d2_leading"                   );
  book(m_jet_d2_subleading                 , "jet_d2_subleading"                );
  book(m_jet_d2_massCut                    , "jet_d2_massCut"                   );
  book(m_jet_d2_2leadings_massCut          , "jet_d2_2leadings_massCut"         );
  book(m_jet_d2_leading_massCut            , "jet_d2_leading_massCut"           );
  book(m_jet_d2_subleading_massCut         , "jet_d2_subleading_massCut"        );  
}

void TCCPlots::fill(const xAOD::JetContainer& jets) {
  fillHisto(m_jet_n , jets.size());  
}

void TCCPlots::fill(const xAOD::Jet& jet) {
  fillHisto(m_jet_pt , jet.pt()/GeV);
  fillHisto(m_jet_eta, jet.eta());
  fillHisto(m_jet_phi, jet.phi());
  fillHisto(m_jet_m  , jet.m()/GeV);
}

void TCCPlots::fill(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_deltaR , jet.p4().DeltaR(truth.p4()));
}
// 
void TCCPlots::fillResponse(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_deltaR_matched , jet.p4().DeltaR(truth.p4()));
  fillHisto(m_jet_response_m     , jet.m() /truth.m() );
  fillHisto(m_jet_response_pt    , jet.pt()/truth.pt());
}

void TCCPlots::fillPseudoResponse(const xAOD::Jet& jet, const xAOD::Jet& calo) {
  fillHisto(m_jet_pseudoresponse_m     , calo.m() /jet.m() );
  fillHisto(m_jet_pseudoresponse_pt    , calo.pt()/jet.pt());
}

void TCCPlots::fillMoments(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet))
    fillHisto(m_jet_d2, accD2(jet));
}

void TCCPlots::fillMoments(const xAOD::Jet& jet, const xAOD::Jet& calo) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet)) {
    float mass = calo.m();
    if (mass > m_minMass and mass < m_maxMass)
      fillHisto(m_jet_d2_massCut, accD2(jet));
  }
}

void TCCPlots::fillMoments2Leadings(std::vector<const xAOD::Jet*>& jets, std::vector<const xAOD::Jet*>& calojets) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (jets.at(0)) {
    if (accD2.isAvailable(*jets.at(0))) {
      fillHisto(m_jet_d2_leading       , accD2(*jets.at(0)));
      fillHisto(m_jet_d2_2leadings     , accD2(*jets.at(0)));
      if (calojets.at(0)) {
	float mass = calojets.at(0)->m();
	if (mass > m_minMass and mass < m_maxMass) {
	  fillHisto(m_jet_d2_leading_massCut  , accD2(*jets.at(0)));
	  fillHisto(m_jet_d2_2leadings_massCut, accD2(*jets.at(0)));
	}
      }
    }
  }
  
  if (jets.at(1)) {
    if (accD2.isAvailable(*jets.at(1))) {
      fillHisto(m_jet_d2_subleading     , accD2(*jets.at(1)));
      fillHisto(m_jet_d2_2leadings      , accD2(*jets.at(1)));
      if (calojets.at(1)) {
	float mass = calojets.at(1)->m();
	if (mass > m_minMass and mass < m_maxMass) {
	  fillHisto(m_jet_d2_subleading_massCut, accD2(*jets.at(1)));
	  fillHisto(m_jet_d2_2leadings_massCut , accD2(*jets.at(1)));
	}
      }
    }
  }   
}

void TCCPlots::fill2Leadings(std::vector<const xAOD::Jet*>& jets, std::vector<const xAOD::Jet*>& truths) {
  if (jets.at(0)) {
    if (truths.at(0)) {
      fillHisto(m_jet_response_m_leading     , jets.at(0)->m() /truths.at(0)->m() );
      fillHisto(m_jet_response_pt_leading    , jets.at(0)->pt()/truths.at(0)->pt());
      fillHisto(m_jet_response_m_2leadings   , jets.at(0)->m() /truths.at(0)->m() );
      fillHisto(m_jet_response_pt_2leadings  , jets.at(0)->pt()/truths.at(0)->pt());
    }
    fillHisto(m_jet_m_2leadings  , jets.at(0)->m() /GeV);
    fillHisto(m_jet_m_leading    , jets.at(0)->m() /GeV);
    fillHisto(m_jet_pt_2leadings , jets.at(0)->pt()/GeV);
    fillHisto(m_jet_pt_leading   , jets.at(0)->pt()/GeV);
  }
  
  if (jets.at(1)) {
    if (truths.at(1)) {
      fillHisto(m_jet_response_m_subleading  , jets.at(1)->m() /truths.at(1)->m() );
      fillHisto(m_jet_response_pt_subleading , jets.at(1)->pt()/truths.at(1)->pt());
      fillHisto(m_jet_response_m_2leadings   , jets.at(1)->m() /truths.at(1)->m() );
      fillHisto(m_jet_response_pt_2leadings  , jets.at(1)->pt()/truths.at(1)->pt());
    }
    fillHisto(m_jet_m_2leadings   , jets.at(1)->m() /GeV);
    fillHisto(m_jet_m_subleading  , jets.at(1)->m() /GeV);
    fillHisto(m_jet_pt_2leadings  , jets.at(1)->pt()/GeV);
    fillHisto(m_jet_pt_subleading , jets.at(1)->pt()/GeV);
  }
}

void TCCPlots::fillPseudoResponse2Leadings(std::vector<const xAOD::Jet*>& jets, std::vector<const xAOD::Jet*>& calo) {
  if (jets.at(0) and calo.at(0)) {
      fillHisto(m_jet_pseudoresponse_m_leading     , calo.at(0)->m() /jets.at(0)->m() );
      fillHisto(m_jet_pseudoresponse_pt_leading    , calo.at(0)->pt()/jets.at(0)->pt());
      fillHisto(m_jet_pseudoresponse_m_2leadings   , calo.at(0)->m() /jets.at(0)->m() );
      fillHisto(m_jet_pseudoresponse_pt_2leadings  , calo.at(0)->pt()/jets.at(0)->pt());
    }
  if (jets.at(1) and calo.at(1)) {
      fillHisto(m_jet_pseudoresponse_m_subleading  , calo.at(1)->m() /jets.at(1)->m() );
      fillHisto(m_jet_pseudoresponse_pt_subleading , calo.at(1)->pt()/jets.at(1)->pt());
      fillHisto(m_jet_pseudoresponse_m_2leadings   , calo.at(1)->m() /jets.at(1)->m() );
      fillHisto(m_jet_pseudoresponse_pt_2leadings  , calo.at(1)->pt()/jets.at(1)->pt());
  }
}

void TCCPlots::finalizePlots() {
  
}