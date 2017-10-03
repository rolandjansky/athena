/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TauHistUtils/EVetoPlots.h"

namespace Tau{

EVetoPlots::EVetoPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):
   PlotBase(pParent, sDir),
   m_HadRadius(nullptr),
   m_EMRadius(nullptr),	 
   m_IsoFrac(nullptr),
   m_CentFrac(nullptr),
   m_id_BDTEleScore(nullptr),
   m_pt_eleBDTloose(nullptr),
   m_pt_eleBDTmed(nullptr),
   m_pt_eleBDTtight(nullptr),
   m_pt_eleBDTlooseHighPt(nullptr),
   m_pt_eleBDTmedHighPt(nullptr),
   m_pt_eleBDTtightHighPt(nullptr),
   m_sTauJetContainerName(sTauJetContainerName)
{
}
	
EVetoPlots::~EVetoPlots()
{
}


void EVetoPlots::initializePlots(){

  m_HadRadius = Book1D("HadRadius",m_sTauJetContainerName + " Had Radius; HadRadius; # Part",20,0,2.);
  m_EMRadius  = Book1D("EMRadius",m_sTauJetContainerName + " EM Radius; EMRadius; # Part",20,0,2.);
  m_IsoFrac   = Book1D("IsoFrac",m_sTauJetContainerName + " Iso Frac; Iso Frac; # Part",20,0,1.);
  m_CentFrac  = Book1D("CentFrac",m_sTauJetContainerName + " Cent Frac; Cent Frac; # Part",20,0,1.);

  m_id_BDTEleScore      = Book1D("id_BDTEleScore",m_sTauJetContainerName + " BDTEleScore ; BDTEleScore; # Tau",10,0.,1.05);
  m_pt_eleBDTloose      = Book1D("Pt_eleBDTloose",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,150.);
  m_pt_eleBDTmed        = Book1D("Pt_eleBDTmed",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,150.);
  m_pt_eleBDTtight      = Book1D("Pt_eleBDTtight",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,150.);
  m_pt_eleBDTlooseHighPt = Book1D("Pt_eleBDTlooseHighPt",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,1500.);
  m_pt_eleBDTmedHighPt   = Book1D("Pt_eleBDTmedHighPt",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,1500.);
  m_pt_eleBDTtightHighPt = Book1D("Pt_eleBDTtightHighPt",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,1500.);

}

void EVetoPlots::fill(const xAOD::TauJet& tau) {


  float avariable=0;

  bool test = false;

  test=tau.detail(xAOD::TauJetParameters::hadRadius, avariable);
  if(test)     m_HadRadius->Fill(avariable,1.);
 
  test=tau.detail(xAOD::TauJetParameters::EMRadius, avariable);
  if(test) m_EMRadius->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::isolFrac, avariable);
  if (test) m_IsoFrac->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::centFrac, avariable);
  if (test) m_CentFrac->Fill(avariable,1.);

  if(tau.hasDiscriminant(xAOD::TauJetParameters::BDTEleScore) )m_id_BDTEleScore->Fill(tau.discriminant(xAOD::TauJetParameters::BDTEleScore));
  if ( tau.isTau(xAOD::TauJetParameters::EleBDTLoose) ) {
     m_pt_eleBDTloose      ->Fill(tau.pt()/1000.0, 1.0);
     m_pt_eleBDTlooseHighPt->Fill(tau.pt()/1000.0, 1.0);
  }
  if ( tau.isTau(xAOD::TauJetParameters::EleBDTMedium) ) {
     m_pt_eleBDTmed      ->Fill(tau.pt()/1000.0, 1.0);
     m_pt_eleBDTmedHighPt->Fill(tau.pt()/1000.0, 1.0);
  }
  if ( tau.isTau(xAOD::TauJetParameters::EleBDTTight) ) {
     m_pt_eleBDTtight      ->Fill(tau.pt()/1000.0, 1.0);
     m_pt_eleBDTtightHighPt->Fill(tau.pt()/1000.0, 1.0);
  }

}

}
