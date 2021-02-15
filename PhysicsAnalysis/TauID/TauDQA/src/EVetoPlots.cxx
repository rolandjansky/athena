/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "EVetoPlots.h"

namespace Tau{

EVetoPlots::EVetoPlots(PlotBase* pParent, std::string sDir, std::string sTauJetContainerName):
   PlotBase(pParent, sDir),
   m_HadRadius(nullptr),
   m_EMRadius(nullptr),	 
   m_IsoFrac(nullptr),
   m_CentFrac(nullptr),
   m_id_RNNEleScore(nullptr),
   m_pt_eleRNNloose(nullptr),
   m_pt_eleRNNmed(nullptr),
   m_pt_eleRNNtight(nullptr),
   m_pt_eleRNNlooseHighPt(nullptr),
   m_pt_eleRNNmedHighPt(nullptr),
   m_pt_eleRNNtightHighPt(nullptr),
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

  m_id_RNNEleScore      = Book1D("id_RNNEleScore",m_sTauJetContainerName + " RNNEleScore ; RNNEleScore; # Tau",10,0.,1.05);
  m_pt_eleRNNloose      = Book1D("Pt_eleRNNloose",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,150.);
  m_pt_eleRNNmed        = Book1D("Pt_eleRNNmed",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,150.);
  m_pt_eleRNNtight      = Book1D("Pt_eleRNNtight",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,150.);
  m_pt_eleRNNlooseHighPt = Book1D("Pt_eleRNNlooseHighPt",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,1500.);
  m_pt_eleRNNmedHighPt   = Book1D("Pt_eleRNNmedHighPt",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,1500.);
  m_pt_eleRNNtightHighPt = Book1D("Pt_eleRNNtightHighPt",m_sTauJetContainerName + " Tau pt; pt; # Taus",20,0.,1500.);
}

void EVetoPlots::fill(const xAOD::TauJet& tau) {

  float avariable=0;

  bool test=tau.detail(xAOD::TauJetParameters::hadRadius, avariable);
  if(test)     m_HadRadius->Fill(avariable,1.);
 
  test=tau.detail(xAOD::TauJetParameters::EMRadius, avariable);
  if(test) m_EMRadius->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::isolFrac, avariable);
  if (test) m_IsoFrac->Fill(avariable, 1.);

  test=tau.detail(xAOD::TauJetParameters::centFrac, avariable);
  if (test) m_CentFrac->Fill(avariable,1.);

  if(tau.isAvailable<float>("RNNEleScore")) {
    m_id_RNNEleScore->Fill(tau.discriminant(xAOD::TauJetParameters::RNNEleScore));
    if ( tau.isTau(xAOD::TauJetParameters::EleRNNLoose) ) {
      m_pt_eleRNNloose      ->Fill(tau.pt()/1000.0, 1.0);
      m_pt_eleRNNlooseHighPt->Fill(tau.pt()/1000.0, 1.0);
    }
    if ( tau.isTau(xAOD::TauJetParameters::EleRNNMedium) ) {
      m_pt_eleRNNmed      ->Fill(tau.pt()/1000.0, 1.0);
      m_pt_eleRNNmedHighPt->Fill(tau.pt()/1000.0, 1.0);
    }
    if ( tau.isTau(xAOD::TauJetParameters::EleRNNTight) ) {
      m_pt_eleRNNtight      ->Fill(tau.pt()/1000.0, 1.0);
      m_pt_eleRNNtightHighPt->Fill(tau.pt()/1000.0, 1.0);
    }
  }
}

}
