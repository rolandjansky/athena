/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// Andrei Gaponenko, 2008

#include "TrkTruthCreatorTools/DecayInFlyTruthTrajectoryBuilder.h"

#include "TrkTruthData/TruthTrajectory.h"

#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "AthContainers/DataVector.h"

#include <stack>

namespace Trk {

//================================================================
DecayInFlyTruthTrajectoryBuilder::
DecayInFlyTruthTrajectoryBuilder(const std::string& type,
			      const std::string& name,
			      const IInterface* parent)
  :  AthAlgTool(type,name,parent)
{
  declareInterface<Trk::ITruthTrajectoryBuilder>(this);
}


//================================================================
StatusCode DecayInFlyTruthTrajectoryBuilder::initialize() {
  return StatusCode::SUCCESS;
}

//================================================================
void DecayInFlyTruthTrajectoryBuilder::
buildTruthTrajectory(TruthTrajectory *result, HepMC::ConstGenParticlePtr input) const
{
  result->clear();
  if(input) {
    HepMC::ConstGenParticlePtr  next{nullptr};
    HepMC::ConstGenParticlePtr  current = input;

    // Extend trajectory outwards.  The last particle should go at [0]
    // in the TruthTrajectory, so we need to use a tmp storage while
    // traversing the structure.
    std::stack<HepMC::ConstGenParticlePtr> tmp;
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
DecayInFlyTruthTrajectoryBuilder::MotherDaughter
DecayInFlyTruthTrajectoryBuilder::truthTrajectoryCuts(HepMC::ConstGenVertexPtr vtx) const
{
  HepMC::ConstGenParticlePtr mother{nullptr};
  HepMC::ConstGenParticlePtr daughter{nullptr};
     // only truth vertices with 1 incoming particle     
      // Restrict to quasi-elastic processes (e.g. brems, delta-rays, pi->pi+Delta).
      // 
      // Require not more than 2 outgoing particles. Note that
      // delta-rays for primary==electron is a special case, because we have two
      // outgoing particles with the same PDG id.  The "correct" one
      // is that with the higher energy (NOT pt).
      // 
      // allow 1 outgoing to cover possible vertexes from interaction in detector material
#ifdef HEPMC3
  if(vtx && (vtx->particles_in().size() == 1) && (vtx->particles_out().size() <= 2)  ) {

    mother = vtx->particles_in().front();
#else 
  if(vtx && (vtx->particles_in_size() == 1) && (vtx->particles_out_size() <= 2) ) {

    mother = *vtx->particles_in_const_begin();
#endif    
    // Allow status code 1 and 2.  E.g. a pion that produced a long track can decay  outside of InDet and have status==2.
    if( mother && (mother->status() < 3) ) {

	int num_passed_cuts = 0;
	HepMC::ConstGenParticlePtr passed_cuts{nullptr};
	for(HepMC::ConstGenParticlePtr candidate: *vtx){
	  if(candidate->pdg_id() == mother->pdg_id()) {

	    if(passed_cuts && (mother->pdg_id() == 11)) { // second negative electron is a special case
	      if(candidate->momentum().e() > passed_cuts->momentum().e()) {
		passed_cuts = candidate; // don't increment num_passed_cuts, we are replacing chosen particle
	      }
	    }
	    else {
	      passed_cuts = candidate;
	      ++num_passed_cuts;
	    }
	  }
	  // allow pi/k->mu decay
	  else if(std::abs(candidate->pdg_id()) == 13){
	    passed_cuts = candidate;
	    ++num_passed_cuts;
	  }
	}

	if(num_passed_cuts==1) { // disallow hadronic pi->N*pi etc.
	  daughter = passed_cuts;
	}
    } // if( mother && (mother->status() == 1) )
  }
  
  return std::make_pair(mother, daughter);
}

//================================================================
HepMC::ConstGenParticlePtr DecayInFlyTruthTrajectoryBuilder::getDaughter(HepMC::ConstGenParticlePtr mother) const {

  HepMC::ConstGenParticlePtr daughter{nullptr};
  
  if(mother) {

    MotherDaughter res = truthTrajectoryCuts(mother->end_vertex());
    if(res.first == mother) {
      daughter = res.second;
    }

  }

  return daughter;
}

//================================================================
HepMC::ConstGenParticlePtr DecayInFlyTruthTrajectoryBuilder::getMother(HepMC::ConstGenParticlePtr daughter) const {

  HepMC::ConstGenParticlePtr mother{nullptr};

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
