/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#include "ParticleJetTools/JetParticleAssociation.h"

using namespace std;
using namespace xAOD;

JetParticleAssociation::JetParticleAssociation(const string& name)
    : AsgTool(name) {

        declareProperty("jetCollectionName", m_jetCollectionName);
        declareProperty("outputCollectionName", m_outputCollectionName);
        declareProperty("inputParticleCollectionName", m_inputParticleCollectionName);

        return;
    }

StatusCode JetParticleAssociation::initialize() {
    m_dec = new SG::AuxElement::Decorator<vector<ElementLink<IParticleContainer> > >(m_outputCollectionName);

    return StatusCode::SUCCESS;
}

StatusCode JetParticleAssociation::execute() {

    const JetContainer* jets = nullptr;
    if ( evtStore()->retrieve( jets, m_jetCollectionName ).isFailure() ) {
        ATH_MSG_FATAL("JetParticleAssociation: "
                "failed to retrieve jet collection \"" +
                m_jetCollectionName + "\"");
        return StatusCode::FAILURE;
    }

    const IParticleContainer* parts = nullptr;
    if ( evtStore()->retrieve( parts, m_inputParticleCollectionName ).isFailure() ) {
        ATH_MSG_FATAL("JetParticleAssociation: "
                "failed to retrieve particle collection \"" +
                m_inputParticleCollectionName + "\"");
        return StatusCode::FAILURE;
    }

    const vector<vector<ElementLink<IParticleContainer> > >* matches = match(*jets, *parts);


    SG::AuxElement::ConstAccessor<vector<ElementLink<TrackParticleContainer> > >
        trkacc("BTagTrackToJetAssociator");

    for (unsigned int iJet = 0; iJet < jets->size(); iJet++)
        (*m_dec)(*jets->at(iJet)) = (*matches)[iJet];

    delete matches;

    return StatusCode::SUCCESS;
}


StatusCode JetParticleAssociation::finalize() {
    delete m_dec;
    return StatusCode::SUCCESS;
}
