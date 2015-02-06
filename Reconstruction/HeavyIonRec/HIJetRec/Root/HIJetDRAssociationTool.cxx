/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetRec/HIJetDRAssociationTool.h"
#include <iomanip>

//**********************************************************************

HIJetDRAssociationTool::HIJetDRAssociationTool(const std::string& n)
   :  JetModifierBase(n)
{
  declareProperty("ContainerKey",m_container_key);
  declareProperty("AssociationName",m_assoc_name);
  declareProperty("DeltaR", m_DR = 0.8);
  declareProperty("ApplyFilter", m_apply_filter=false);
  declareProperty("FilterMinE", m_E_min = -999);
  declareProperty("FilterMinPt", m_pT_min = 0);


}

//**********************************************************************


int HIJetDRAssociationTool::modify(xAOD::JetContainer& jets) const
{

  const xAOD::IParticleContainer* ppars=0;
  if ( evtStore()->contains<xAOD::IParticleContainer>(m_container_key) ) 
  {
    ATH_MSG_DEBUG("Retrieving xAOD container " << m_container_key );
    ppars = evtStore()->retrieve<const xAOD::IParticleContainer>(m_container_key);
    if (!ppars)
    {
      ATH_MSG_ERROR("Failed to retrieve xAOD container " << m_container_key );
      return 0;
    }
  }
  else
  {
    ATH_MSG_ERROR("Failed to retrieve xAOD container " << m_container_key );
    return 0;
  }

  for (xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ijet++)
  {
    std::vector<const xAOD::IParticle*> ParticleVector;
    xAOD::Jet* theJet=(*ijet);
    for(xAOD::IParticleContainer::const_iterator pItr=ppars->begin(); pItr!=ppars->end(); pItr++)
    {
      xAOD::IParticle* ap=(*pItr);
      if(theJet->p4().DeltaR( ap->p4()) < m_DR) ParticleVector.push_back(ap);
    }
    theJet->setAssociatedObjects(m_assoc_name,ParticleVector);
  }
  return 1;
} 

  // msg(MSG::INFO) << std::setw(20) << "CHECKING ASSOCIATIONS" << endreq;
  // for (xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ijet++)
  // {
  //   xAOD::Jet* theJet=(*ijet);
  //   msg(MSG::INFO) << std::setw(20) << "JET"
  // 		   << std::setw(15) << theJet->e()
  // 		   << std::setw(15) << theJet->eta()
  // 		   << std::setw(15) << theJet->phi()
  // 		   << std::setw(15) << theJet->pt()
  // 		   << endreq;
    
  //   std::vector<const xAOD::IParticle*> theParticles=theJet->getAssociatedObjects<xAOD::IParticle>(m_assoc_name);
  //   unsigned int ip=0;
  //   for(auto pItr=theParticles.begin(); pItr!=theParticles.end(); pItr++, ip++)
  //   {
  //     msg(MSG::INFO) << std::setw(20) << ip
  // 		     << std::setw(15) << (*pItr)->e()
  // 		     << std::setw(15) << (*pItr)->eta()
  // 		     << std::setw(15) << (*pItr)->phi()
  // 		     << std::setw(15) << (*pItr)->pt()
  // 		     << endreq;
  //   }


