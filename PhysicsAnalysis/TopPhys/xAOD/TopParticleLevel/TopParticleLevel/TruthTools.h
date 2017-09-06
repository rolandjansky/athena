/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: TruthTools.h
// Description:
// Author: Fabian Wilk
// Created: Wed Feb  4 11:04:23 2015

#ifndef _TOP_PARTICLE_LEVEL_TRUTHTOOLS_H_
#define _TOP_PARTICLE_LEVEL_TRUTHTOOLS_H_

#include <ostream>
#include <iostream>
#include <string>

#include "xAODTruth/TruthParticleContainer.h"

namespace top {
    namespace truth {

	/*!
	 * @brief Prints the decay chain leading up to a certain particle to a
	 *  std::ostream.
	 * @param part Pointer to xAOD::TruthParticle whose decay origin shall
	 *  be printed.
	 * @param os The std::ostream into which the decay chain shall be
	 *  printed. Defaults to std::cout.
	 * @param prefix A string which will be prefixed to the output.
	 */
	void printDecayChain( const xAOD::TruthParticle * part,
			      std::ostream & os = std::cout,
			      const std::string & prefix = "" );

	/**
	 * @brief Check if a certain pdg ID corresponds to a B hadron.
	 *  This follows the official PDG Monte-Carlo numbering scheme
	 *	(c.f. http://pdg.lbl.gov/2007/reviews/montecarlorpp.pdf) which defines
	 *	the PDG ID to be a 7 digit number in the form
	 *		+- n_r n_L n_q1 n_q2 n_q3 n_J
	 *  where
	 * 		n_q1 = 0, n_q2 >= n_q3	(for mesons)
	 * 		n_q1 >= n_q2 >= n_q3	(for baryons)
	 * 		n_q1 >= n_q2, n_q3 = 0	(for diquarks)
	 *  the n_qi denote the flavour of the quark composing the hadron, i.e.
	 * 		n_q = 1   -> d		n_q = 2   -> u
	 * 		n_q = 3   -> s		n_q = 4   -> 4
	 * 		n_q = 5   -> b
	 *  The digit \f$n_J\f$ encodes the total angular momentum \f$J\f$ of a
	 *  compound system. \f$n_L\f$ is used to distinguish compound objects which
	 *  have the same total angular momentum but different spin (\f$S\f$) and orbital
	 *  (\f$L\f$) angular momentum numbers. Finally, \f$n_r\f$ is used to label
	 *  mesons which are radially exited.
	 *
	 * @param pdgId The PDG ID to test.
	 *
	 * @return True if pdg ID corresponds to B hadron, false otherwise.
	 */
	inline bool isBHadron( int pdgId ){
	    return ( ( 510 < pdgId && pdgId < 555 ) ||   // Check for (normal) Mesons / Diquarks
		     ( 5100 < pdgId && pdgId < 5555 ) || // Check for (normal) Baryons
		     ( pdgId / 100 ) % 10 == 5 ||	 // Check exited Mesons (and Baryons with double beauty)
		     ( pdgId / 1000 ) % 10 == 5 );	 // Check exited Baryons
	}


	/**
	 * @brief Function to determine whether the input particle was produced
	 * in the physical decay of a particle of given PDG ID.
	 *
	 * The algorithm uses the MC Truth Record.
	 * The algorithm works by ascending through the decay chain following
	 * the production vertex reference (obtained by the prodVtx() member
	 * function of the xAOD::TruthParticle). From the input particle, the
	 * algorithm ascends through the chain until the PDG ID of the particle
	 * found as first incoming particle is not equal to the PDG ID of the
	 * input particle. The last production vertex found during this ascent
	 * should then correspond to the physical production vertex of the input
	 * particle.
	 *
	 * @param truthParticle The input particle.
	 * @param parentPDGIds A list of PDG IDs which are valid parents
	 * @param bOnlyDirect A flag denoting whether "chained matching" is
	 *  allowed. In chained matching mode, intermittend particles are
	 *  allowed, i.e. for the chain
	 *     A -> B -> C
	 *  isFrom( C, A ) is true (for A != B != C). If only direct decays are
	 *  considered, this is only the case if A == B or B == C. Defaults to false.
	 *
	 * @return Return true if the input particle was produced in the decay
	 *         of a particle which has a PDG ID that is listed in
	 *         parentPDGIds. Return false otherwise. Also return false if
	 *         the input particle has an invalid production vertex reference.
	 */
	bool isFrom( const xAOD::TruthParticle * truthParticle,
		     const std::vector<int> & parentPDGIds,
		     bool bOnlyDirect = false );

