/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleHistograms.h"
#include <typeinfo>
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ITHistSvc.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "AsgTools/AnaToolHandle.h"
namespace egammaMonitoring {

  StatusCode ParticleHistograms::initializePlots() {

    histoMap["pT"]   = new TH1D(Form("%s_%s",m_name.c_str(),"pT")  , ";p_{T} [GeV]; Track p_{T} Events",  40,            0,         200);
    histoMap["eta"]  = new TH1D(Form("%s_%s",m_name.c_str(),"eta") , ";#eta; Track #eta Events"        ,  40,           -3,           3);
    histoMap["phi"]  = new TH1D(Form("%s_%s",m_name.c_str(),"phi") , ";#phi; Track #phi Events"        ,  20, -TMath::Pi(), TMath::Pi());
    
    histoMap["pT_15GeV"]   = new TH1D(Form("%s_%s",m_name.c_str(),"pT_15GeV")  , ";p_{T} [GeV]; Track p_{T} Events",  40,            0,         200);
    histoMap["eta_15GeV"]  = new TH1D(Form("%s_%s",m_name.c_str(),"eta_15GeV") , ";#eta; Track #eta Events"        ,  40,           -3,           3);
    histoMap["phi_15GeV"]  = new TH1D(Form("%s_%s",m_name.c_str(),"phi_15GeV") , ";#phi; Track #phi Events"        ,  20, -TMath::Pi(), TMath::Pi());  

    ATH_CHECK(m_rootHistSvc->regHist(m_folder+"pT", histoMap["pT"]));
    ATH_CHECK(m_rootHistSvc->regHist(m_folder+"eta", histoMap["eta"]));
    ATH_CHECK(m_rootHistSvc->regHist(m_folder+"phi", histoMap["phi"]));

    ATH_CHECK(m_rootHistSvc->regHist(m_folder+"pT_15GeV", histoMap["pT_15GeV"]));
    ATH_CHECK(m_rootHistSvc->regHist(m_folder+"eta_15GeV", histoMap["eta_15GeV"]));
    ATH_CHECK(m_rootHistSvc->regHist(m_folder+"phi_15GeV", histoMap["phi_15GeV"]));
    return StatusCode::SUCCESS;
  }
  
  void ParticleHistograms::fill(const xAOD::IParticle& egamma) {
    
    if((egamma.pt())/1000. > 0) histoMap["pT"]->Fill((egamma.pt())/1000.);
    histoMap["eta"]->Fill(egamma.eta());
    histoMap["phi"]->Fill(egamma.phi());

    if((egamma.pt())/1000. > 15) {
      histoMap["pT_15GeV"]->Fill((egamma.pt())/1000.);
      histoMap["eta_15GeV"]->Fill(egamma.eta());
      histoMap["phi_15GeV"]->Fill(egamma.phi());
    }

  }
  
}
