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

        declareProperty("outputCollectionName", m_outputCollectionName);
        dec = new SG::AuxElement::Decorator<vector<ElementLink<IParticleContainer> > >(m_outputCollectionName);

        return;
    }

int JetParticleAssociation::modify(xAOD::JetContainer& jets) const {

    const vector<vector<ElementLink<IParticleContainer> > >* matches = match(jets);


    SG::AuxElement::ConstAccessor<vector<ElementLink<TrackParticleContainer> > >
        trkacc("BTagTrackToJetAssociator");

    for (unsigned int iJet = 0; iJet < jets.size(); iJet++)
        (*dec)(*jets.at(iJet)) = (*matches)[iJet];

    delete matches;

    return 0;
}


JetParticleAssociation::~JetParticleAssociation() {
    delete dec;
}
