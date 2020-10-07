/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#include "ParticleJetTools/JetParticleShrinkingConeAssociation.h"
#include "AsgMessaging/Check.h"

using namespace std;
using namespace xAOD;

JetParticleShrinkingConeAssociation::JetParticleShrinkingConeAssociation(const string& name)
    : JetParticleAssociation(name) {

        declareProperty("coneSizeFitPar1", m_coneSizeFitPar1=0);
        declareProperty("coneSizeFitPar2", m_coneSizeFitPar2=0);
        declareProperty("coneSizeFitPar3", m_coneSizeFitPar3=0);

        return;
    }


const vector<vector<ElementLink<IParticleContainer> > >*
JetParticleShrinkingConeAssociation::match(const xAOD::JetContainer& jets, const xAOD::IParticleContainer& parts) const {

    vector<vector<ElementLink<IParticleContainer> > >* matchedparts =
        new vector<vector<ElementLink<IParticleContainer> > >(jets.size());


    for (xAOD::IParticleContainer::const_iterator part_itr = parts.begin();
            part_itr != parts.end(); ++part_itr) {

        const xAOD::IParticle& part = **part_itr;

        double drmin = -1;
        int matchjetidx = -1;
        for (unsigned int iJet = 0; iJet < jets.size(); iJet++) {
            const xAOD::Jet& jet = *jets[iJet];

            double match_dr = coneSize(jet.pt());
            double dr = jet.p4().DeltaR(part.p4());

            if (dr > match_dr)
                continue;

            // if there is more than one matching jet, take the
            // closest
            if (drmin < 0 || dr < drmin) {
                drmin = dr;
                matchjetidx = iJet;
            }
        }


        if (matchjetidx >= 0) {
            ElementLink<IParticleContainer> EL; 
            EL.toContainedElement(parts, *part_itr);
            (*matchedparts)[matchjetidx].push_back(EL);
        }
    }


    return matchedparts;
}
