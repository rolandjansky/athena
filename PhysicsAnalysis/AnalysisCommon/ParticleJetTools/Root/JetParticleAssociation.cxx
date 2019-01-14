/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#include "ParticleJetTools/JetParticleAssociation.h"

using namespace std;
using namespace xAOD;

JetParticleAssociation::JetParticleAssociation(const string& name)
    : AsgTool(name) {

        declareProperty("jetCollectionName", m_jetCollectionName);
        declareProperty("outputCollectionName", m_outputCollectionName);

        return;
    }

StatusCode JetParticleAssociation::initialize() {
    dec = new SG::AuxElement::Decorator<vector<ElementLink<IParticleContainer> > >(m_outputCollectionName);

    return StatusCode::SUCCESS;
}

StatusCode JetParticleAssociation::execute() {

    const JetContainer* jets = NULL;
    if ( evtStore()->retrieve( jets, m_jetCollectionName ).isFailure() ) {
        ATH_MSG_FATAL("JetParticleAssociation: "
                "failed to retrieve jet collection \"" +
                m_jetCollectionName + "\"");
        return StatusCode::FAILURE;
    }

    const vector<vector<ElementLink<IParticleContainer> > >* matches = match(*jets);

#ifndef GENERATIONBASE
    SG::AuxElement::ConstAccessor<vector<ElementLink<TrackParticleContainer> > >
        trkacc("BTagTrackToJetAssociator");

    for (unsigned int iJet = 0; iJet < jets->size(); iJet++)
        (*dec)(*jets->at(iJet)) = (*matches)[iJet];

    delete matches;
#endif //if not GENERATIONBASE

    return StatusCode::SUCCESS;
}


StatusCode JetParticleAssociation::finalize() {
    delete dec;
    return StatusCode::SUCCESS;
}


