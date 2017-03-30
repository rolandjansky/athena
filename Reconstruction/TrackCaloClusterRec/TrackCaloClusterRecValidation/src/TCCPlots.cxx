/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TCCPlots.h"
#include <cmath>

using CLHEP::GeV;

TCCPlots::TCCPlots(TCCPlotsBase* pParent, const std::string& sDir) :
  TCCPlotsBase(pParent, sDir),
  m_jet_n                           (nullptr),
  m_jet_pt                          (nullptr),
  m_jet_eta                         (nullptr),
  m_jet_phi                         (nullptr),
  m_jet_m                           (nullptr),
  m_jet_m_2leadings                 (nullptr),
  m_jet_pt_2leadings                (nullptr),
  m_jet_eta_2leadings               (nullptr),
  m_jet_phi_2leadings               (nullptr),
  m_jet_m_leading                   (nullptr),
  m_jet_pt_leading                  (nullptr),
  m_jet_eta_leading                 (nullptr),
  m_jet_phi_leading                 (nullptr),
  m_jet_m_subleading                (nullptr),
  m_jet_pt_subleading               (nullptr),
  m_jet_eta_subleading              (nullptr),
  m_jet_phi_subleading              (nullptr),
  m_jet_deltaR                      (nullptr),
  m_jet_deltaR_matched              (nullptr),
  m_jet_response_m                  (nullptr),
  m_jet_response_m_pt               (nullptr),
  m_jet_response_pt                 (nullptr),
  m_jet_response_eta                (nullptr),
  m_jet_response_phi                (nullptr),
  m_jet_response_m_2leadings        (nullptr),
  m_jet_response_m_pt_2leadings     (nullptr),
  m_jet_response_pt_2leadings       (nullptr),
  m_jet_response_eta_2leadings      (nullptr),
  m_jet_response_phi_2leadings      (nullptr),
  m_jet_response_m_leading          (nullptr),
  m_jet_response_m_pt_leading       (nullptr),
  m_jet_response_pt_leading         (nullptr),
  m_jet_response_eta_leading        (nullptr),
  m_jet_response_phi_leading        (nullptr),
  m_jet_response_m_subleading       (nullptr),
  m_jet_response_m_pt_subleading    (nullptr),
  m_jet_response_pt_subleading      (nullptr),
  m_jet_response_eta_subleading     (nullptr),
  m_jet_response_phi_subleading     (nullptr), 
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
  m_jet_d2_subleading_massCut       (nullptr),
  m_jet_response_d2                 (nullptr),
  m_jet_response_d2_2leadings       (nullptr),
  m_jet_response_d2_leading         (nullptr),
  m_jet_response_d2_subleading      (nullptr),
  m_jet_resolution_m                (nullptr),
  m_jet_resolution_m_2leadings      (nullptr),
  m_jet_resolution_m_leading        (nullptr),
  m_jet_resolution_m_subleading     (nullptr) {
}

