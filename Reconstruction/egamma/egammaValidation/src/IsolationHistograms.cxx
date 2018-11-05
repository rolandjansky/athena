/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "IsolationHistograms.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AsgTools/AnaToolHandle.h"


using namespace egammaMonitoring;

StatusCode IsolationHistograms::initializePlots() {

  histoMap["ptCone20"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptCone20" ), ";p_{T}^{cone20}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["ptCone30"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptCone30" ), ";p_{T}^{cone30}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["ptCone40"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptCone40" ), ";p_{T}^{cone40}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["ptVarCone20"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptVarCone20" ), ";p_{T}^{varCone20}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["ptVarCone30"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptVarCone30" ), ";p_{T}^{varCone30}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["ptVarCone40"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"ptVarCone40" ), ";p_{T}^{varCone40}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["etCone20"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"etCone20" ), ";E_{T}^{cone20}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["etCone30"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"etCone30" ), ";E_{T}^{cone30}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["etCone40"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"etCone40" ), ";E_{T}^{cone40}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["topoEtCone20"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"topoEtCone20" ), ";E_{T}^{topoCone20}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["topoEtCone30"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"topoEtCone30" ), ";E_{T}^{topoCone30}; Events / 0.5 GeV", 60, 0., 30.));
  histoMap["topoEtCone40"]  = (new TH1D(Form("%s_%s",m_name.c_str(),"topoEtCone40" ), ";E_{T}^{topoCone40}; Events / 0.5 GeV", 60, 0., 30.));

  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptCone20", histoMap["ptCone20"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptCone30", histoMap["ptCone30"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptCone40", histoMap["ptCone40"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptVarCone20", histoMap["ptVarCone20"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptVarCone30", histoMap["ptVarCone30"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"ptVarCone40", histoMap["ptVarCone40"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"etCone20", histoMap["etCone20"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"etCone30", histoMap["etCone30"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"etCone40", histoMap["etCone40"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"topoEtCone20", histoMap["topoEtCone20"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"topoEtCone30", histoMap["topoEtCone30"]));
  ATH_CHECK(m_rootHistSvc->regHist(m_folder+"topoEtCone40", histoMap["topoEtCone40"]));

  return StatusCode::SUCCESS;

} // initializePlots

void IsolationHistograms::fill(const xAOD::Egamma& egamma) {

  if(egamma.isolationValue(m_ptCone20, xAOD::Iso::ptcone20) and m_ptCone20>=1000.) histoMap["ptCone20"]->Fill(m_ptCone20/1000.);
  if(egamma.isolationValue(m_ptCone30, xAOD::Iso::ptcone30) and m_ptCone30>=1000.) histoMap["ptCone30"]->Fill(m_ptCone30/1000.);
  if(egamma.isolationValue(m_ptCone40, xAOD::Iso::ptcone40) and m_ptCone40>=1000.) histoMap["ptCone40"]->Fill(m_ptCone40/1000.);
  if(egamma.isolationValue(m_ptVarCone20, xAOD::Iso::ptvarcone20) and m_ptVarCone20>=1000.) histoMap["ptVarCone20"]->Fill(m_ptVarCone20/1000.);
  if(egamma.isolationValue(m_ptVarCone30, xAOD::Iso::ptvarcone30) and m_ptVarCone30>=1000.) histoMap["ptVarCone30"]->Fill(m_ptVarCone30/1000.);
  if(egamma.isolationValue(m_ptVarCone40, xAOD::Iso::ptvarcone40) and m_ptVarCone40>=1000.) histoMap["ptVarCone40"]->Fill(m_ptVarCone40/1000.);
  if(egamma.isolationValue(m_etCone20, xAOD::Iso::etcone20)) histoMap["etCone20"]->Fill(m_etCone20/1000.);
  if(egamma.isolationValue(m_etCone30, xAOD::Iso::etcone30)) histoMap["etCone30"]->Fill(m_etCone30/1000.);
  if(egamma.isolationValue(m_etCone40, xAOD::Iso::etcone40)) histoMap["etCone40"]->Fill(m_etCone40/1000.);
  if(egamma.isolationValue(m_topoEtCone20, xAOD::Iso::topoetcone20)) histoMap["topoEtCone20"]->Fill(m_topoEtCone20/1000.);
  if(egamma.isolationValue(m_topoEtCone30, xAOD::Iso::topoetcone30)) histoMap["topoEtCone30"]->Fill(m_topoEtCone30/1000.);
  if(egamma.isolationValue(m_topoEtCone40, xAOD::Iso::topoetcone40)) histoMap["topoEtCone40"]->Fill(m_topoEtCone40/1000.);

}
