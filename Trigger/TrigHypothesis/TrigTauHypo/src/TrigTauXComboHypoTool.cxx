/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigTauXComboHypoTool.h"

#include <cmath>

using namespace TrigCompositeUtils;

TrigTauXComboHypoTool::TrigTauXComboHypoTool(const std::string& type, const std::string& name, const IInterface* parent)
  : ComboHypoToolBase(type, name, parent) {}


StatusCode TrigTauXComboHypoTool::initialize()
{
  ATH_MSG_DEBUG("AcceptAll         = " << m_acceptAll );
  ATH_MSG_DEBUG("dRCut min         = " << m_dRmin );
  ATH_MSG_DEBUG("dRCut max         = " << m_dRmax );

  if ( not m_monTool.name().empty() ) {
    ATH_CHECK( m_monTool.retrieve() );
    ATH_MSG_DEBUG("m_monTool name: " << m_monTool);
  }

  ATH_MSG_DEBUG("Initialization completed successfully");

  return StatusCode::SUCCESS;
}

bool TrigTauXComboHypoTool::executeAlg(std::vector<LegDecision> &combination) const {
  ATH_MSG_DEBUG("On combination executeAlg");
  auto dROfAccepted  = Monitored::Scalar( "dROfAccepted"   , -1.0 );
  auto dROfProcessed = Monitored::Scalar( "dROfProcessed"  , -1.0 );
  auto monitorIt    = Monitored::Group( m_monTool, dROfAccepted, dROfProcessed);

  //retrieve the elements
  std::vector<ElementLink<xAOD::IParticleContainer>> selected_particles;
  for (auto el: combination){
    auto EL= el.second;    
    auto particleLink = TrigCompositeUtils::findLink<xAOD::IParticleContainer>( *EL, featureString() ).link;
    ATH_MSG_DEBUG(  particleLink.dataID() );
    selected_particles.push_back(particleLink);
  }
  TLorentzVector hlv1 = (*selected_particles[0])->p4();
  TLorentzVector hlv2 = (*selected_particles[1])->p4();  
  auto dR = hlv1.DeltaR(hlv2);
  ATH_MSG_DEBUG("Found a combination with dR " <<dR);

  // apply the cut
  bool pass=true;
  dROfProcessed = dR;
  if (dROfProcessed < m_dRmin || dROfProcessed > m_dRmax){ 
    ATH_MSG_DEBUG("Combination failed dR cut: " << dROfProcessed << " not in [" << m_dRmin << "," <<  m_dRmax << "]");
    pass=false;
  }else{
    dROfAccepted = dR;
    ATH_MSG_DEBUG( " dR " << dROfAccepted << " is  within [" <<m_dRmin<< "," << m_dRmax << "] This selection passed! ");
  }
  
  return pass;

}