void TCCPlots::initializePlots() {
  book(m_jet_n                             , "jet_n"                            );
  book(m_jet_pt                            , "jet_pt"                           );
  book(m_jet_eta                           , "jet_eta"                          );
  book(m_jet_phi                           , "jet_phi"                          );
  book(m_jet_m                             , "jet_m"                            );
  book(m_jet_m_2leadings                   , "jet_m_2leadings"                  );
  book(m_jet_pt_2leadings                  , "jet_pt_2leadings"                 );
  book(m_jet_eta_2leadings                 , "jet_eta_2leadings"                );
  book(m_jet_phi_2leadings                 , "jet_phi_2leadings"                );
  book(m_jet_m_leading                     , "jet_m_leading"                    );
  book(m_jet_pt_leading                    , "jet_pt_leading"                   );
  book(m_jet_eta_leading                   , "jet_eta_leading"                  );
  book(m_jet_phi_leading                   , "jet_phi_leading"                  );
  book(m_jet_m_subleading                  , "jet_m_subleading"                 );
  book(m_jet_pt_subleading                 , "jet_pt_subleading"                );
  book(m_jet_eta_subleading                , "jet_eta_subleading"               );
  book(m_jet_phi_subleading                , "jet_phi_subleading"               );
  book(m_jet_deltaR                        , "jet_deltaR"                       );
  book(m_jet_deltaR_matched                , "jet_deltaR_matched"               );
  book(m_jet_response_m                    , "jet_response_m"                   );
  book(m_jet_response_m_pt                 , "jet_response_m_pt"                );
  book(m_jet_response_pt                   , "jet_response_pt"                  );
  book(m_jet_response_m_2leadings          , "jet_response_m_2leadings"         );
  book(m_jet_response_m_pt_2leadings       , "jet_response_m_pt_2leadings"      );
  book(m_jet_response_pt_2leadings         , "jet_response_pt_2leadings"        );
  book(m_jet_response_m_leading            , "jet_response_m_leading"           );
  book(m_jet_response_m_pt_leading         , "jet_response_m_pt_leading"        );
  book(m_jet_response_pt_leading           , "jet_response_pt_leading"          );
  book(m_jet_response_m_subleading         , "jet_response_m_subleading"        );
  book(m_jet_response_m_pt_subleading      , "jet_response_m_pt_subleading"     );
  book(m_jet_response_pt_subleading        , "jet_response_pt_subleading"       );
  book(m_jet_response_eta                  , "jet_response_eta"                 );
  book(m_jet_response_phi                  , "jet_response_phi"                 );
  book(m_jet_response_eta_2leadings        , "jet_response_eta_2leadings"       );
  book(m_jet_response_phi_2leadings        , "jet_response_phi_2leadings"       );
  book(m_jet_response_eta_leading          , "jet_response_eta_leading"         );
  book(m_jet_response_phi_leading          , "jet_response_phi_leading"         );
  book(m_jet_response_eta_subleading       , "jet_response_eta_subleading"      );
  book(m_jet_response_phi_subleading       , "jet_response_phi_subleading"      );
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
  book(m_jet_response_d2                   , "jet_response_d2"                  );
  book(m_jet_response_d2_2leadings         , "jet_response_d2_2leadings"        );
  book(m_jet_response_d2_leading           , "jet_response_d2_leading"          );
  book(m_jet_response_d2_subleading        , "jet_response_d2_subleading"       ); 
  book(m_jet_resolution_m                  , "jet_resolution_m"                 );
  book(m_jet_resolution_m_2leadings        , "jet_resolution_m_2leadings"       );
  book(m_jet_resolution_m_leading          , "jet_resolution_m_leading"         );
  book(m_jet_resolution_m_subleading       , "jet_resolution_m_subleading"      );
  
  
  // change pT binning for the mass response vs pT
  float ptBins[18] = { 200., 300., 400., 500.,
                       600., 700., 800., 900.,
		       1000., 1100., 1200., 1300.,
		       1500., 1700., 1900., 2100.,
		       2500., 3000. };
  m_jet_response_m_pt            ->GetXaxis()->Set(sizeof(ptBins)/sizeof(*ptBins)-1, ptBins);
  m_jet_response_m_pt_2leadings  ->GetXaxis()->Set(sizeof(ptBins)/sizeof(*ptBins)-1, ptBins);
  m_jet_response_m_pt_leading    ->GetXaxis()->Set(sizeof(ptBins)/sizeof(*ptBins)-1, ptBins);
  m_jet_response_m_pt_subleading ->GetXaxis()->Set(sizeof(ptBins)/sizeof(*ptBins)-1, ptBins);  
  m_jet_resolution_m             ->GetXaxis()->Set(sizeof(ptBins)/sizeof(*ptBins)-1, ptBins);          
  m_jet_resolution_m_2leadings   ->GetXaxis()->Set(sizeof(ptBins)/sizeof(*ptBins)-1, ptBins);
  m_jet_resolution_m_leading     ->GetXaxis()->Set(sizeof(ptBins)/sizeof(*ptBins)-1, ptBins);
  m_jet_resolution_m_subleading  ->GetXaxis()->Set(sizeof(ptBins)/sizeof(*ptBins)-1, ptBins);
  
  
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
  fillHisto(m_jet_response_eta   , jet.eta()/truth.eta());
  fillHisto(m_jet_response_phi   , jet.phi()/truth.phi());
  
  fillHisto(m_jet_response_m_pt  , truth.pt()/GeV, jet.m()/truth.m() );
  
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet) and accD2.isAvailable(truth)) {
    fillHisto(m_jet_response_d2 , accD2(jet)/accD2(truth));
    fillHisto(m_jet_response_d2 , accD2(jet)/accD2(truth));
  }
}

