/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/BDTinputPlots.h"

namespace Tau{
   
BDTinputPlots::BDTinputPlots(PlotBase* pParent, std::string sDir,
                             std::string sTauJetContainerName):
   PlotBase(pParent, sDir),
   m_id_BDTJetScore(nullptr),
   m_id_BDTEleScore(nullptr),
   m_pt_eleBDTloose(nullptr),
   m_pt_eleBDTmed(nullptr),
   m_pt_eleBDTtight(nullptr),
   m_pt_jetBDTloose(nullptr),
   m_pt_jetBDTmed(nullptr),
   m_pt_jetBDTtight(nullptr),
   m_sTauJetContainerName(sTauJetContainerName)
{
}
   
   
   
BDTinputPlots::~BDTinputPlots()
{
}
   
   
void BDTinputPlots::initializePlots(){
   
   /*+++++++++++++++++++++++++++++++++++++++++++++++++
     +++++++++++++++++++BDT OUTPUT+++++++++++++++++++++
     +++++++++++++++++++++++++++++++++++++++++++++++++*/
   
   m_id_BDTJetScore = Book1D("id_BDTJetScore",m_sTauJetContainerName + " BDTJetScore ; BDTJetScore; # Tau",10,-1.01,1.01);
   m_id_BDTEleScore = Book1D("id_BDTEleScore",m_sTauJetContainerName + " BDTEleScore ; BDTEleScore; # Tau",10,0.,1.01);
   m_pt_eleBDTloose = Book1D("Pt_eleBDTloose",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,300.);
   m_pt_eleBDTmed   = Book1D("Pt_eleBDTmed",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,300.);
   m_pt_eleBDTtight = Book1D("Pt_eleBDTtight",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,300.);
   m_pt_jetBDTloose = Book1D("Pt_jetBDTloose",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,300.);
   m_pt_jetBDTmed   = Book1D("Pt_jetBDTmed",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,300.);
   m_pt_jetBDTtight = Book1D("Pt_jetBDTtigth",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,300.);
   
   
   
}
   
void BDTinputPlots::fill(const xAOD::TauJet& thisTau) {
   
   /*+++++++++++++++++++++++++++++++++++++++++++++++++
     +++++++++++++++++++BDT OUTPUT+++++++++++++++++++++
     +++++++++++++++++++++++++++++++++++++++++++++++++*/
   
   if( thisTau.hasDiscriminant(xAOD::TauJetParameters::BDTJetScore) )m_id_BDTJetScore->Fill(thisTau.discriminant(xAOD::TauJetParameters::BDTJetScore));
   if( thisTau.hasDiscriminant(xAOD::TauJetParameters::BDTEleScore) )m_id_BDTEleScore->Fill(thisTau.discriminant(xAOD::TauJetParameters::BDTEleScore));
   if(thisTau.isTau(xAOD::TauJetParameters::JetBDTSigLoose))   m_pt_eleBDTloose->Fill( thisTau.pt()/1000,1);
   if(thisTau.isTau(xAOD::TauJetParameters::JetBDTSigMedium))  m_pt_eleBDTmed  ->Fill( thisTau.pt()/1000,1);
   if(thisTau.isTau(xAOD::TauJetParameters::JetBDTSigTight))   m_pt_eleBDTtight->Fill( thisTau.pt()/1000,1);
   if(thisTau.isTau(xAOD::TauJetParameters::EleBDTLoose))      m_pt_jetBDTloose->Fill( thisTau.pt()/1000,1);
   if(thisTau.isTau(xAOD::TauJetParameters::EleBDTMedium))     m_pt_jetBDTmed  ->Fill( thisTau.pt()/1000,1);
   if(thisTau.isTau(xAOD::TauJetParameters::EleBDTTight))      m_pt_jetBDTtight->Fill( thisTau.pt()/1000,1);
   
}
   
}
