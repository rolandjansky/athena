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
      const xAOD::IParticle* ap=(*pItr);
      if(theJet->p4().DeltaR( ap->p4()) < m_DR) ParticleVector.push_back(ap);
    }
    theJet->setAssociatedObjects(m_assoc_name,ParticleVector);
  }
  return 1;
} 

