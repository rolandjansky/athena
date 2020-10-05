/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/src/TrigEgammaMassHypoTool.h
 **
 **   Description: - Hypothesis Tool: search for electron pairs with
 **                invariant mass in some interval; intended for Z->ee
 **                - Modified from TrigL2DielectronMassHypo by R. Goncalo
 **                - Modified from TrigEFDielectronMassHypo by
 **                  Debottam Bakshi Gupta
 **
 **   Author: T. Hrynova  <thrynova@mail.cern.ch>
 **
 **   Created:   Nov 13 2009
 **
 **************************************************************************/


#include "TrigEgammaMassHypoTool.h"

#include <cmath>

using namespace TrigCompositeUtils;

TrigEgammaMassHypoTool::TrigEgammaMassHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
    : ComboHypoToolBase(type, name, parent) {}


StatusCode TrigEgammaMassHypoTool::initialize()
{
  ATH_MSG_DEBUG("AcceptAll            = " << m_acceptAll );
  ATH_MSG_DEBUG("LowerMassCut         = " << m_lowerMassElectronClusterCut );
  ATH_MSG_DEBUG("UpperMassCut         = " << m_upperMassElectronClusterCut );

  if ( not m_monTool.name().empty() ) {
        ATH_CHECK( m_monTool.retrieve() );
        ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }

 ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}

bool TrigEgammaMassHypoTool::executeAlg(std::vector<LegDecision> &combination) const {
  auto massOfAccepted = Monitored::Scalar( "MassOfAccepted"   , -1.0 );
  auto monitorIt    = Monitored::Group( m_monTool, massOfAccepted);

//retrieve the electrons 
  std::vector<ElementLink<xAOD::IParticleContainer>> selected_electrons;
  for (auto el: combination){
    auto EL= el.second;    
    auto electronLink = TrigCompositeUtils::findLink<xAOD::IParticleContainer>( *EL, featureString() ).link;
    selected_electrons.push_back(electronLink);
  }
  auto electronLink1=selected_electrons[0];
  auto electronLink2=selected_electrons[1];
  TLorentzVector hlv1 = (*electronLink1)->p4();
  TLorentzVector hlv2 = (*electronLink2)->p4();
  massOfAccepted = (hlv1+hlv2).M();
  ATH_MSG_DEBUG("Found two Electrons with mass " <<massOfAccepted);

  // apply the cut
  bool pass=true;
  if (massOfAccepted < m_lowerMassElectronClusterCut || massOfAccepted > m_upperMassElectronClusterCut){ 
      ATH_MSG_DEBUG("Combination failed mass cut: " << massOfAccepted << " not in [" << m_lowerMassElectronClusterCut << "," <<  m_upperMassElectronClusterCut << "]");
      pass=false;
  }

  if (pass)
     ATH_MSG_DEBUG( " Invariant mass " << massOfAccepted << " is  within [" <<m_lowerMassElectronClusterCut<< "," << m_upperMassElectronClusterCut << "] This seleciton passed! ");
  
  return pass;

}




