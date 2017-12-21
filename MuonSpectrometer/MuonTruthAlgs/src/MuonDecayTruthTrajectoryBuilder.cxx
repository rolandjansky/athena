/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Niels van Eldik 2010

#include "MuonDecayTruthTrajectoryBuilder.h"

#include "TrkTruthData/TruthTrajectory.h"

#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

#include "GeneratorObjects/HepMcParticleLink.h"
#include "AthContainers/DataVector.h"

#include <stack>

namespace Muon {

  //================================================================
  MuonDecayTruthTrajectoryBuilder::
  MuonDecayTruthTrajectoryBuilder(const std::string& type,
				  const std::string& name,
				  const IInterface* parent)
    :  AthAlgTool(type,name,parent)
       // ,m_isDecayIntoTwoMuons(false)
  {
    declareInterface<Trk::ITruthTrajectoryBuilder>(this);
  }


  //================================================================
  StatusCode MuonDecayTruthTrajectoryBuilder::initialize() {
    return StatusCode::SUCCESS;
  }

  //================================================================
  void MuonDecayTruthTrajectoryBuilder::
  buildTruthTrajectory(TruthTrajectory *result, const HepMC::GenParticle *input) const
  {
    result->clear();
    if(input) {
      const HepMC::GenParticle *next(0), *current = input;
      ATH_MSG_DEBUG( " New TruthTrajectory: input: barcode " << input->barcode() << " PDG id " << input->pdg_id());

      // Extend trajectory outwards.  The last particle should go at [0]
      // in the TruthTrajectory, so we need to use a tmp storage while
      // traversing the structure.
      std::stack<const HepMC::GenParticle*> tmp;
      while((next = getDaughter(current))) {
	tmp.push(current = next);
      }

      // copy the outer half to result
      while(!tmp.empty()) {
	ATH_MSG_DEBUG( " Adding daughter: barcode " << current->barcode() << " PDG id " << current->pdg_id());
	result->push_back(tmp.top());
	tmp.pop();
      }

      // The input particle itself
      result->push_back(input);

      // Now continue towards the interaction point
      while((next = getMother(current))) {
	ATH_MSG_DEBUG( " Adding mother: barcode " << current->barcode() << " PDG id " << current->pdg_id());
	result->push_back(current = next);
      }
    
      ATH_MSG_DEBUG( " Final TruthTrajectory: ");
      std::vector<HepMcParticleLink>::const_iterator pit = result->begin();
      std::vector<HepMcParticleLink>::const_iterator pit_end = result->end();
      for( ;pit!=pit_end;++pit ){
	const HepMC::GenParticle& par = *pit->cptr(); 
	if(msgLvl(MSG::DEBUG))  {
	  msg(MSG::DEBUG) << " PDG ID " << par.pdg_id() << " barcode: " << par.barcode() << " pt: " << par.momentum().perp();
	  if( par.production_vertex() ) msg(MSG::DEBUG) << " vertices prod: r " << par.production_vertex()->point3d().perp() 
							<< " z " << par.production_vertex()->point3d().z();
	  if( par.end_vertex() ) msg(MSG::DEBUG) << " end: r " << par.end_vertex()->point3d().perp() << " z " << par.end_vertex()->point3d().z();
	  msg(MSG::DEBUG) << endmsg;
	}
      }
    }
  }

  //================================================================
  MuonDecayTruthTrajectoryBuilder::MotherDaughter
  MuonDecayTruthTrajectoryBuilder::truthTrajectoryCuts(const HepMC::GenVertex *vtx) const
  {
    const HepMC::GenParticle *mother(0), *daughter(0);

    if( vtx && msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " new vertex: nparticles in " << vtx->particles_in_size() << endmsg;
    // only truth vertices with 1 incoming particle
    if(vtx && (vtx->particles_in_size() == 1)) {

      mother = *vtx->particles_in_const_begin();
    
      if( mother && msgLvl(MSG::DEBUG) ) msg(MSG::DEBUG) << " new mother: " << mother->pdg_id() << " status " << mother->status() << " particles out " << vtx->particles_out_size() << endmsg;
      // Allow status code 1 and 2.  E.g. a pion that produced a long track can decay  outside of InDet and have status==2.
      if( mother && (mother->status() < 3) ) {
	unsigned int nDecayMuons = 0;
	const HepMC::GenParticle *passed_cuts(0);
	for(HepMC::GenVertex::particles_in_const_iterator it = vtx->particles_out_const_begin();
	    it != vtx->particles_out_const_end(); ++it) {
      
	  const HepMC::GenParticle *candidate = *it;
	  const HepMC::GenParticle& par = *candidate;
        
	  if( msgLvl(MSG::DEBUG) ){
	    msg(MSG::DEBUG) << " PDG ID " << par.pdg_id() << " barcode: " << par.barcode() <<  " pt: " << par.momentum().perp(); 
	    if( par.production_vertex() ) msg(MSG::DEBUG) << " vertices prod: r " << par.production_vertex()->point3d().perp() 
							  << " z " << par.production_vertex()->point3d().z();
	    if( par.end_vertex() ) msg(MSG::DEBUG) << " end: r " << par.end_vertex()->point3d().perp() << " z " << par.end_vertex()->point3d().z();
	    msg(MSG::DEBUG) << endmsg;
	  }

	  if(candidate->pdg_id() == mother->pdg_id()) {

	    if(passed_cuts && (mother->pdg_id() == 11)) { // second negative electron is a special case
	      if(candidate->momentum().e() > passed_cuts->momentum().e()) {
		passed_cuts = candidate;
	      }
	    }
	    else {
	      passed_cuts = candidate;
	    }
	  }else if( abs(candidate->pdg_id()) == 13 ){
	    ATH_MSG_DEBUG( " selecting Decay into muon ");
	    ++nDecayMuons;
	    passed_cuts = candidate;
	  } else { // temp addition for debugging
            msg(MSG::DEBUG) << " Neither muon nor identical pdgId " << endmsg;
            passed_cuts = candidate;
          }
	}
      
	if( nDecayMuons > 0 ){
	    daughter = passed_cuts;
	  if( nDecayMuons == 2 ){
	    ATH_MSG_DEBUG( " decay into two muons ");
	    // m_isDecayIntoTwoMuons = true;
	  }
	}
      } // if( mother && (mother->status() == 1) )
    }
  
    return std::make_pair(mother, daughter);
  }

  //================================================================
  const HepMC::GenParticle* MuonDecayTruthTrajectoryBuilder::getDaughter(const HepMC::GenParticle* mother) const {

    const HepMC::GenParticle *daughter = 0;
  
    if(mother) {

      MotherDaughter res = truthTrajectoryCuts(mother->end_vertex());
      if(res.first == mother) {
	daughter = res.second;
      }

    }

    return daughter;
  }

  //================================================================
  const HepMC::GenParticle* MuonDecayTruthTrajectoryBuilder::getMother(const HepMC::GenParticle* daughter) const {

    const HepMC::GenParticle *mother = 0;

    if(daughter) {
      MotherDaughter res = truthTrajectoryCuts(daughter->production_vertex());
      mother = res.first;
      // m_isDecayIntoTwoMuons = false; // Don't think this does anything? EJWM.
    }

    return mother;
  }

  //================================================================

} // namespace Trk
