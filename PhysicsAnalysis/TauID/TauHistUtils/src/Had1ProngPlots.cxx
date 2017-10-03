/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/Had1ProngPlots.h"

namespace Tau{

Had1ProngPlots::Had1ProngPlots(PlotBase* pParent, std::string sDir,
			       std::string sTauJetContainerName):
   PlotBase(pParent, sDir),
   m_oGeneralTauPlots(this, "", sTauJetContainerName),
   m_tauNWideTracks(nullptr),
   m_tauCoreFrac(nullptr),
   m_tauEoverPTrk(nullptr),
   m_tauTrkAvgDist(nullptr),
   m_tauIpSig(nullptr),
   m_tauDRMax(nullptr),
   m_tauSflight(nullptr),
   m_tauMtrks(nullptr),
   m_SumPtTrkFrac(nullptr),
   m_innerTrkAvgDist(nullptr),
   m_ptRatioEflowApprox(nullptr),
   m_mEflowApprox(nullptr),
   m_ChPiEMEOverCaloEME(nullptr),
   m_EMPOverTrkSysP(nullptr),
   m_HadRadius(nullptr),
   m_EMRadius(nullptr),
   m_IsoFrac(nullptr),
   m_sTauJetContainerName(sTauJetContainerName)
{
}
	
Had1ProngPlots::~Had1ProngPlots()
{
}

void Had1ProngPlots::initializePlots(){

   // m_oGeneralTauPlots.initialize();
   m_tauCoreFrac   = Book1D("CoreFrac",m_sTauJetContainerName + " Tau CoreFrac; CoreFrac; # Taus",40,0.,2.);
   m_tauEoverPTrk  = Book1D("EoverPTrk",m_sTauJetContainerName + " Tau E over pLeadTrk; EoverP; # Taus",50,0.,4.);
   m_tauTrkAvgDist = Book1D("TrkAvgDist",m_sTauJetContainerName + " Tau track avg dist; TrkAvgDist; # Taus",10,-0.2,.8);
   m_tauIpSig      = Book1D("IpSig",m_sTauJetContainerName + " Tau lead track IP signif. ; IpSig; # Taus",30,-15.,15.);
   
   m_tauDRMax     = Book1D("DRMax",m_sTauJetContainerName + " Tau DR Max track-seed; DRMax; # Taus",20,0.,0.5);
   m_tauSflight   = Book1D("Sflight",m_sTauJetContainerName + " Tau flight sign. ; Sflight; # Taus",100,-10.,20.);
   m_tauMtrks     = Book1D("Mtracks",m_sTauJetContainerName + " Tau tracks mass ; Mtrks; # Taus",50,-1.,10.);
   m_SumPtTrkFrac = Book1D("SumPtTrkFrac",m_sTauJetContainerName + " Tau Sum PtTrk Frac ; SumPtTrkFrac; # Taus",100,-10.,20.);
   
   m_innerTrkAvgDist    = Book1D("innerTrkAvgDist",m_sTauJetContainerName + "Tau innerTrkAvgDist; innerTrkAvgDist; # of Taus",10,-0.2,.8);
   m_ptRatioEflowApprox = Book1D("ptRatioEflowApprox",m_sTauJetContainerName + "Tau ptRatioEflowApprox; ptRatioEflowApprox; # of Taus", 10, 0, 10);
   m_mEflowApprox       = Book1D("mEflowApprox",m_sTauJetContainerName + "Tau mEflowApprox; mEflowApprox; #Taus",30,0.0,3000.0);
   
   m_ChPiEMEOverCaloEME = Book1D("ChPiEMEOverCaloEME",m_sTauJetContainerName + "Tau ChPiEMEOverCaloEME; ChPiEMEOverCaloEME; # of Taus", 30, -15, 15);
   m_EMPOverTrkSysP     = Book1D("EMPOverTrkSysP",m_sTauJetContainerName + "Tau EMPOverTrkSysP; EMPOverTrkSysP; # of Taus", 21, -1, 20);
   
   m_HadRadius = Book1D("HadRadius",m_sTauJetContainerName + " Had Radius; HadRadius; # Part",20,0,2.);
   m_EMRadius  = Book1D("EMRadius",m_sTauJetContainerName + " EM Radius; EMRadius; # Part",20,0,2.);
   m_IsoFrac   = Book1D("IsoFrac",m_sTauJetContainerName + " Iso Frac; Iso Frac; # Part",20,0,1.);

}

void Had1ProngPlots::fill(const xAOD::TauJet& tau) {
  m_oGeneralTauPlots.fill(tau);

  float avariable=0;

  bool test = false;

  test=tau.detail(xAOD::TauJetParameters::centFrac, avariable);
  if(test)     m_tauCoreFrac->Fill(avariable,1.);
 
  test=tau.detail(xAOD::TauJetParameters::etOverPtLeadTrk, avariable);
  if(test) m_tauEoverPTrk->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::trkAvgDist, avariable);
  if (test) m_tauTrkAvgDist->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::ipSigLeadTrk, avariable);
  if (test) m_tauIpSig->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::dRmax, avariable);
  if (test) m_tauDRMax->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::trFlightPathSig, avariable);
  if (test) m_tauSflight->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::massTrkSys, avariable);
  if (test) m_tauMtrks->Fill(avariable/1000.,1.);

  test=tau.detail(xAOD::TauJetParameters::SumPtTrkFrac, avariable);
  if (test) m_SumPtTrkFrac->Fill(avariable/1000.,1.);

  test=tau.detail(xAOD::TauJetParameters::innerTrkAvgDist, avariable);
  if (test) m_innerTrkAvgDist->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::ptRatioEflowApprox, avariable);
  if (test) m_ptRatioEflowApprox->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::mEflowApprox, avariable);
  if (test) m_mEflowApprox->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::ChPiEMEOverCaloEME, avariable);
  if(test)     m_ChPiEMEOverCaloEME->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::EMPOverTrkSysP, avariable);
  if(test)     m_EMPOverTrkSysP->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::hadRadius, avariable);
  if(test)     m_HadRadius->Fill(avariable,1.);

  test=tau.detail(xAOD::TauJetParameters::EMRadius, avariable);
  if(test) m_EMRadius->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::isolFrac, avariable);
  if (test) m_IsoFrac->Fill(avariable, 1.);


}

}
