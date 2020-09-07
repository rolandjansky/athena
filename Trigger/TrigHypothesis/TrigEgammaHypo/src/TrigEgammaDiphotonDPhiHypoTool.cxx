/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/src/TrigEgammaDiphotonDPhiHypoTool.h
 **
 **   Description: - Hypothesis Tool: search for photon pairs with
 **                deltaPhi more than a threshold value; intended for H->gg
 **
 **   Author: D. BakshiGupta  <debottam.bakshigupta@cern.ch>
 **
 **************************************************************************/


#include "TrigEgammaDiphotonDPhiHypoTool.h"

#include <cmath>

using namespace TrigCompositeUtils;

TrigEgammaDiphotonDPhiHypoTool::TrigEgammaDiphotonDPhiHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
    : ComboHypoToolBase(type, name, parent) {}


StatusCode TrigEgammaDiphotonDPhiHypoTool::initialize()
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

bool TrigEgammaDiphotonDPhiHypoTool::executeAlg(std::vector<LegDecision> &combination) const {

//retrieve the electrons 
  std::vector<ElementLink<xAOD::PhotonContainer>> selected_photons;
  for (auto el: combination){
    auto EL= el.second;    
    auto photonLink = TrigCompositeUtils::findLink<xAOD::PhotonContainer>( *EL, featureString() ).link;
    selected_photons.push_back(photonLink);
  }
  auto photonLink1=selected_photons[0];
  auto photonLink2=selected_photons[1];
  TLorentzVector hlv1 = (*photonLink1)->p4();
  TLorentzVector hlv2 = (*photonLink2)->p4();
  double dPhi = hlv1.DeltaPhi(hlv2);
  ATH_MSG_DEBUG("Found two Photons with deltaPhi " <<dPhi);

  // apply the cut
  bool pass=true;
  if (dPhi<m_thresholdDPhiCut){ 
      ATH_MSG_DEBUG("Combination failed deltaPhi cut: " << dPhi << " not in " << m_thresholdDPhiCut);
      pass=false;
  }

  if (pass)
     ATH_MSG_DEBUG( " deltaPhi " << dPhi << " is above the threshold "<<m_thresholdDPhiCut<<" This seleciton passed! ");
  
return pass;

}




