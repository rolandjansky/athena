/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "HIJetDRAssociationTool.h"
#include <iomanip>

//**********************************************************************

HIJetDRAssociationTool::HIJetDRAssociationTool(const std::string& n)
   :  JetModifierBase(n)
{
}

//**********************************************************************

StatusCode HIJetDRAssociationTool::initialize()
{
   ATH_MSG_VERBOSE("HIJetDRAssociationTool initialize");
   ATH_CHECK( m_containerKey.initialize( !m_containerKey.key().empty() ) );
   return StatusCode::SUCCESS;
}

StatusCode HIJetDRAssociationTool::modify(xAOD::JetContainer& jets) const
{

  const xAOD::IParticleContainer* ppars=0;

    ATH_MSG_DEBUG("Retrieving xAOD container " << m_containerKey.key() );
    SG::ReadHandle<xAOD::IParticleContainer>  readHandlePcontainer ( m_containerKey );
    ppars = readHandlePcontainer.get();

  for (xAOD::JetContainer::iterator ijet=jets.begin(); ijet!=jets.end(); ijet++)
  {
    std::vector<const xAOD::IParticle*> ParticleVector;
    xAOD::Jet* theJet=(*ijet);
    for(xAOD::IParticleContainer::const_iterator pItr=ppars->begin(); pItr!=ppars->end(); pItr++)
    {
      const xAOD::IParticle* ap=(*pItr);
      if(theJet->p4().DeltaR( ap->p4()) < m_DR) ParticleVector.push_back(ap);
    }
    theJet->setAssociatedObjects(m_assocName,ParticleVector);
  }
  return StatusCode::SUCCESS;
}
