/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko, 2008

#include "TrkTruthCreatorTools/ElasticTruthTrajectoryBuilder.h"

#include "TrkTruthData/TruthTrajectory.h"

#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "AthContainers/DataVector.h"

#include <stack>

namespace Trk {

//================================================================
ElasticTruthTrajectoryBuilder::
ElasticTruthTrajectoryBuilder(const std::string& type,
const std::string& name,
const IInterface* parent)
	:  AthAlgTool(type,name,parent)
{
	declareInterface<Trk::ITruthTrajectoryBuilder>(this);
}


//================================================================
StatusCode ElasticTruthTrajectoryBuilder::initialize() {
	return StatusCode::SUCCESS;
}

//================================================================
void ElasticTruthTrajectoryBuilder::
buildTruthTrajectory(TruthTrajectory *result, const HepMC::GenParticle *input) const
{
	result->clear();
	if(input) {
		const HepMC::GenParticle *next(nullptr);
		const HepMC::GenParticle *current = input;

		// Extend trajectory outwards.  The last particle should go at [0]
		// in the TruthTrajectory, so we need to use a tmp storage while
		// traversing the structure.
		std::stack<const HepMC::GenParticle*> tmp;
		while( (next = getDaughter(current)) ) {
			tmp.push(current = next);
		}

		// copy the outer half to result
		while(!tmp.empty()) {
			result->push_back(tmp.top());
			tmp.pop();
		}

		// The input particle itself
		result->push_back(input);

		// Now continue towards the interaction point
		while( (next = getMother(current)) ) {
			result->push_back(current = next);
		}
	}
}

//================================================================
ElasticTruthTrajectoryBuilder::MotherDaughter
	ElasticTruthTrajectoryBuilder::truthTrajectoryCuts(const HepMC::GenVertex *vtx) const
{
	const HepMC::GenParticle *mother(nullptr);
	const HepMC::GenParticle *daughter(nullptr);

	// only truth vertices with 1 incoming particle
	if(vtx && (vtx->particles_in_size() == 1)) {

		mother = *vtx->particles_in_const_begin();
		// Allow status code 1 and 2.  E.g. a pion that produced a long track can decay  outside of InDet and have status==2.
		if( mother && (mother->status() < 3) ) {

			// Restrict to quasi-elastic processes (e.g. brems, delta-rays, pi->pi+Delta).
			// 
			// Require not more than 2 outgoing particles. Note that
			// delta-rays for primary==electron is a special case, because we have two
			// outgoing particles with the same PDG id.  The "correct" one
			// is that with the higher energy (NOT pt).
			// 
			// allow 1 outgoing to cover possible vertexes from interaction in detector material
			if (vtx->particles_out_size() <= 2) {

				int num_passed_cuts = 0;
				const HepMC::GenParticle *passed_cuts(nullptr);
				for(HepMC::GenVertex::particles_in_const_iterator it = vtx->particles_out_const_begin();
				it != vtx->particles_out_const_end(); ++it) {

					const HepMC::GenParticle *candidate = *it;
					if(candidate->pdg_id() == mother->pdg_id()) {

						if(passed_cuts && (mother->pdg_id() == 11)) { // second negative electron is a special case
							if(candidate->momentum().e() > passed_cuts->momentum().e()) {
								passed_cuts = candidate; // don't increment num_passed_cuts, we are replacing chosen particle
							}
						} else {
							passed_cuts = candidate;
							++num_passed_cuts;
						}
					}
				}

				if(num_passed_cuts==1) { // disallow hadronic pi->N*pi etc.
					daughter = passed_cuts;
				}

			} // if (vtx->particles_out_size() <= 2)
		} // if( mother && (mother->status() == 1) )
	}

	return std::make_pair(mother, daughter);
}

//================================================================
const HepMC::GenParticle* ElasticTruthTrajectoryBuilder::getDaughter(const HepMC::GenParticle* mother) const {

	const HepMC::GenParticle *daughter = nullptr;

	if(mother) {

		MotherDaughter res = truthTrajectoryCuts(mother->end_vertex());
		if(res.first == mother) {
			daughter = res.second;
		}

	}

	return daughter;
}

//================================================================
const HepMC::GenParticle* ElasticTruthTrajectoryBuilder::getMother(const HepMC::GenParticle* daughter) const {

	const HepMC::GenParticle *mother = nullptr;

	if(daughter) {

		MotherDaughter res = truthTrajectoryCuts(daughter->production_vertex());
		if(res.second == daughter) {
			mother = res.first;
		}

	}

	return mother;
}

	//================================================================

} // namespace Trk
