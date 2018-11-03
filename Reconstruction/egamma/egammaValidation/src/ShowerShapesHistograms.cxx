/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ShowerShapesHistograms.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/AnaToolHandle.h"


using namespace egammaMonitoring;

StatusCode ShowerShapesHistograms::initializePlots() {
  
  histoMap["hadleak"] = (new TH1D(Form("%s_%s",m_name.c_str(),"hadleak"), ";E_{hadleak} [GeV]; Hadronic leakage Events" , 100, -0.07,  0.13  ));
  histoMap["reta"]    = (new TH1D(Form("%s_%s",m_name.c_str(),"reta"   ), ";R_{#eta}; R_{#eta} Events"                  , 355,  0.  ,  1.1005));
  histoMap["rphi"]    = (new TH1D(Form("%s_%s",m_name.c_str(),"rphi"   ), ";R_{#phi}; R_{#phi} Events"                  , 355,  0.  ,  1.1005));
  histoMap["weta2"]   = (new TH1D(Form("%s_%s",m_name.c_str(),"weta2"  ), ";W_{#etas2}; W_{#etas2} Events"              , 100,  0.  ,  0.03  ));
  histoMap["eratio"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"eratio" ), ";E_{ratio}; E_{ratio} Events"                , 100,  0.  ,  1.    ));
  histoMap["deltae"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"deltae" ), ";#DeltaE [GeV]; #DeltaE Events"              , 250,  0.  ,  0.5   ));
  histoMap["f1"]      = (new TH1D(Form("%s_%s",m_name.c_str(),"f1"     ), ";f_{1}; f_{1} Events"                        , 100,  0.  ,  1.0   ));
  histoMap["fside"]   = (new TH1D(Form("%s_%s",m_name.c_str(),"fside"  ), ";f_{side}; f_{side} Events"                  , 350,  0.  ,  3.5   ));
  histoMap["wtots1"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"wtots1" ), ";w_{s, tot}; w_{s, tot} Events"              , 100,  0.  , 10.    ));
  histoMap["ws3"]     = (new TH1D(Form("%s_%s",m_name.c_str(),"ws3"    ), ";w_{s, 3}; w_{s, 3} Events"                  , 100,  0.  ,  1.    ));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"hadleak", histoMap["hadleak"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"reta", histoMap["reta"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"rphi", histoMap["rphi"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"weta2", histoMap["weta2"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"eratio", histoMap["eratio"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"deltae", histoMap["deltae"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"f1", histoMap["f1"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"fside", histoMap["fside"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"wtots1", histoMap["wtots1"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ws3", histoMap["ws3"]));

  return StatusCode::SUCCESS;
  
} // initializePlots

void ShowerShapesHistograms::fill(const xAOD::Egamma& egamma) {
  
  m_eta2 = fabs(egamma.caloCluster()->etaBE(2));
  
  if(egamma.showerShapeValue(m_rhad , xAOD::EgammaParameters::Rhad) &&
     egamma.showerShapeValue(m_rhad1, xAOD::EgammaParameters::Rhad1)) {  
    
    m_hadrleak = (m_eta2 >= 0.8 && m_eta2 < 1.37) ? m_rhad : m_rhad1;
    histoMap["hadleak"]->Fill(m_hadrleak);
    
  }
  
  if(egamma.showerShapeValue(m_Reta, xAOD::EgammaParameters::Reta)) histoMap["reta"]->Fill(m_Reta);
  if(egamma.showerShapeValue(m_Rphi, xAOD::EgammaParameters::Rphi)) histoMap["rphi"]->Fill(m_Rphi);
  
  if(egamma.showerShapeValue(m_shweta2, xAOD::EgammaParameters::weta2)) histoMap["weta2"]->Fill(m_shweta2);
  
  if(egamma.showerShapeValue(m_Eratio, xAOD::EgammaParameters::Eratio)) histoMap["eratio"]->Fill(m_Eratio);
  if(egamma.showerShapeValue(m_DeltaE, xAOD::EgammaParameters::DeltaE)) histoMap["deltae"]->Fill(m_DeltaE);
  
  if(egamma.showerShapeValue(m_frac_f1  , xAOD::EgammaParameters::f1)) histoMap["f1"]->Fill(m_frac_f1);
  
  if(egamma.showerShapeValue(m_shfside, xAOD::EgammaParameters::fracs1)) histoMap["fside"]->Fill(m_shfside);
  
  if(egamma.showerShapeValue(m_shwtots1, xAOD::EgammaParameters::wtots1)) histoMap["wtots1"]->Fill(m_shwtots1);
  
  if(egamma.showerShapeValue(m_shws3, xAOD::EgammaParameters::weta1)) histoMap["ws3"]->Fill(m_shws3);

}
