/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: TruthTools.cxx
// Description:
// Author: Fabian Wilk
// Created: Wed Feb  4 11:05:43 2015

#include "TopParticleLevel/TruthTools.h"

#include "xAODTruth/TruthVertex.h"

#include "TruthUtils/PIDHelpers.h"

#include <list>
#include <string>
#include <sstream>
#include <functional>
namespace top {
    namespace truth {

	void printDecayChain( const xAOD::TruthParticle * truthPart,
				 std::ostream & os /* = std::cout */,
				 const std::string & prefix /* = "" */ ){
	    // Protect against a faulty production vertex reference.
	    if ( not truthPart->prodVtx() ){
		os << prefix << "[" << truthPart->pdgId() << " <- NULL]" << '\n';
		return;
	    }

	    const xAOD::TruthParticle * parent{ truthPart->prodVtx()->incomingParticle( 0 ) };
	    os << prefix << "[" << truthPart->pdgId();
	    while ( parent ){
		os << " <- " << parent->pdgId();
		if ( parent->prodVtx() ){
		    parent = parent->prodVtx()->incomingParticle( 0 );
		}
		else {
		    break;
		}
	    }
	    os << "]" << '\n';
	}


	bool isFrom( const xAOD::TruthParticle * truthParticle,
		     const std::vector<int> & parentPDGIds,
		     bool bOnlyDirect /* = false */ ){
	    // If the input does not have aproper production vertex reference or
	    // there are no incoming particle to the production vertex, directly
	    // return false
	    if ( ! truthParticle->hasProdVtx() ||
		 ! truthParticle->prodVtx() ||
		 truthParticle->prodVtx()->nIncomingParticles() == 0 ) {
		return false;
	    }

	    // The current production vertex
	    const xAOD::TruthVertex * prodVtx = truthParticle->prodVtx();
	    // The previous production vertex (initialised to the current
	    // production vertex)
	    const xAOD::TruthVertex * prevProdVtx = prodVtx;

	    auto numIncoming = prodVtx->nIncomingParticles();
	    for ( std::size_t motherIndex = 0;
		  motherIndex < numIncoming;
		  ++motherIndex ){

		const xAOD::TruthParticle * mother= nullptr;
		int motherPdgId = truthParticle->pdgId();

		// Ascend through the decay chain until we find the "actual"
		// decay, i.e. a change in the PDG ID. This skips all the
		// non-decay truth table entries which come from "particle
		// evolution" rather than physical decay
		while ( truthParticle->pdgId() == motherPdgId ) {
		    mother = prodVtx->incomingParticle( motherIndex );
		    if ( mother ) {
			motherPdgId = mother->pdgId();
		    }
		    else { break; }

		    if ( truthParticle->pdgId() != motherPdgId ) { break; }

		    // Include protection against cyclic or broken reference
		    // chains which can occur in SHERPA samples
		    if( ! mother->hasProdVtx() ) { break; }
		    if ( prevProdVtx == mother->prodVtx() )  { break; }

		    // Update the previous / current production vertex references
		    prevProdVtx = prodVtx;
		    prodVtx = mother->prodVtx();

		    // safeguard
		    if ( ! prodVtx || prodVtx->nIncomingParticles() == 0 ) { break; }
		}

		// Check that the mother pointer is valid. If it is not valid, then
		// the particle could not possibly come from any of the requested
		// PDG Ids, hence return false
		if ( ! mother ){ return false; }

		// If the mother PDG ID is in the parentPDGIds collection, then return true
		if ( std::find( parentPDGIds.begin(), parentPDGIds.end(), motherPdgId ) != parentPDGIds.end()  ){
		    return true;
		}

		// If we allow chained matching (I.e. not only _direct_ decays) and
		// the mother particle does come from the request PDG ID(s), return true
		if ( ! bOnlyDirect && isFrom( mother, parentPDGIds, bOnlyDirect ) ){
		    return true;
		}
	    }

	    // If we did't find an ancestor with the requested pdg id, return false
	    return false;
	}

