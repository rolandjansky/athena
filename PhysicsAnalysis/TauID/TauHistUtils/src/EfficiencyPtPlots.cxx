/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/EfficiencyPtPlots.h"
                       
namespace Tau{

EfficiencyPtPlots::EfficiencyPtPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):
   PlotBase(pParent, sDir),
   m_eff_pt_jetBDTloose(nullptr),
   m_eff_pt_jetBDTmed(nullptr),
   m_eff_pt_jetBDTtight(nullptr),
   m_eff_jetBDTloose(nullptr),
   m_eff_jetBDTmed(nullptr),
   m_eff_jetBDTtight(nullptr),
   m_sTauJetContainerName(sTauJetContainerName)
{	
}

EfficiencyPtPlots::~EfficiencyPtPlots()
{
}

void EfficiencyPtPlots::initializePlots()
{
   m_eff_pt_jetBDTloose       = BookTProfile("Eff_Pt_jetBDTloose"," Matched Tau loose eff in pt; pt; eff", 20, 0., 150.0);
   m_eff_pt_jetBDTmed         = BookTProfile("Eff_Pt_jetBDTmed","Matched Tau med eff in pt; pt; eff", 20, 0.0, 150.0);
   m_eff_pt_jetBDTtight       = BookTProfile("Eff_Pt_jetBDTtight","Matched Tau tight eff in pt; pt; eff", 20, 0.0, 150.0);
   m_eff_pt_jetBDTlooseHighPt = BookTProfile("Eff_Pt_jetBDTlooseHightPt"," Matched Tau loose eff in pt; pt; eff", 20, 0.0, 1500.0);
   m_eff_pt_jetBDTmedHighPt   = BookTProfile("Eff_Pt_jetBDTmedHightPt","Matched Tau med eff in pt; pt; eff", 20, 0.0, 1500.0);
   m_eff_pt_jetBDTtightHighPt = BookTProfile("Eff_Pt_jetBDTtightHightPt","Matched Tau tight eff in pt; pt; eff", 20, 0.0, 1500.0);
   m_eff_jetBDTloose          = BookTProfile("Eff_jetBDTloose"," Matched Tau loose eff total; bin; eff",3,-1.5,1.5);
   m_eff_jetBDTmed            = BookTProfile("Eff_jetBDTmed","Matched Tau med eff total; bin; eff",3,-1.5,1.5);
   m_eff_jetBDTtight          = BookTProfile("Eff_jetBDTtight","Matched Tau tight eff total; bin; eff",3,-1.5,1.5);
}

void EfficiencyPtPlots::fill(const xAOD::TauJet& tau)
{
   if ( tau.isTau(xAOD::TauJetParameters::JetBDTSigLoose) ) {
      m_eff_pt_jetBDTloose      ->Fill(tau.pt()/1000, 1.0, 1.0);
      m_eff_pt_jetBDTlooseHighPt->Fill(tau.pt()/1000, 1.0, 1.0);
      m_eff_jetBDTloose         ->Fill(0.0,           1.0, 1.0);
   }
   else {
      m_eff_pt_jetBDTloose      ->Fill(tau.pt()/1000, 0.0, 1.0);
      m_eff_pt_jetBDTlooseHighPt->Fill(tau.pt()/1000, 0.0, 1.0);
      m_eff_jetBDTloose         ->Fill(0.0,           0.0, 1.0);
   }
   
   if ( tau.isTau(xAOD::TauJetParameters::JetBDTSigMedium) ) {
      m_eff_pt_jetBDTmed      ->Fill(tau.pt()/1000, 1.0, 1.0);
      m_eff_pt_jetBDTmedHighPt->Fill(tau.pt()/1000, 1.0, 1.0);
      m_eff_jetBDTmed         ->Fill(0.0,           1.0, 1.0);
   }
   else {
      m_eff_pt_jetBDTmed      ->Fill(tau.pt()/1000, 0.0, 1.0);
      m_eff_pt_jetBDTmedHighPt->Fill(tau.pt()/1000, 0.0, 1.0);
      m_eff_jetBDTmed         ->Fill(0.0,           0.0, 1.0);
   }

   if ( tau.isTau(xAOD::TauJetParameters::JetBDTSigTight) ) {
      m_eff_pt_jetBDTtight      ->Fill(tau.pt()/1000, 1.0, 1.0);
      m_eff_pt_jetBDTtightHighPt->Fill(tau.pt()/1000, 1.0, 1.0);
      m_eff_jetBDTtight         ->Fill(0.0,           1.0, 1.0);
   }
   else {
      m_eff_pt_jetBDTtight      ->Fill(tau.pt()/1000, 0.0, 1.0);
      m_eff_pt_jetBDTtightHighPt->Fill(tau.pt()/1000, 0.0, 1.0);
      m_eff_jetBDTtight         ->Fill(0.0,           0.0, 1.0);
   }

}


}
