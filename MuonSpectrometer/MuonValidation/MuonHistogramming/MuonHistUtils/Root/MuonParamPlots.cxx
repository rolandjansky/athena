/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonHistUtils/MuonParamPlots.h"
#include "xAODTracking/TrackingPrimitives.h"

namespace Muon{

MuonParamPlots::MuonParamPlots(PlotBase *pParent, const std::string& sDir):PlotBase(pParent, sDir)
								   ,msInnerMatchChi2(nullptr)
								   ,msOuterMatchChi2(nullptr)
								   ,ELoss(nullptr)
								   ,ELossSigma(nullptr)
								   ,paramELoss(nullptr)
								   ,measELoss(nullptr)
{}

void MuonParamPlots::initializePlots()
{
  msInnerMatchChi2 = Book1D("msInnerMatchChi2","inner match #chi^{2};inner match #chi^{2};;Entries",100,0.,100.);
  msOuterMatchChi2 = Book1D("msOuterMatchChi2","outer match #chi^{2};outer match #chi^{2};;Entries",100,0.,100.);
    
  ELoss = Book1D("ELoss","ELoss;ELoss [GeV];Entries",100,0,20);
  ELossSigma = Book1D("ELossSigma","ELossSigma;ELossSgima [GeV];Entries",50,0,5);

  paramELoss = Book1D("paramELoss","paramELoss;paramELoss [GeV];Entries",50,0,10);
  measELoss = Book1D("measELoss","measELoss;measELoss [GeV];Entries",100,0,20);
}


  void MuonParamPlots::fill(const xAOD::Muon& mu, float weight)
{
  FillPlot(msInnerMatchChi2,mu,xAOD::Muon::msInnerMatchChi2, weight);
  FillPlot(msOuterMatchChi2,mu,xAOD::Muon::msOuterMatchChi2, weight);
#ifndef XAOD_ANALYSIS
  FillPlot(ELoss,mu,xAOD::Muon::EnergyLoss,0.001, weight);
  FillPlot(measELoss,mu,xAOD::Muon::MeasEnergyLoss,0.001, weight);  
  FillPlot(ELossSigma,mu,xAOD::Muon::EnergyLossSigma,0.001, weight);
  FillPlot(paramELoss,mu,xAOD::Muon::ParamEnergyLoss,0.001, weight);
#endif // not XAOD_ANALYSIS
}
  
  void MuonParamPlots::FillPlot(TH1* hist, const xAOD::Muon& mu,const xAOD::Muon::ParamDef paramDef,float scale, float weight) {
  if (mu.author()==xAOD::Muon::CaloTag || mu.author()==xAOD::Muon::CaloLikelihood || mu.author()==xAOD::Muon::ExtrapolateMuonToIP) return; //protection
  float fpar = 0;
  if (mu.isAvailable<float>("EnergyLoss")) {
     if (mu.parameter(fpar, paramDef)) 
       hist->Fill(scale*fpar, weight); //scale to GeV, if needed
  }
  return;
}

}//namespace Muon
