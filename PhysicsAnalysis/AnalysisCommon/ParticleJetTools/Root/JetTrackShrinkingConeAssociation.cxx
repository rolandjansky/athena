/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#include "ParticleJetTools/JetTrackShrinkingConeAssociation.h"
#include "AsgTools/Check.h"

using namespace std;
using namespace xAOD;
using namespace Analysis;

JetTrackShrinkingConeAssociation::JetTrackShrinkingConeAssociation(const string& name)
    : JetTrackAssociation(name) {

        declareProperty("inputTrackCollectionName", m_inputTrackCollectionName);
        declareProperty("coneSizeFitPar1", m_coneSizeFitPar1);
        declareProperty("coneSizeFitPar2", m_coneSizeFitPar2);
        declareProperty("coneSizeFitPar3", m_coneSizeFitPar3);

        return;
    }


const std::vector<std::vector<const xAOD::TrackParticle*> >*
JetTrackShrinkingConeAssociation::match(xAOD::JetContainer& jets) const {

    vector<vector<const xAOD::TrackParticle*> >* matchedtracks =
        new vector<vector<const xAOD::TrackParticle*> >(jets.size());

    const xAOD::TrackParticleContainer* tracks = NULL;
    if (evtStore()->retrieve( tracks, m_inputTrackCollectionName ).isFailure() )
        ATH_MSG_FATAL("JetTrackShrinkingConeAssociation: "
                      "failed to retrieve track collection \"" +
                      m_inputTrackCollectionName + "\"");

    for (xAOD::TrackParticleContainer::const_iterator track_itr = tracks->begin();
            track_itr != tracks->end(); ++track_itr) {

        const xAOD::TrackParticle& track = **track_itr;

        double maxpt = 0;
        int matchjetidx = -1;
        for (unsigned int iJet = 0; iJet < jets.size(); iJet++) {
            const xAOD::Jet& jet = *jets[iJet];
            double pt = jet.pt();

            double match_dr = coneSize(pt);
            double dr = jet.p4().DeltaR(track.p4());

            // TODO
            // what about largest cone size?
            if (dr > match_dr)
                continue;

            // TODO
            // closest jet or highest-pT jet if track matched to 2?
            if (pt > maxpt)
                matchjetidx = iJet;
        }

        if (matchjetidx >= 0)
            (*matchedtracks)[matchjetidx].push_back(&track);
    }

    return matchedtracks;
}
