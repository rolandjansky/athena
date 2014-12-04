/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ======================================================================
// TracksInCone
// James.Catmore@cern.ch
// Given an xAOD::IParticle and an IDTrackParticle container, checks 
// which tracks are in a cone of a given size around the IParticle. Sets
// a mask accordingly. Intended for thinning use.
// ======================================================================
#pragma once

#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
namespace DerivationFramework {
	struct TracksInCone{
		
		void select(const xAOD::IParticle* particle, float coneSize, const xAOD::TrackParticleContainer* tracks, std::vector<bool> &mask) {
			float particleEta = particle->eta();
			float particlePhi = particle->phi();
			unsigned int i(0);
			for (xAOD::TrackParticleContainer::const_iterator trIt = tracks->begin(); trIt!=tracks->end(); ++trIt, ++i) {
				float deltaEta = (*trIt)->eta() - particleEta;
				float deltaPhi = fabs((*trIt)->phi() - particlePhi);
				if (deltaPhi > TMath::Pi()) deltaPhi = 2.0*TMath::Pi() - deltaPhi;
				float deltaR = sqrt(deltaEta*deltaEta + deltaPhi*deltaPhi);
				if (deltaR < coneSize) mask[i] = true;
			}
			return;
		}
	};
}
