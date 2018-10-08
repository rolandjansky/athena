/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
   */

// author: cpollard@cern.ch

#include "ParticleJetTools/JetParticleAssociation.h"
#include "AsgTools/AsgTool.h"

using namespace std;
using namespace xAOD;

JetParticleAssociation::JetParticleAssociation(const string& name)
  : AsgTool(name) {

    declareProperty("OutputCollectionName", m_OutputCollectionName);

    return;
  }

StatusCode JetParticleAssociation::initialize() {
  return StatusCode::SUCCESS;
}

StatusCode JetParticleAssociation::finalize() {
  return StatusCode::SUCCESS;
}

int JetParticleAssociation::modify(xAOD::JetContainer& jets) const {


  typedef SG::AuxElement::Decorator<vector<ElementLink<IParticleContainer> > > VecIPart;

  // TODO
  // should this be local?
  const VecIPart dec
    = SG::AuxElement::Decorator<vector<ElementLink<IParticleContainer> > >(m_OutputCollectionName);

  const vector<vector<ElementLink<IParticleContainer> > >* matches = match(jets);

  for (unsigned int iJet = 0; iJet < jets.size(); iJet++)
    dec(*jets.at(iJet)) = (*matches)[iJet];

  delete matches;

  return 0;
}
