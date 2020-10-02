/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// author: jie.yu@cern.ch

#include "ParticleJetTools/JetParticleCenterOfMassAssociation.h"
#include "AsgMessaging/Check.h"
#include "TLorentzVector.h"

using namespace std;
using namespace xAOD;

JetParticleCenterOfMassAssociation::JetParticleCenterOfMassAssociation(const string& name)
    : JetParticleAssociation(name) {

        declareProperty("partMatchCone", m_partMatchCone = 0.8);
        declareProperty("parentJetCone", m_parentJetCone = 1.0);

        return;
    }


const vector<vector<ElementLink<IParticleContainer> > >*
JetParticleCenterOfMassAssociation::match(const JetContainer& jets, const xAOD::IParticleContainer& parts) const {

    // up limit of the delta angle in center-of-mass frame
    double partAngleMax = getAngleSize( m_partMatchCone );

    vector<vector<ElementLink<IParticleContainer> > >* matchedparts =
        new vector<vector<ElementLink<IParticleContainer> > >(jets.size());

    for (IParticleContainer::const_iterator part_itr = parts.begin();
            part_itr != parts.end(); ++part_itr) {

        const IParticle& part = **part_itr;

        double deltaAngleMatch = 999.;
        int matchjetidx = -1;
        for (unsigned int iJet = 0; iJet < jets.size(); iJet++) {
            const Jet& jet = *jets[iJet];

            bool foundEL = jet.isAvailable< ElementLink< JetContainer > >("Parent");
            if ( ! foundEL ){
                ATH_MSG_WARNING("PARTICLE to JET center-of-mass Associator: PARENT jet not available.");
                continue;
            }
            ElementLink< JetContainer > assoParentJet = jet.auxdata< ElementLink< JetContainer > >("Parent");
            if ( ! assoParentJet.isValid()){
                ATH_MSG_WARNING("PARTICLE to JET center-of-mass Associator: Unable to get parent link Null ptr is returned.");
                continue;
            }
            const Jet* parentJet = *(assoParentJet);
            double jetMass = parentJet->m();
            if (jetMass < 1000. ) jetMass = 1000.;

            TLorentzVector tlv_parentj(0, 0, 0, 0);
            tlv_parentj.SetPtEtaPhiM(parentJet->pt(), parentJet->eta(), parentJet->phi(), jetMass);
            TVector3 t3_boost = -1. * tlv_parentj.BoostVector();

            TLorentzVector tlv_subj(0, 0, 0, 0);
            tlv_subj.SetPtEtaPhiM(jet.pt(), jet.eta(), jet.phi(), jet.m());

            TLorentzVector tlv_part(0, 0, 0, 0);
            tlv_part.SetPtEtaPhiM(part.pt(), part.eta(), part.phi(), part.m());

            double dR = tlv_part.DeltaR(tlv_parentj);

            // the part is first matched to the parent jet by requiring dR < parent-jet-Cone
            if (dR < m_parentJetCone){
                tlv_part.Boost( t3_boost );
                tlv_subj.Boost( t3_boost );
                double angleSubTrk = tlv_subj.Angle(tlv_part.Vect());
                if ( angleSubTrk < deltaAngleMatch){
                    deltaAngleMatch = angleSubTrk; 
                    matchjetidx = iJet;
                }
            }
        }

        if (matchjetidx >= 0 && deltaAngleMatch < partAngleMax) {
            ElementLink<IParticleContainer> EL; 
            EL.toContainedElement(parts, *part_itr);
            (*matchedparts)[matchjetidx].push_back(EL);
        }
    }

    return matchedparts;
}
