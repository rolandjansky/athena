/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopObjectSelectionTools/AntiMuonMC15.h"

#include "TopEvent/EventTools.h"

namespace top {

AntiMuonMC15::AntiMuonMC15(const double ptcut, IsolationBase* /* isolation */) :
    m_ptcut(ptcut),
    m_muonSelectionTool("CP::MuonSelectionTool")
    //m_muonSelectionToolLoose("CP::MuonSelectionToolLoose"),
    //m_isolation(isolation)
{
  top::check( m_muonSelectionTool.retrieve() , "Failed to retrieve muonSelectionTool" );
//  top::check( m_muonSelectionToolLoose.retrieve() , "Failed to retrieve muonSelectionToolLoose" );
}

bool AntiMuonMC15::passSelection(const xAOD::Muon& mu) const 
{
    if (mu.pt() < m_ptcut)
        return false; 
    
    ///-- https://twiki.cern.ch/twiki/bin/view/AtlasProtected/MCPAnalysisGuidelinesMC15 --///
    if (!m_muonSelectionTool->accept(mu))
        return false;
    

    if (mu.energyLossType()!=xAOD::Muon::NotIsolated) return false;
    float eloss=0;
    bool ok=mu.parameter(eloss,xAOD::Muon::EnergyLoss);
    if (ok && eloss>6000) return false;
    float etcone20=0,ptvarcone40=0;
    ok=mu.isolation(etcone20,xAOD::Iso::etcone20);
    if (ok && etcone20/mu.pt()<0.03) return false;
    //if (mu.auxdataConst<float>("miniIso")/mu.pt() > .1) return false;
    ok=mu.isolation(ptvarcone40,xAOD::Iso::ptvarcone40);
    if (ok && ptvarcone40/mu.pt()>0.1) return false;

    return true;
}

bool AntiMuonMC15::passSelectionLoose(const xAOD::Muon& /*mu*/) const
{

  // code does not compile without this function
    
    return true;
}

  void AntiMuonMC15::print(std::ostream& os) const {
      os << "AntiMuonMC15\n"
        << "    * pT > " << m_ptcut << "\n"
//         << "    * |eta| < " << m_etamax << "\n"
//         << "    * quality=" <<  m_quality << " (tight=0, medium=1, loose=2, v.loose=3)\n"
        << "    * Everything else from muon tool - fill this in?\n";

//      if (!m_isolation)
//        os << "    * No isolation requirement\n";
//      else
//        m_isolation->print(os);
  }


  
}

