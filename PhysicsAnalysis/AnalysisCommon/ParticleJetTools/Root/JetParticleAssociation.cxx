/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#include "ParticleJetTools/JetParticleAssociation.h"
#include "AsgDataHandles/WriteDecorHandle.h"

JetParticleAssociation::JetParticleAssociation(const std::string& name)
    : asg::AsgTool(name) {
}

StatusCode JetParticleAssociation::initialize() {

    m_decKey = m_jetContainerName + "." + m_decKey.key();

    ATH_CHECK(m_decKey.initialize());
    ATH_CHECK(m_particleKey.initialize());

    return StatusCode::SUCCESS;
}

StatusCode JetParticleAssociation::decorate(const xAOD::JetContainer& jets) const{
    SG::WriteDecorHandle<xAOD::JetContainer, std::vector<ElementLink<xAOD::IParticleContainer> > > decHandle(m_decKey);

    const std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > >* matches;

    SG::ReadHandle<xAOD::IParticleContainer> parts(m_particleKey);
    if( !parts.isValid() ) {
      ATH_MSG_WARNING ("Couldn't retrieve particles with key: " << m_particleKey.key() );
      return StatusCode::FAILURE;
    }

    matches = match(jets, *parts);

    ATH_MSG_DEBUG("About to decorate jets with" << m_decKey);

    for (unsigned int iJet = 0; iJet < jets.size(); iJet++)
        decHandle(*(jets.at(iJet))) = (*matches)[iJet];

    delete matches;

    return StatusCode::SUCCESS;
}