/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: jie.yu@cern.ch

#include "ParticleJetTools/JetTrackCenterOfMassAssociation.h"
#include "AsgTools/Check.h"
#include "TLorentzVector.h"

using namespace std;
using namespace xAOD;
using namespace Analysis;

JetTrackCenterOfMassAssociation::JetTrackCenterOfMassAssociation(const string& name)
    : JetTrackAssociation(name) {

      declareProperty("inputTrackCollectionName", m_inputTrackCollectionName);
      declareProperty("trackMatchCone", m_trackMatchCone = 0.8);
      declareProperty("parentJetCone", m_parentJetCone = 1.0);

        return;
    }


const std::vector<std::vector<const xAOD::TrackParticle*> >*
JetTrackCenterOfMassAssociation::match(xAOD::JetContainer& jets) const {

  // up limit of the delta angle in center-of-mass frame
  double trackAngleMax = getAngleSize( m_trackMatchCone );

  vector<vector<const xAOD::TrackParticle*> >* matchedtracks =
    new vector<vector<const xAOD::TrackParticle*> >(jets.size());

  const xAOD::TrackParticleContainer* tracks = NULL;
  if (evtStore()->retrieve( tracks, m_inputTrackCollectionName ).isFailure() )
    ATH_MSG_FATAL("JetTrackCenterOfMassAssociation: "
        "failed to retrieve track collection \"" +
        m_inputTrackCollectionName + "\"");

  for (xAOD::TrackParticleContainer::const_iterator track_itr = tracks->begin();
      track_itr != tracks->end(); ++track_itr) {

    const xAOD::TrackParticle& track = **track_itr;

    double deltaAngleMatch = 999.;
    int matchjetidx = -1;
    for (unsigned int iJet = 0; iJet < jets.size(); iJet++) {
      const xAOD::Jet& jet = *jets[iJet];

      bool foundEL = jet.isAvailable< ElementLink< xAOD::JetContainer > >("Parent");
      if ( ! foundEL ){
        ATH_MSG_WARNING("PARTICLE to JET center-of-mass Associator: PARENT jet not available.");
        continue;
      }
      ElementLink< xAOD::JetContainer > assoParentJet = jet.auxdata< ElementLink< xAOD::JetContainer > >("Parent");
      if ( ! assoParentJet.isValid()){
        ATH_MSG_WARNING("PARTICLE to JET center-of-mass Associator: Unable to get parent link Null ptr is returned.");
        continue;
      }
      const xAOD::Jet* parentJet = *(assoParentJet);
      double jetMass = parentJet->m();
      if (jetMass < 1000. ) jetMass = 1000.;

      TLorentzVector tlv_parentj(0, 0, 0, 0);
      tlv_parentj.SetPtEtaPhiM(parentJet->pt(), parentJet->eta(), parentJet->phi(), jetMass);
      TVector3 t3_boost = -1. * tlv_parentj.BoostVector();

      TLorentzVector tlv_subj(0, 0, 0, 0);
      tlv_subj.SetPtEtaPhiM(jet.pt(), jet.eta(), jet.phi(), jet.m());

      TLorentzVector tlv_track(0, 0, 0, 0);
      tlv_track.SetPtEtaPhiM(track.pt(), track.eta(), track.phi(), track.m());

      double dR = tlv_track.DeltaR(tlv_parentj);

      // the track is first matched to the parent jet by requiring dR < parent-jet-Cone
      if (dR < m_parentJetCone){
        tlv_track.Boost( t3_boost );
        tlv_subj.Boost( t3_boost );
        double angleSubTrk = tlv_subj.Angle(tlv_track.Vect());
        if ( angleSubTrk < deltaAngleMatch){
          deltaAngleMatch = angleSubTrk; 
          matchjetidx = iJet;
        }
      }
    }

    if (matchjetidx >= 0 && deltaAngleMatch < trackAngleMax)
      (*matchedtracks)[matchjetidx].push_back(&track);
  }

  return matchedtracks;
}
