/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// JetWidthTool.cxx

#include "JetMomentTools/JetWidthTool.h"
#include "AsgDataHandles/WriteDecorHandle.h"
#include "xAODJet/JetConstituentVector.h"
#include "JetUtils/JetDistances.h"
#include "PFlowUtils/IWeightPFOTool.h"

//**********************************************************************

JetWidthTool::JetWidthTool(std::string myname)
  : asg::AsgTool(myname)
{
}

//**********************************************************************

StatusCode JetWidthTool::initialize(){

  if(m_jetContainerName.empty()){
    ATH_MSG_ERROR("JetWidthTool needs to have its input jet container name configured!");
    return StatusCode::FAILURE;
  }

  // Prepend jet container name
  m_widthKey = m_jetContainerName + "." + m_widthKey.key();
  m_widthPhiKey = m_jetContainerName + "." + m_widthPhiKey.key();

  ATH_CHECK(m_widthKey.initialize());
  ATH_CHECK(m_widthPhiKey.initialize());
  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode JetWidthTool::decorate(const xAOD::JetContainer& jets) const {

  SG::WriteDecorHandle<xAOD::JetContainer, float> widthHandle(m_widthKey);
  SG::WriteDecorHandle<xAOD::JetContainer, float> widthPhiHandle(m_widthPhiKey);

  for(const xAOD::Jet* jet : jets){
    float widthEta = 0, widthPhi = 0;
    widthHandle(*jet) = width(*jet,widthEta,widthPhi);
    widthPhiHandle(*jet) = widthPhi;
  }
  return StatusCode::SUCCESS;
}

//**********************************************************************

float JetWidthTool::width(const xAOD::Jet& jet, float& widthEta, float& widthPhi) const {

  // Get the constituents of the jet
  // TODO: Switch to using helper function once JetUtils has been updated
  // Set the width
  // jetWidth = JetKinematics::ptWeightedWidth(iter,iEnd,&jet);
    
  // Calculate the pt weighted width
  const float jetEta = jet.eta();
  const float jetPhi = jet.phi();
  float weightedWidth = 0;
  float weightedWidthEta = 0;
  float weightedWidthPhi = 0;
  float ptSum = 0;

  const xAOD::JetConstituentVector constituents = jet.getConstituents();
  for (const auto& constituent : constituents) {
    const float dR   = jet::JetDistances::deltaR(jetEta, jetPhi, constituent->eta(),  constituent->phi() );
    const float dEta = fabs(jet::JetDistances::deltaEta(jetEta, constituent->eta() ));
    const float dPhi = fabs(jet::JetDistances::deltaPhi(jetPhi, constituent->phi() ));
    const float pt   = constituent->pt();

    weightedWidth += dR * pt;
    weightedWidthEta += dEta * pt;
    weightedWidthPhi += dPhi * pt;
    
    ptSum += pt;
  }

  widthEta = ptSum > 0 ? weightedWidthEta/ptSum : -1;
  widthPhi = ptSum > 0 ? weightedWidthPhi/ptSum : -1;

  return ptSum > 0 ? weightedWidth/ptSum : -1;
}