	/**
	 * @brief Function to determine whether a given truth particle is a
	 * result of a decay chain that started in the decay of a W or Z boson.
	 *
	 * The algorithm works by ascending through the decay chain following
	 * the production vertex reference (obtained by the prodVtx() member
	 * function of the xAOD::TruthParticle). From the input particle, the
	 * algorithm ascends through the chain until the PDG ID of the particle
	 * found as first incoming particle is not equal to the PDG ID of the
	 * input particle. The last production vertex found during this ascent
	 * should then correspond to the physical production vertex of the input
	 * particle.
	 *
	 * If the PDG ID of the mother particle (i.e. the incoming particle to
	 * the truth decay vertex) corresponds to a W / Z boson the algorithm
	 * terminates directly, returning true. Otherwise a vertex-based check
	 * is employed (which was designed to operate on SHERPA samples, c.f.
	 * the JetSimTools-00-02-09 package
	 * [svn: /Reconstruction/Jet/JetSimTools]).
	 *
	 * @param truthParticle The input particle.
	 * @param bOnlyDirect A flag denoting whether "chained matching" is
	 *  allowed. In chained matching mode, intermittend particles are
	 *  allowed, i.e. for the chain
	 *     A -> B -> C
	 *  isFrom( C, A ) is true (for A != B != C). If only direct decays are
	 *  considered, this is only the case if A == B or B == C. Defaults to false.
	 *
	 * @return Return true if the input particle was created in a physical
	 *         decay of a W or Z boson. Otherwise return false. Will also
	 *         return false if the prduction vertex of the input particle is
	 *         invalid or has zero incoming particles.
	 */
	bool isFromWZDecay( const xAOD::TruthParticle * truthParticle,
			    bool bOnlyDirect = false );

	/**
	 * @brief Function the check if a particle is a lepton which was
	 * produced in the decay of a tau lepton.
	 *
	 * @param truthParticle The input truth particle which will be checked.
	 *
	 * @return True if the input particle is a lepton and it has a parent
	 *         which was a tau. False otherwise. Will also return false if
	 *         the production vertex of the input particle is invalid.
	 */
	bool isLeptonFromTau( const xAOD::TruthParticle * truthParticle );

	/**
	 * @brief Check whether a given truth particle is a lepton.
	 * @param truthParticle The input truth particle to be tested.
	 * @return Return true if input is a lepton, false otherwise.
	 */
	inline bool isLepton( const xAOD::TruthParticle * truthParticle ){
	    return ( 10 < truthParticle->absPdgId() && truthParticle->absPdgId() < 17 );
	}

	/*!
	 * @brief Check whether a given truth particle is not produced in the
	 *  decay of physical hadrons or their descendents. This must be done in
	 *  a recursive manner, i.e. the decay chain must be traversed upwards
	 *  until the initial state is found.
	 * @param truthParticle The truth particle to be tested.
	 * @param bOnlyDirect A flag denoting whether "chained matching" is
	 *  allowed. In chained matching mode, intermittend particles are
	 *  allowed, i.e. for the chain
	 *     A -> B -> C
	 *  isFrom( C, A ) is true (for A != B != C). If only direct decays are
	 *  considered, this is only the case if A == B or B == C. Defaults to
	 *  false.
	 * @param bTauIsHadron If true, any intermediate tauons are treated as hadrons.
	 * @return True if truthParticle does not descend from a physical
	 *  hadron, false otherwise.
	 */
	bool isNotFromHadron( const xAOD::TruthParticle * truthParticle,
			      bool bOnlyDirect = false, bool bTauIsHadron = false );

    }
}

#endif /* _TOP_PARTICLE_LEVEL_TRUTHTOOLS_H_ */