	bool isFromWZDecay( const xAOD::TruthParticle * truthParticle,
			    bool bOnlyDirect /* = false */ ) {
	    // If the input does not have aproper production vertex reference or
	    // there are no incoming particle to the production vertex, directly
	    // return false
	    if ( ! truthParticle->hasProdVtx() ||
		 ! truthParticle->prodVtx() ||
		 truthParticle->prodVtx()->nIncomingParticles() == 0 ) {
		return false;
	    }

	    // The current production vertex
	    const xAOD::TruthVertex * prodVtx = truthParticle->prodVtx();
	    // The previous production vertex (initialised to the current
	    // production vertex)
	    const xAOD::TruthVertex * prevProdVtx = prodVtx;


	    auto numIncoming = prodVtx->nIncomingParticles();
	    for ( std::size_t motherIndex = 0;
		  motherIndex < numIncoming;
		  ++motherIndex ){

		const xAOD::TruthParticle * mother = nullptr;
		int motherAbsPdgId = truthParticle->absPdgId();

		// Ascend through the decay chain until we find the "actual"
		// decay, i.e. a change in the PDG ID. This skips all the
		// non-decay truth table entries which come from "particle
		// evolution" rather than physical decay
		while ( truthParticle->absPdgId() == motherAbsPdgId ) {
		    mother = prodVtx->incomingParticle( motherIndex );
		    if ( mother ) {
			motherAbsPdgId = mother->absPdgId();
		    }
		    else { break; }

		    if ( truthParticle->absPdgId() != motherAbsPdgId ) { break; }

		    // Include protection against cyclic or broken reference
		    // chains which can occur in SHERPA samples
		    if( ! mother->hasProdVtx() ) { break; }
		    if ( prevProdVtx == mother->prodVtx() )  { break; }

		    // Update the previous / current production vertex references
		    prevProdVtx = prodVtx;
		    prodVtx = mother->prodVtx();

		    // safeguard
		    if ( ! prodVtx || prodVtx->nIncomingParticles() == 0 ) { break; }
		}

		// Check that the mother pointer is valid. If it is not valid, then
		// the particle could not possibly come from any of the requested
		// PDG Ids, hence return false
		if ( ! mother ){ return false; }

		// If the direct physical decay in the MC record was from a Z or W
		// boson to the truth particle then return true
		// Otherwise perform a vertex-based identification of W/Z's
		if ( motherAbsPdgId == 23 || motherAbsPdgId == 24 ){
		    return true;
		}

		// The following vertex-based identification of W/Z's is needed
		// for SHERPA samples where the W/Z particle is not explicitly
		// in the particle record. At this point if we have a valid
		// vertex, it should be a true decay vertex.
		//
		// If it is a W or Z then two of those decay products should be
		// lepton / neutrino pair corresponding to the transitions
		//    W+ -> l+ nu
		//    W- -> l- nu~
		//    Z  -> l+ l-
		//    Z  -> nu nu~
		// Hence loop through the outgoing particles of the truth vertex
		// and check that the vertex matches the requirement. We
		// simplify this check by just counting the number of leptonic
		// outgoing particles which should be equal to 2. It is assumed
		// that the MC generator does not produce unphysical decays.
		//
		// Furthermore, prompt W/Z's should come from a vertex with more
		// than one incoming particle. Consequently, test this
		// requirement prior to checking the outgoing states.
		int nDecay = 0;
		if ( prodVtx && prodVtx->nIncomingParticles() > 1 ) {
		    for ( const auto & child : prodVtx->outgoingParticleLinks() ){
			if ( ( * child )->absPdgId() > 10 &&
			     ( * child )->absPdgId() < 17 ) {
			    nDecay++;
			}
		    }
		}

		// There must be exactly two leptonic outgoing particles in that vertex
		if ( nDecay == 2 ){
		    return true;
		}

		// If we allow chained matching (I.e. not only _direct_ decays) and
		// the mother particle does come from a W / Z, return true
		if ( ! bOnlyDirect && isFromWZDecay( mother, bOnlyDirect ) ){
		    return true;
		}
	    }

	    // We did not find any W / Z in the ancestry of the input particle ->
	    // return false
	    return false;
	}

