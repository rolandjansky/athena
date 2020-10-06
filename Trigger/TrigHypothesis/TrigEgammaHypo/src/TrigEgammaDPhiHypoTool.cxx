/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/src/TrigEgammaDPhiHypoTool.h
 **
 **   Description: - Hypothesis Tool: search for photon pairs with
 **                deltaPhi more than a threshold value; intended for H->gg
 **
 **   Author: D. BakshiGupta  <debottam.bakshigupta@cern.ch>
 **
 **************************************************************************/


#include "TrigEgammaDPhiHypoTool.h"

#include <cmath>

using namespace TrigCompositeUtils;

TrigEgammaDPhiHypoTool::TrigEgammaDPhiHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
    : ComboHypoToolBase(type, name, parent) {}


StatusCode TrigEgammaDPhiHypoTool::initialize()
{
  ATH_MSG_DEBUG("AcceptAll            = " << m_acceptAll );
  ATH_MSG_DEBUG("ThresholdDPhiCut         = " << m_thresholdDPhiCut );

  if ( not m_monTool.name().empty() ) {
        ATH_CHECK( m_monTool.retrieve() );
        ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }

 ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}

bool TrigEgammaDPhiHypoTool::executeAlg(std::vector<LegDecision> &combination) const {

  auto dphiOfAccepted = Monitored::Scalar( "DphiOfAccepted"   , -99 );
  auto monitorIt    = Monitored::Group( m_monTool, dphiOfAccepted);
//retrieve the elements 
  std::vector<ElementLink<xAOD::IParticleContainer>> selected_photons;
  for (auto el: combination){
    auto EL= el.second;    
    auto photonLink = TrigCompositeUtils::findLink<xAOD::IParticleContainer>( *EL, featureString() ).link;
    selected_photons.push_back(photonLink);
  }
  auto photonLink1=selected_photons[0];
  auto photonLink2=selected_photons[1];
  TLorentzVector hlv1 = (*photonLink1)->p4();
  TLorentzVector hlv2 = (*photonLink2)->p4();
  dphiOfAccepted = hlv1.DeltaPhi(hlv2);
  ATH_MSG_DEBUG("Found two Photons with deltaPhi " <<dphiOfAccepted);

  // apply the cut
  bool pass=true;
  if (dphiOfAccepted<m_thresholdDPhiCut){ 
      ATH_MSG_DEBUG("Combination failed deltaPhi cut: " << dphiOfAccepted << " is below " << m_thresholdDPhiCut);
      pass=false;
  }

  if (pass)
     ATH_MSG_DEBUG( " deltaPhi " << dphiOfAccepted << " is above the threshold "<<m_thresholdDPhiCut<<" This selection passed! ");
  
return pass;

}




