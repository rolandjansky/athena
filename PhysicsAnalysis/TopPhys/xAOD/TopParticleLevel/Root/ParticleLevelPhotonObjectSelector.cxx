/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: ParticleLevelPhotonObjectSelector.cxx
// Description:
// Author: Fabian Wilk
// Created: Wed Oct 12 11:41:36 2016
//
// (c) by The ATLAS Collaboration
//     by Fabian Wilk <mail@fabianwilk.de>
//
// This file is licensed under a Creative Commons Attribution-ShareAlike 4.0
// International License.
//
// You should have received a copy of the license along with this work.
// If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.

#include "TopParticleLevel/ParticleLevelPhotonObjectSelector.h"

#include "TopConfiguration/Tokenize.h"

namespace top {
    ParticleLevelPhotonObjectSelector::ParticleLevelPhotonObjectSelector(Options opt /* = Options() */)
        : m_opt(opt){ /* Deliberately Empty */ }

    ParticleLevelPhotonObjectSelector::Options::Options(double ptMin /* = 25.e3 */,
                    double etaMax /* = 2.5 */,
                    const std::string & Origin /* = "" */,
                    const std::string & Isolation /* = "" */) :
                pt_min(ptMin),
                eta_max(etaMax),
                origin(),
                isolationVar(""),
                isolationCut(){

                    // =========================================================
                    // Parse the TruthOrigin Configuration Parameter.
                    if (Origin == ""  || Origin == " "|| Origin == "False" || Origin == "None"){
                         // Deliberately Empty
                    } else {
                        // This allows us to convert from string name to enum value.
                        MCTruthPartClassifier::ParticleDef def;

                        // Tokenize at comma.
                        std::vector<std::string> tokens;
                        tokenize(Origin, tokens, ",");

                        while (tokens.size()){
                            const auto & token = tokens.back();

                            auto it = std::find(def.sParticleOrigin.begin(),
                                    def.sParticleOrigin.end(),
                                    token);

                            top::check(it != def.sParticleOrigin.end(),
                                    "[ParticleLevelPhotonObjectSelector] Invalid particle origin '" + token + "'");

                            origin.push_back(
                                    static_cast<MCTruthPartClassifier::ParticleOrigin>(
                                        std::distance(def.sParticleOrigin.begin(),
                                            it)));

                            tokens.pop_back();
                        }
                    }

                    // =========================================================
                    // Parse the TruthIsolation Configuration Parameter
                    if (Isolation == "" || Isolation == " " || Isolation == "False" || Isolation == "None"){
                         isolationVar = "";
                    } else {
                        // Split at space, should be exactly 2 tokens.
                        std::vector<std::string> tokens;
                        tokenize(Isolation, tokens, " ");
                        top::check(tokens.size() == 2,
                                   "[ParticleLevelPhotonObjectSelector] Invalid input for isolation parameter (expected 2 tokens).");

                        isolationVar = tokens.at(0);
                        isolationCut = std::stof(tokens.at(1));
                    }
                }


    /* virtual */ bool ParticleLevelPhotonObjectSelector::apply(const xAOD::TruthParticle & truthParticle){
        // --------------------------------------------------
        // Require that the photon is status=1 (stable)
        // TODO: Should we include other statuses?
        if (truthParticle.status() != 1){
            return false;
        }

        // --------------------------------------------------
        // Apply kinematic cut on the pT:
        //     must exceed 25 GeV
        if ( truthParticle.pt() < m_opt.pt_min ){ return false; }

        // --------------------------------------------------
        // Apply kinematic cut on the eta:
        //     must be less than or equal to 2.5
        if ( std::abs( truthParticle.eta() ) > m_opt.eta_max ){ return false; }

        // --------------------------------------------------
        // Apply particle origin cut.
        if (m_opt.origin.size()){
            unsigned int origin;
            if ( truthParticle.isAvailable<unsigned int>( "particleOrigin" ) ){
                origin = truthParticle.auxdata<unsigned int>( "particleOrigin" );
            } else if ( truthParticle.isAvailable<unsigned int>( "classifierParticleOrigin" ) ){
                origin = truthParticle.auxdata<unsigned int>( "classifierParticleOrigin" );
            } else {
                top::check(false, "Could not obtain MCTruthClassifier result decoration.");
            }

            if (std::find(m_opt.origin.begin(), m_opt.origin.end(), origin) == m_opt.origin.end()){
                return false;
            }
        }

        // --------------------------------------------------
        // Apply isolation cut
        if (m_opt.isolationVar.size()){
            top::check(truthParticle.isAvailable<float>(m_opt.isolationVar),
                       "[ParticleLevelPhotonObjectSelector] Selected isolation variable not available!");
            if (m_opt.isolationCut <= truthParticle.auxdata<float>(m_opt.isolationVar) / truthParticle.pt()){
                return false;
            }
        }

        // --------------------------------------------------
        // Everything that reaches this point has passed the selection
        return true;
    }
}