	bool isLeptonFromTau( const xAOD::TruthParticle * truthParticle ){
	    // If the input particle is not a lepton directly terminate the
	    // algorithm retunring false
	    if ( truthParticle->absPdgId() != 11 &&
		 truthParticle->absPdgId() != 12 &&
		 truthParticle->absPdgId() != 13 &&
		 truthParticle->absPdgId() != 14 &&
		 truthParticle->absPdgId() != 15 &&
		 truthParticle->absPdgId() != 16 ) {
		return false;
	    }
	    return isFrom( truthParticle, { 15, -15 }, true );
	}

	bool isNotFromHadron( const xAOD::TruthParticle * truthParticle,
			      bool bOnlyDirect /* = false */,
			      bool bTauIsHadron /* = false */ ){
	    // If there is no production vertex, either something is wrong, or
	    // we are at the beginning of the record. However, either way, the
	    // particle does not come from a hadron, hence return true.
	    if ( ! truthParticle->hasProdVtx() ||
		 ! truthParticle->prodVtx() ||
		 truthParticle->prodVtx()->nIncomingParticles() == 0 ) {
		return true;
	    }

	    // The current production vertex
	    const xAOD::TruthVertex * prodVtx = truthParticle->prodVtx();
	    // The previous production vertex (initialised to the current
	    // production vertex)
	    const xAOD::TruthVertex * prevProdVtx = prodVtx;

	    // Loop all the particles going into the production vertex of the
	    // current truth particle.
	    //   * If the mother of any of the is a physical hadron return false
	    //   * If the mother of any of them descends from a physical return false
	    //   * Otherwise return true
	    auto numIncoming = prodVtx->nIncomingParticles();
	    for ( std::size_t motherIndex = 0;
		  motherIndex < numIncoming;
		  ++motherIndex ){

		int motherPdgId = truthParticle->pdgId();
		const xAOD::TruthParticle * mother = nullptr;

		// Ascend through the decay chain until we find the "actual"
		// decay, i.e. a change in the PDG ID. This skips all the
		// non-decay truth table entries which come from "particle
		// evolution" rather than physical decay
		while ( truthParticle->pdgId() == motherPdgId ) {
		    mother = prodVtx->incomingParticle( motherIndex );
		    if ( mother ) {
			motherPdgId = mother->pdgId();
		    }
		    else { break; }

		    // Include protection against cyclic or broken reference
		    // chains which can occur in SHERPA samples
		    if( ! mother->hasProdVtx() ) { break; }
		    if ( prevProdVtx == mother->prodVtx() )  { break; }

		    // Update the previous / current production vertex references
		    prevProdVtx = prodVtx;
		    prodVtx = mother->prodVtx();

		    // safeguard
		    if ( ! prodVtx || prodVtx->nIncomingParticles() == 0 ) { break; }
		}

		// Check that the mother pointer is valid. If it is not valid, then
		// the particle could not possibly come from a hadron, hence return
		// true
		if ( ! mother ){ return true; }

		// If the mother particle is a "physical decayed" particle (HepMC
		//   status code) and a hadron, then return false.
		if ( mother->status() == 2 &&
		     ( MC::PID::isHadron( motherPdgId ) ||
		       ( bTauIsHadron && MC::PID::isTau( motherPdgId ) ) ) ){
		    return false;
		}
		// } else {
		//     std::cout << "Mother is: " << motherPdgId << ", " << mother->status() << std::endl;
		// }

		// If we allow chained matching (I.e. not only _direct_ decays) and
		// the mother particle does come from a hadron, return false
		if ( ! bOnlyDirect && ! isNotFromHadron( mother, bOnlyDirect, bTauIsHadron ) ){
		    return false;
		}
	    }
	    // No hadronic && physical parents
	    return true;
	}

    }
}
