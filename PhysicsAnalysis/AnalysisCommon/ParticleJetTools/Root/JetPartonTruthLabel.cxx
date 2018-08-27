/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ParticleJetTools/JetPartonTruthLabel.h"
#include "xAODTruth/TruthParticle.h"

using namespace std;
using namespace Analysis;

int JetPartonTruthLabel::modifyJet(xAOD::Jet &jet) const {
  ATH_MSG_VERBOSE("In " << name() << "::modifyJet()");

	int label = -1;

	vector<const xAOD::TruthParticle*> partons;
	bool success = jet.getAssociatedObjects("GhostPartons", partons);
	if(success) {
		double e_max = 0;
		for(auto it = partons.begin(); it != partons.end(); it++) {
			if((*it)->absPdgId() == 6) continue; // Skip top

			if((*it)->e() > e_max) {
				e_max = (*it)->e();
				label = (*it)->absPdgId();
			}
		}
	}

  jet.setAttribute("PartonTruthLabelID", label);

  return 0;
}