void TCCPlots::fillPseudoResponse(const xAOD::Jet& jet, const xAOD::Jet& calo) {
  fillHisto(m_jet_pseudoresponse_m     , jet.m() /calo.m() );
  fillHisto(m_jet_pseudoresponse_pt    , jet.pt()/calo.pt());
}

void TCCPlots::fillMoments(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet))
    fillHisto(m_jet_d2, accD2(jet));
}

void TCCPlots::fillMomentsWithMassCut(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet))
    fillHisto(m_jet_d2_massCut, accD2(jet));
}

void TCCPlots::fillMomentsLeading(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet)) {
    fillHisto(m_jet_d2_leading       , accD2(jet));
    fillHisto(m_jet_d2_2leadings     , accD2(jet));
  }
}

void TCCPlots::fillMomentsLeadingWithMassCut(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet)) {
    fillHisto(m_jet_d2_leading_massCut       , accD2(jet));
    fillHisto(m_jet_d2_2leadings_massCut     , accD2(jet));
  }
}

void TCCPlots::fillMomentsSubLeading(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet)) {
    fillHisto(m_jet_d2_subleading    , accD2(jet));
    fillHisto(m_jet_d2_2leadings     , accD2(jet));
  }
}

void TCCPlots::fillMomentsSubLeadingWithMassCut(const xAOD::Jet& jet) {
  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet)) {
    fillHisto(m_jet_d2_subleading_massCut    , accD2(jet));
    fillHisto(m_jet_d2_2leadings_massCut     , accD2(jet));
  }
}

void TCCPlots::fillResponseLeading(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_response_m_leading     , jet.m()  /truth.m() );
  fillHisto(m_jet_response_pt_leading    , jet.pt() /truth.pt());
  fillHisto(m_jet_response_m_2leadings   , jet.m()  /truth.m() );
  fillHisto(m_jet_response_pt_2leadings  , jet.pt() /truth.pt());
  fillHisto(m_jet_response_eta_leading   , jet.eta()/truth.eta());
  fillHisto(m_jet_response_phi_leading   , jet.phi()/truth.phi());
  fillHisto(m_jet_response_eta_2leadings , jet.eta()/truth.eta());
  fillHisto(m_jet_response_phi_2leadings , jet.phi()/truth.phi());
  
  fillHisto(m_jet_response_m_pt_2leadings  , truth.pt()/GeV, jet.m()/truth.m() );
  fillHisto(m_jet_response_m_pt_leading    , truth.pt()/GeV, jet.m()/truth.m() );

  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet) and accD2.isAvailable(truth)) {
    fillHisto(m_jet_response_d2_2leadings , accD2(jet)/accD2(truth));
    fillHisto(m_jet_response_d2_leading   , accD2(jet)/accD2(truth));
  }
}

void TCCPlots::fillLeading(const xAOD::Jet& jet) {
  fillHisto(m_jet_m_2leadings   , jet.m() /GeV);
  fillHisto(m_jet_m_leading     , jet.m() /GeV);
  fillHisto(m_jet_pt_2leadings  , jet.pt()/GeV);
  fillHisto(m_jet_pt_leading    , jet.pt()/GeV);
  fillHisto(m_jet_eta_2leadings , jet.eta());
  fillHisto(m_jet_eta_leading   , jet.eta());
  fillHisto(m_jet_phi_2leadings , jet.phi());
  fillHisto(m_jet_phi_leading   , jet.phi());
}

