/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#include "ParticleJetTools/JetParticleAssociation.h"
#include "AsgDataHandles/WriteDecorHandle.h"
#include "AthContainers/ConstDataVector.h"

JetParticleAssociation::JetParticleAssociation(const std::string& name)
    : asg::AsgTool(name) {
}

StatusCode JetParticleAssociation::initialize() {

    ATH_MSG_DEBUG("Initializing JetParticleAssociator");
    m_decKey = m_jetContainerName + "." + m_decKey.key();

    ATH_CHECK(m_decKey.initialize());
    ATH_CHECK(m_particleKey.initialize());

    ATH_MSG_DEBUG("Minimum pt threshold: " << m_ptMinimum);
    if (m_ptMinimum > 0.0) {
      if (!m_passPtKey.key().empty()) {
        m_passPtKey = m_jetContainerName + "." + m_passPtKey.key();
        ATH_MSG_DEBUG("Pass Key Decorator: " << m_passPtKey);
      }
    }

    ATH_CHECK(m_passPtKey.initialize(!m_passPtKey.key().empty()));

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

    ConstDataVector<xAOD::JetContainer> viewJets(SG::VIEW_ELEMENTS);
    std::vector<unsigned int> skipped;
    for (unsigned int i = 0; i < jets.size(); i++) {
      const xAOD::Jet* jet = jets.at(i);
      if (jet->pt() > m_ptMinimum) {
        ATH_MSG_VERBOSE("Adding jet, pt = " << jet->pt());
        viewJets.push_back(jet);
      } else {
        ATH_MSG_VERBOSE("Skipping jet, pt = " << jet->pt());
        skipped.push_back(i);
      }
    }

    matches = match(*viewJets.asDataVector(), *parts);

    ATH_MSG_DEBUG("About to decorate jets with" << m_decKey);

    for (unsigned int iJet = 0; iJet < viewJets.size(); iJet++) {
      decHandle(*(viewJets.at(iJet))) = (*matches)[iJet];
    }
    for (unsigned int iJet: skipped) {
      decHandle(*jets.at(iJet)) = {};
    }

    if (!m_passPtKey.empty()) {
      SG::WriteDecorHandle<xAOD::JetContainer, char> passHandle(m_passPtKey);
      for (const xAOD::Jet* jet: viewJets) {
        passHandle(*jet) = 1;
      }
      for (unsigned int iJet: skipped) {
        passHandle(*jets.at(iJet)) = 0;
      }
    }

    delete matches;

    return StatusCode::SUCCESS;
}
