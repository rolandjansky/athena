/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "GeneralTauPlots.h"

namespace Tau{

GeneralTauPlots::GeneralTauPlots(PlotBase* pParent, const std::string& sDir, const std::string& sTauJetContainerName):
   PlotBase(pParent, sDir),
   m_oParamPlots(this, "", sTauJetContainerName),
   m_tauCharge(nullptr),
   m_tauNCoreTracks(nullptr),
   m_tauNWideTracks(nullptr),
   m_ptHighPt(nullptr),
   m_RNNScore(nullptr),
   m_RNNScoreSigTrans(nullptr),
   m_ptRNNVeryLoose(nullptr),
   m_ptRNNLoose(nullptr),
   m_ptRNNMedium(nullptr),
   m_ptRNNTight(nullptr),
   m_ptRNNVeryLooseHighPt(nullptr),
   m_ptRNNLooseHighPt(nullptr),
   m_ptRNNMediumHighPt(nullptr),
   m_ptRNNTightHighPt(nullptr),
   m_sTauJetContainerName(sTauJetContainerName)
{	
}

GeneralTauPlots::~GeneralTauPlots()
{
}

void GeneralTauPlots::initializePlots(){

   m_tauCharge      = Book1D("Charge",m_sTauJetContainerName + " Tau charge; charge; # Taus",7,-3.,4.);
   m_tauNCoreTracks = Book1D("NCoreTracks",m_sTauJetContainerName + " Tau n core tracks; nCore; # Taus",10,0.,10.);
   m_tauNWideTracks = Book1D("NWideTracks",m_sTauJetContainerName + " Tau n wide tracks; nWide; # Taus",10,0.,10.);
   m_ptHighPt = Book1D("ptHighPt", m_sTauJetContainerName+" HighPt"+"; pt; # Taus",20, 0.0, 1500.0);
   m_RNNScore = Book1D("RNNJetScore", m_sTauJetContainerName+" RNNJetScore;RNNJetScore;# Tau", 10, -1.01, 1.01);
   m_RNNScoreSigTrans = Book1D("RNNJetScoreSigTrans", m_sTauJetContainerName+" RNNJetScoreSigTrans;RNNJetScoreSigTrans;"+"# Tau", 10, -1.01, 1.01);
   m_ptRNNVeryLoose = Book1D("ptRNNSigVeryLoose", m_sTauJetContainerName+" RNNSigVeryLoose; pt; # Taus",20, 0.0, 150.0);
   m_ptRNNVeryLooseHighPt = Book1D("ptRNNSigVeryLooseHighPt", m_sTauJetContainerName+" RNNSigVeryLooseHighPt"+"; pt; # Taus",20, 0.0, 1500.0);
   m_ptRNNLoose = Book1D("ptRNNSigLoose",m_sTauJetContainerName+" RNNSigLoose; pt; # Taus", 20, 0.0, 150.0);
   m_ptRNNLooseHighPt = Book1D("ptRNNSigLooseHighPt", m_sTauJetContainerName+" RNNSigLooseHighPt; pt"+"; # Taus",20, 0.0, 1500.0);
   m_ptRNNMedium = Book1D("ptRNNSigMedium",m_sTauJetContainerName+" RNNSigMedium; pt; # Taus", 20, 0.0, 150.0);
   m_ptRNNMediumHighPt = Book1D("ptRNNSigMediumHighPt", m_sTauJetContainerName+" RNNSigMediumHighPt; pt"+"; # Taus",20, 0.0, 1500.0);
   m_ptRNNTight = Book1D("ptRNNSigTight",m_sTauJetContainerName+" RNNSigTight; pt; # Taus", 20, 0.0, 150.0);
   m_ptRNNTightHighPt = Book1D("ptRNNSigTightHighPt", m_sTauJetContainerName+" RNNSigTightHighPt; pt"+"; # Taus",20, 0.0, 1500.0);
}
  
void GeneralTauPlots::fill(const xAOD::TauJet& tau, float weight) {
  m_oParamPlots.fill(tau, weight);
  m_tauCharge->Fill(tau.charge(), weight); 
  m_tauNCoreTracks->Fill(tau.nTracks(), weight);
  m_tauNWideTracks->Fill(tau.nTracks(xAOD::TauJetParameters::classifiedIsolation), weight);
  m_ptHighPt->Fill(tau.pt()/1000, weight);

  static const SG::AuxElement::ConstAccessor<float> acc_RNNJetScore("RNNJetScore");
  if ( acc_RNNJetScore.isAvailable(tau) ) {
     float rnnScore = tau.discriminant(xAOD::TauJetParameters::RNNJetScore);
     if ( rnnScore > -2.0 ) m_RNNScore->Fill(rnnScore, weight);
  }
  static const SG::AuxElement::ConstAccessor<float> acc_RNNJetScoreSigTrans("RNNJetScoreSigTrans");
  if ( acc_RNNJetScoreSigTrans.isAvailable(tau) ) {
     float rnnScore = tau.discriminant(xAOD::TauJetParameters::RNNJetScoreSigTrans);
     if ( rnnScore > -2.0 ) m_RNNScoreSigTrans->Fill(rnnScore, weight);
  }
  if ( tau.isTau(xAOD::TauJetParameters::JetRNNSigVeryLoose) ) {
     m_ptRNNVeryLoose      ->Fill(tau.pt()/1000, weight);
     m_ptRNNVeryLooseHighPt->Fill(tau.pt()/1000, weight);
  }
  if ( tau.isTau(xAOD::TauJetParameters::JetRNNSigLoose) ) {
     m_ptRNNLoose      ->Fill(tau.pt()/1000, weight);
     m_ptRNNLooseHighPt->Fill(tau.pt()/1000, weight);
  }
  if ( tau.isTau(xAOD::TauJetParameters::JetRNNSigMedium) ) {
     m_ptRNNMedium      ->Fill(tau.pt()/1000, weight);
     m_ptRNNMediumHighPt->Fill(tau.pt()/1000, weight);
  }
  if ( tau.isTau(xAOD::TauJetParameters::JetRNNSigTight) ) {
     m_ptRNNTight      ->Fill(tau.pt()/1000, weight);
     m_ptRNNTightHighPt->Fill(tau.pt()/1000, weight);
  }

}


}
