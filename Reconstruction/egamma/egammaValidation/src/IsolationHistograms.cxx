/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "IsolationHistograms.h"

#include "AthenaBaseComps/AthCheckMacros.h"

#include "GaudiKernel/ITHistSvc.h"
 
#include "TH1D.h"


using namespace egammaMonitoring;

StatusCode IsolationHistograms::initializePlots(bool do_var_histos) {

  histoMap["ptCone20"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptCone20" ), ";p_{T}^{cone20}; Events / 0.5 GeV", 60, 0., 15.));
  histoMap["ptCone30"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptCone30" ), ";p_{T}^{cone30}; Events / 0.5 GeV", 60, 0., 15.));

  if(do_var_histos){
    histoMap["ptVarCone20"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptVarCone20" ), ";p_{T}^{varCone20}; Events / 0.5 GeV", 60, 0., 15.));
    histoMap["ptVarCone30"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptVarCone30" ), ";p_{T}^{varCone30}; Events / 0.5 GeV", 60, 0., 15.));
  }
  histoMap["topoEtCone20"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"topoEtCone20" ), ";E_{T}^{topoCone20}; Events / 0.5 GeV", 60, -20., 30.));
  histoMap["topoEtCone30"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"topoEtCone30" ), ";E_{T}^{topoCone30}; Events / 0.5 GeV", 60, -20., 30.));
  histoMap["topoEtCone40"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"topoEtCone40" ), ";E_{T}^{topoCone40}; Events / 0.5 GeV", 60, -20., 30.));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptCone20", histoMap["ptCone20"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptCone30", histoMap["ptCone30"]));
  if(do_var_histos){
    ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptVarCone20", histoMap["ptVarCone20"]));
    ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptVarCone30", histoMap["ptVarCone30"]));
  }
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"topoEtCone20", histoMap["topoEtCone20"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"topoEtCone30", histoMap["topoEtCone30"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"topoEtCone40", histoMap["topoEtCone40"]));

  m_do_var_histos = do_var_histos;

  return StatusCode::SUCCESS;

} // initializePlots

void IsolationHistograms::fill(const xAOD::Egamma& egamma) {

  if(egamma.isolationValue(m_ptCone20, xAOD::Iso::ptcone20)) histoMap["ptCone20"]->Fill(std::min(m_ptCone20/1000., histoMap["ptCone20"]->GetBinCenter(histoMap["ptCone20"]->GetNbinsX())));
  if(egamma.isolationValue(m_ptCone30, xAOD::Iso::ptcone30)) histoMap["ptCone30"]->Fill(std::min(m_ptCone30/1000., histoMap["ptCone30"]->GetBinCenter(histoMap["ptCone30"]->GetNbinsX())));
  if(m_do_var_histos){
    if(egamma.isolationValue(m_ptVarCone20, xAOD::Iso::ptvarcone20)) histoMap["ptVarCone20"]->Fill(std::min(m_ptVarCone20/1000., histoMap["ptVarCone20"]->GetBinCenter(histoMap["ptVarCone20"]->GetNbinsX())));
    if(egamma.isolationValue(m_ptVarCone30, xAOD::Iso::ptvarcone30)) histoMap["ptVarCone30"]->Fill(std::min(m_ptVarCone30/1000., histoMap["ptVarCone30"]->GetBinCenter(histoMap["ptVarCone30"]->GetNbinsX())));
  }
  if(egamma.isolationValue(m_topoEtCone20, xAOD::Iso::topoetcone20)) histoMap["topoEtCone20"]->Fill(std::min(m_topoEtCone20/1000., histoMap["topoEtCone20"]->GetBinCenter(histoMap["topoEtCone20"]->GetNbinsX())));
  if(egamma.isolationValue(m_topoEtCone30, xAOD::Iso::topoetcone30)) histoMap["topoEtCone30"]->Fill(std::min(m_topoEtCone30/1000., histoMap["topoEtCone30"]->GetBinCenter(histoMap["topoEtCone30"]->GetNbinsX())));
  if(egamma.isolationValue(m_topoEtCone40, xAOD::Iso::topoetcone40)) histoMap["topoEtCone40"]->Fill(std::min(m_topoEtCone40/1000., histoMap["topoEtCone40"]->GetBinCenter(histoMap["topoEtCone40"]->GetNbinsX())));

}
