/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#include "ParticleJetTools/JetParticleFixedConeAssociation.h"
#include "AsgTools/Check.h"

using namespace std;
using namespace xAOD;

JetParticleFixedConeAssociation::JetParticleFixedConeAssociation(const string& name)
    : JetParticleAssociation(name) {

        declareProperty("InputParticleCollectionName", m_InputParticleCollectionName);
        declareProperty("ConeSize", m_ConeSize=0);

        return;
    }


// this algorithm associates particles jets using a pT-dependent
// shrinking cone:
// - for each particle we find the closest jet
// - if the closest jet is within the dR cone of that jet,
//   it is considered "associated"
const vector<vector<ElementLink<IParticleContainer> > >*
JetParticleFixedConeAssociation::match(const xAOD::JetContainer& jets) const {

    const xAOD::IParticleContainer* parts = NULL;
    if (evtStore()->retrieve( parts, m_InputParticleCollectionName ).isFailure() )
        ATH_MSG_FATAL("JetParticleFixedConeAssociation: "
                      "failed to retrieve part collection \"" +
                      m_InputParticleCollectionName + "\"");


    vector<vector<ElementLink<IParticleContainer> > >* matchedparts =
        new vector<vector<ElementLink<IParticleContainer> > >(jets.size());


    for (xAOD::IParticleContainer::const_iterator part_itr = parts->begin();
            part_itr != parts->end(); ++part_itr) {

        const xAOD::IParticle& part = **part_itr;

        double drmin = -1;
        int matchjetidx = -1;
        for (unsigned int iJet = 0; iJet < jets.size(); iJet++) {
            const xAOD::Jet& jet = *jets[iJet];

            double dr = jet.p4().DeltaR(part.p4());

            // if there is more than one matching jet, take the
            // closest
            if (drmin < 0 || dr < drmin) {
                drmin = dr;
                matchjetidx = iJet;
            }
        }


        if (matchjetidx >= 0) {
            if (drmin > m_ConeSize) continue;

            ElementLink<IParticleContainer> EL; 
            EL.toContainedElement(*parts, *part_itr);
            (*matchedparts)[matchjetidx].push_back(EL);
        }
    }


    return matchedparts;
}
