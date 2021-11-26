/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include <utility>

#include "EfficiencyPtPlots.h"
                       
namespace Tau{

  EfficiencyPtPlots::EfficiencyPtPlots(PlotBase* pParent, const std::string& sDir, std::string sTauJetContainerName):
    PlotBase(pParent, sDir),
    m_eff_pt_jetRNNloose(nullptr),
    m_eff_pt_jetRNNmed(nullptr),
    m_eff_pt_jetRNNtight(nullptr),
    m_eff_jetRNNloose(nullptr),
    m_eff_jetRNNmed(nullptr),
    m_eff_jetRNNtight(nullptr),
    m_sTauJetContainerName(std::move(sTauJetContainerName))
  {	
  }

  EfficiencyPtPlots::~EfficiencyPtPlots()
  {
  }

  void EfficiencyPtPlots::initializePlots()
  {
    m_eff_pt_jetRNNloose       = BookTProfile("Eff_Pt_jetRNNloose"," Matched Tau loose eff in pt; pt; eff", 20, 0., 150.0);
    m_eff_pt_jetRNNmed         = BookTProfile("Eff_Pt_jetRNNmed","Matched Tau med eff in pt; pt; eff", 20, 0.0, 150.0);
    m_eff_pt_jetRNNtight       = BookTProfile("Eff_Pt_jetRNNtight","Matched Tau tight eff in pt; pt; eff", 20, 0.0, 150.0);
    m_eff_pt_jetRNNlooseHighPt = BookTProfile("Eff_Pt_jetRNNlooseHightPt"," Matched Tau loose eff in pt; pt; eff", 20, 0.0, 1500.0);
    m_eff_pt_jetRNNmedHighPt   = BookTProfile("Eff_Pt_jetRNNmedHightPt","Matched Tau med eff in pt; pt; eff", 20, 0.0, 1500.0);
    m_eff_pt_jetRNNtightHighPt = BookTProfile("Eff_Pt_jetRNNtightHightPt","Matched Tau tight eff in pt; pt; eff", 20, 0.0, 1500.0);
    m_eff_jetRNNloose          = BookTProfile("Eff_jetRNNloose"," Matched Tau loose eff total; bin; eff",3,-1.5,1.5);
    m_eff_jetRNNmed            = BookTProfile("Eff_jetRNNmed","Matched Tau med eff total; bin; eff",3,-1.5,1.5);
    m_eff_jetRNNtight          = BookTProfile("Eff_jetRNNtight","Matched Tau tight eff total; bin; eff",3,-1.5,1.5);
  }

  void EfficiencyPtPlots::fill(const xAOD::TauJet& tau, float weight)
  {
    if ( tau.isTau(xAOD::TauJetParameters::JetRNNSigLoose) ) {
      m_eff_pt_jetRNNloose      ->Fill(tau.pt()/1000., 1., weight);
      m_eff_pt_jetRNNlooseHighPt->Fill(tau.pt()/1000., 1., weight);
      m_eff_jetRNNloose         ->Fill(0., 1., weight);
    }
    else {
      m_eff_pt_jetRNNloose      ->Fill(tau.pt()/1000., 0., weight);
      m_eff_pt_jetRNNlooseHighPt->Fill(tau.pt()/1000., 0., weight);
      m_eff_jetRNNloose         ->Fill(0., 0., weight);
    }
   
    if ( tau.isTau(xAOD::TauJetParameters::JetRNNSigMedium) ) {
      m_eff_pt_jetRNNmed      ->Fill(tau.pt()/1000., 1., weight);
      m_eff_pt_jetRNNmedHighPt->Fill(tau.pt()/1000., 1., weight);
      m_eff_jetRNNmed         ->Fill(0., 1., weight);
    }
    else {
      m_eff_pt_jetRNNmed      ->Fill(tau.pt()/1000., 0., weight);
      m_eff_pt_jetRNNmedHighPt->Fill(tau.pt()/1000., 0., weight);
      m_eff_jetRNNmed         ->Fill(0., 0., weight);
    }

    if ( tau.isTau(xAOD::TauJetParameters::JetRNNSigTight) ) {
      m_eff_pt_jetRNNtight      ->Fill(tau.pt()/1000., 1., weight);
      m_eff_pt_jetRNNtightHighPt->Fill(tau.pt()/1000., 1., weight);
      m_eff_jetRNNtight         ->Fill(0., 1., weight);
    }
    else {
      m_eff_pt_jetRNNtight      ->Fill(tau.pt()/1000., 0., weight);
      m_eff_pt_jetRNNtightHighPt->Fill(tau.pt()/1000., 0., weight);
      m_eff_jetRNNtight         ->Fill(0., 0., weight);
    }
  }

}
