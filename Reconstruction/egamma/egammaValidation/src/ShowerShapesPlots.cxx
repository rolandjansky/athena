/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ShowerShapesPlots.h"

using namespace egammaMonitoring;

void ShowerShapesPlots::initializePlots() {
  
  hadleak = new TH1D("hadleak", ";E_{hadleak} [GeV]; Hadronic leakage Events" , 100, -0.07,  0.13  ); 
  reta    = new TH1D("reta"   , ";R_{#eta}; R_{#eta} Events"                  , 355,  0.  ,  1.1005);
  rphi    = new TH1D("rphi"   , ";R_{#phi}; R_{#phi} Events"                  , 355,  0.  ,  1.1005);
  weta2   = new TH1D("weta2"  , ";W_{#etas2}; W_{#etas2} Events"              , 100,  0.  ,  0.03  );
  eratio  = new TH1D("eratio" , ";E_{ratio}; E_{ratio} Events"                , 100,  0.  ,  1.    );
  deltae  = new TH1D("deltae" , ";#DeltaE [GeV]; #DeltaE Events"              , 250,  0.  ,  0.5   );
  f1      = new TH1D("f1"     , ";f_{1}; f_{1} Events"                        , 100,  0.  ,  1.0   );    
  fside   = new TH1D("fside"  , ";f_{side}; f_{side} Events"                  , 350,  0.  ,  3.5   );
  wtots1  = new TH1D("wtots1" , ";w_{s, tot}; w_{s, tot} Events"              , 100,  0.  , 10.    );
  ws3     = new TH1D("ws3"    , ";w_{s, 3}; w_{s, 3} Events"                  , 100,  0.  ,  1.    );
  
} // initializePlots

void ShowerShapesPlots::fill(const xAOD::Egamma& egamma) {
  
  m_eta2 = fabs(egamma.caloCluster()->etaBE(2));
  
  if(egamma.showerShapeValue(m_rhad , xAOD::EgammaParameters::Rhad) &&
     egamma.showerShapeValue(m_rhad1, xAOD::EgammaParameters::Rhad1)) {  
    
    m_hadrleak = (m_eta2 >= 0.8 && m_eta2 < 1.37) ? m_rhad : m_rhad1;
    hadleak->Fill(m_hadrleak);
    
  }
  
  if(egamma.showerShapeValue(m_Reta, xAOD::EgammaParameters::Reta)) reta->Fill(m_Reta);
  if(egamma.showerShapeValue(m_Rphi, xAOD::EgammaParameters::Rphi)) rphi->Fill(m_Rphi);
  
  if(egamma.showerShapeValue(m_shweta2, xAOD::EgammaParameters::weta2)) weta2->Fill(m_shweta2);
  
  if(egamma.showerShapeValue(m_Eratio, xAOD::EgammaParameters::Eratio)) eratio->Fill(m_Eratio);
  if(egamma.showerShapeValue(m_DeltaE, xAOD::EgammaParameters::DeltaE)) deltae->Fill(m_DeltaE);
  
  if(egamma.showerShapeValue(m_frac_f1  , xAOD::EgammaParameters::f1)) f1->Fill(m_frac_f1);
  
  if(egamma.showerShapeValue(m_shfside, xAOD::EgammaParameters::fracs1)) fside->Fill(m_shfside);
  
  if(egamma.showerShapeValue(m_shwtots1, xAOD::EgammaParameters::wtots1)) wtots1->Fill(m_shwtots1);
  
  if(egamma.showerShapeValue(m_shws3, xAOD::EgammaParameters::weta1)) ws3->Fill(m_shws3);

}
