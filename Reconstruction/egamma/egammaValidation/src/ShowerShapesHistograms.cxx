/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ShowerShapesHistograms.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/AnaToolHandle.h"
#include "xAODEgamma/ElectronxAODHelpers.h"

using namespace egammaMonitoring;

StatusCode ShowerShapesHistograms::initializePlots() {
  
  histoMap["hadleak"] = (new TH1D(Form("%s_%s",m_name.c_str(),"hadleak"), ";E_{hadleak} ; Hadronic leakage Events" , 100, -0.07,  0.13  ));
  histoMap["rhad"]    = (new TH1D(Form("%s_%s",m_name.c_str(),"rhad"), ";R_{had}; Events" , 100, -0.07,  0.13  ));
  histoMap["reta"]    = (new TH1D(Form("%s_%s",m_name.c_str(),"reta"   ), ";R_{#eta}; R_{#eta} Events"                  , 355,  0.  ,  1.1005));
  histoMap["rphi"]    = (new TH1D(Form("%s_%s",m_name.c_str(),"rphi"   ), ";R_{#phi}; R_{#phi} Events"                  , 355,  0.  ,  1.1005));
  histoMap["weta2"]   = (new TH1D(Form("%s_%s",m_name.c_str(),"weta2"  ), ";W_{#etas2}; W_{#etas2} Events"              , 100,  0.  ,  0.03  ));
  histoMap["eratio"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"eratio" ), ";E_{ratio}; E_{ratio} Events"                , 100,  0.  ,  1.    ));
  histoMap["deltae"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"deltae" ), ";#DeltaE [GeV]; #DeltaE Events"              , 100,  0.  ,  0.1   ));
  histoMap["f1"]      = (new TH1D(Form("%s_%s",m_name.c_str(),"f1"     ), ";f_{1}; f_{1} Events"                        , 100,  0.  ,  1.0   ));
  histoMap["fside"]   = (new TH1D(Form("%s_%s",m_name.c_str(),"fside"  ), ";f_{side}; f_{side} Events"                  , 100,  0.  ,  2.0   ));
  histoMap["wtots1"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"wtots1" ), ";w_{s, tot}; w_{s, tot} Events"              , 100,  0.  , 10.    ));
  histoMap["ws3"]     = (new TH1D(Form("%s_%s",m_name.c_str(),"ws3"    ), ";w_{s, 3}; w_{s, 3} Events"                  , 100,  0.  ,  1.    ));
  histoMap["lateral"] = (new TH1D(Form("%s_%s",m_name.c_str(),"lateral"), ";Lateral of seed; Events", 50, 0, 1));
  histoMap["second_R"] = (new TH1D(Form("%s_%s",m_name.c_str(),"second_R"), ";Second R; Events", 150,0. , 15000.));
  histoMap["EMFrac"] = (new TH1D(Form("%s_%s",m_name.c_str(),"EMFrac"), ";EMFrac; Events", 50, 0, 1));
  histo2DMap["lateral_second_R_2D"] = (new TH2D(Form("%s_%s",m_name.c_str(),"lateral_second_R_2D"), ";Lateral of seed; Second R", 50, 0, 1, 150,0.,15000.));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"lateral", histoMap["lateral"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"second_R", histoMap["second_R"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"EMFrac", histoMap["EMFrac"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"lateral_second_R_2D", histo2DMap["lateral_second_R_2D"]));
 
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"rhad", histoMap["rhad"]));
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

  float eta2 = -999;
  float rhad = -999;
  float rhad1 = -999;
  float hadrleak = -999;
  float Reta = -999;
  float Rphi = -999;
  float shweta2 = -999;
  float Eratio = -999;
  float DeltaE = -999;
  float frac_f1 = -999;
  float shfside = -999;
  float shwtots1= -999;
  float shws3= -999; 
  
  eta2 = fabs(egamma.caloCluster()->etaBE(2));
  
  if(egamma.showerShapeValue(rhad , xAOD::EgammaParameters::Rhad) &&
     egamma.showerShapeValue(rhad1, xAOD::EgammaParameters::Rhad1)) {  
    
    hadrleak = (eta2 >= 0.8 && eta2 < 1.37) ? rhad : rhad1;
    histoMap["hadleak"]->Fill(hadrleak);
    
  }
 
  double lateral(0.);
  double emfrac(0.);
  double second_r(0.);
  const std::vector<const xAOD::CaloCluster*> topoclusters = xAOD::EgammaHelpers::getAssociatedTopoClusters(egamma.caloCluster());

  if (topoclusters.at(0)->retrieveMoment(xAOD::CaloCluster::LATERAL,lateral)) {
    histoMap["lateral"]->Fill(lateral);
  }

  if (topoclusters.at(0)->retrieveMoment(xAOD::CaloCluster::ENG_FRAC_EM ,emfrac)) {
    histoMap["EMFrac"]->Fill(emfrac);
  }

  if (topoclusters.at(0)->retrieveMoment(xAOD::CaloCluster::SECOND_R,second_r)) {
    histoMap["second_R"]->Fill(second_r);
  }

  histo2DMap["lateral_second_R_2D"]->Fill(lateral,second_r);  

  if(egamma.showerShapeValue(rhad, xAOD::EgammaParameters::Rhad)) histoMap["rhad"]->Fill(rhad);
  if(egamma.showerShapeValue(Reta, xAOD::EgammaParameters::Reta)) histoMap["reta"]->Fill(Reta);
  if(egamma.showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi)) histoMap["rphi"]->Fill(Rphi);
  if(egamma.showerShapeValue(shweta2, xAOD::EgammaParameters::weta2)) histoMap["weta2"]->Fill(shweta2);
  if(egamma.showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio)) histoMap["eratio"]->Fill(Eratio);
  if(egamma.showerShapeValue(DeltaE, xAOD::EgammaParameters::DeltaE)) histoMap["deltae"]->Fill(DeltaE);
  if(egamma.showerShapeValue(frac_f1  , xAOD::EgammaParameters::f1)) histoMap["f1"]->Fill(frac_f1);
  if(egamma.showerShapeValue(shfside, xAOD::EgammaParameters::fracs1)) histoMap["fside"]->Fill(shfside);
  if(egamma.showerShapeValue(shwtots1, xAOD::EgammaParameters::wtots1)) histoMap["wtots1"]->Fill(shwtots1);
  if(egamma.showerShapeValue(shws3, xAOD::EgammaParameters::weta1)) histoMap["ws3"]->Fill(shws3);

}