void TCCPlots::fillResponseSubLeading(const xAOD::Jet& jet, const xAOD::Jet& truth) {
  fillHisto(m_jet_response_m_subleading   , jet.m()  /truth.m() );
  fillHisto(m_jet_response_pt_subleading  , jet.pt() /truth.pt());
  fillHisto(m_jet_response_m_2leadings    , jet.m()  /truth.m() );
  fillHisto(m_jet_response_pt_2leadings   , jet.pt() /truth.pt());
  fillHisto(m_jet_response_eta_subleading , jet.eta()/truth.eta());
  fillHisto(m_jet_response_phi_subleading , jet.phi()/truth.phi());
  fillHisto(m_jet_response_eta_2leadings  , jet.eta()/truth.eta());
  fillHisto(m_jet_response_phi_2leadings  , jet.phi()/truth.phi());
  
  fillHisto(m_jet_response_m_pt_2leadings  , truth.pt()/GeV, jet.m()/truth.m() );
  fillHisto(m_jet_response_m_pt_subleading , truth.pt()/GeV, jet.m()/truth.m() );

  static SG::AuxElement::Accessor<float> accD2("D2");
  if (accD2.isAvailable(jet) and accD2.isAvailable(truth)) {
    fillHisto(m_jet_response_d2_2leadings    , accD2(jet)/accD2(truth));
    fillHisto(m_jet_response_d2_subleading   , accD2(jet)/accD2(truth));
  }
}

void TCCPlots::fillSubLeading(const xAOD::Jet& jet) {
  fillHisto(m_jet_m_2leadings    , jet.m() /GeV);
  fillHisto(m_jet_m_subleading   , jet.m() /GeV);
  fillHisto(m_jet_pt_2leadings   , jet.pt()/GeV);
  fillHisto(m_jet_pt_subleading  , jet.pt()/GeV);
  fillHisto(m_jet_eta_2leadings  , jet.eta());
  fillHisto(m_jet_eta_subleading , jet.eta());
  fillHisto(m_jet_phi_2leadings  , jet.phi());
  fillHisto(m_jet_phi_subleading , jet.phi());
}

void TCCPlots::fillPseudoResponseLeading(const xAOD::Jet& jet, const xAOD::Jet& calo) {
  fillHisto(m_jet_pseudoresponse_m_leading     , jet.m() /calo.m() );
  fillHisto(m_jet_pseudoresponse_pt_leading    , jet.pt()/calo.pt());
  fillHisto(m_jet_pseudoresponse_m_2leadings   , jet.m() /calo.m() );
  fillHisto(m_jet_pseudoresponse_pt_2leadings  , jet.pt()/calo.pt());
}

void TCCPlots::fillPseudoResponseSubLeading(const xAOD::Jet& jet, const xAOD::Jet& calo) {
  fillHisto(m_jet_pseudoresponse_m_subleading   , jet.m() /calo.m() );
  fillHisto(m_jet_pseudoresponse_pt_subleading  , jet.pt()/calo.pt());
  fillHisto(m_jet_pseudoresponse_m_2leadings    , jet.m() /calo.m() );
  fillHisto(m_jet_pseudoresponse_pt_2leadings   , jet.pt()/calo.pt());
}

void TCCPlots::finalizePlots() {
  make_median(m_jet_response_m_pt           , m_jet_resolution_m            );
  make_median(m_jet_response_m_pt_2leadings , m_jet_resolution_m_2leadings  );  
  make_median(m_jet_response_m_pt_leading   , m_jet_resolution_m_leading    );
  make_median(m_jet_response_m_pt_subleading, m_jet_resolution_m_subleading );
}

void TCCPlots::make_median(TH2* h2_response, TH1* h1_resolution){
  for (int i=1; i<=h2_response->GetNbinsX(); i++){
    TH1F* hold = new TH1F("","", h2_response->GetNbinsY(),
			  h2_response->GetYaxis()->GetBinCenter(1)-0.5*h2_response->GetYaxis()->GetBinWidth(1),
			  h2_response->GetYaxis()->GetBinCenter(h2_response->GetNbinsY())+0.5*h2_response->GetYaxis()->GetBinWidth(h2_response->GetNbinsY()));
    for (int j=1; j<=h2_response->GetNbinsY(); j++){
      hold->SetBinContent(j,h2_response->GetBinContent(i,j));
    }
    Double_t prob,quant25,quant75,median;
    prob=.5;
    hold->GetQuantiles(1,&median,&prob);
    prob=.75;
    hold->GetQuantiles(1,&quant75,&prob);
    prob=.25;
    hold->GetQuantiles(1,&quant25,&prob);
    h1_resolution->SetBinContent(i,fabs(quant75-quant25)/median);
  }
}