/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#include "ParticleJetTools/JetTrackAssociation.h"

using namespace std;
using namespace xAOD;
using namespace Analysis;

JetTrackAssociation::JetTrackAssociation(const string& name)
    : AsgTool(name) {

        declareProperty("outputCollectionName", m_outputCollectionName);

        return;
    }

int JetTrackAssociation::modify(JetContainer& jets) const {

    const vector<vector<const TrackParticle*> >* matches = match(jets);

    for (unsigned int iJet = 0; iJet < jets.size(); iJet++)
        jets[iJet]->setAssociatedObjects<TrackParticle>(m_outputCollectionName, (*matches)[iJet]);

    return 0;
}
