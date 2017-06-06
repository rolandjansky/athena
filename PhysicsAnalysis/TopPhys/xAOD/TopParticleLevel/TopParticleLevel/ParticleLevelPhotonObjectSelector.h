/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// Filename: ParticleLevelPhotonObjectSelector.h
// Description:
// Author: Fabian Wilk
// Created: Wed Oct 12 11:37:51 2016
//
// (c) by The ATLAS Collaboration
//     by Fabian Wilk <mail@fabianwilk.de>
//
// This file is licensed under a Creative Commons Attribution-ShareAlike 4.0
// International License.
//
// You should have received a copy of the license along with this work.
// If not, see <http://creativecommons.org/licenses/by-sa/4.0/>.

#ifndef _TOP_PARTICLE_LEVEL_PHOTONOBJECTSELECTOR_H_
#define _TOP_PARTICLE_LEVEL_PHOTONOBJECTSELECTOR_H_

#include "TopParticleLevel/ObjectSelectorBase.h"

#include "xAODTruth/TruthParticleContainer.h"

#include "MCTruthClassifier/MCTruthClassifierDefs.h"

#include <vector>
#include <string>
#include <sstream>

#include "TopEvent/EventTools.h"

namespace top {

    class ParticleLevelPhotonObjectSelector
        : public ObjectSelectorBase<xAOD::TruthParticle> {
    public:
        // Struct used to store the cut values used by the object selector. It
        // is important that the object has a sensible default config because it
        // will be default constructed for the trivial constructor of the object
        // selector.
        struct Options {
            Options(double ptMin = 25.e3,
                    double etaMax = 2.5,
                    const std::string & Origin = "",
                    const std::string & Isolation = "");

            double  pt_min;
            double  eta_max;
            std::vector<MCTruthPartClassifier::ParticleOrigin> origin;
            std::string isolationVar;
            float isolationCut;
        };

    public:
        ParticleLevelPhotonObjectSelector(Options cuts = Options());
        virtual bool apply(const xAOD::TruthParticle & truthParticle);

    private:
        Options m_opt;
    };
}

#endif /* _TOP_PARTICLE_LEVEL_PHOTONOBJECTSELECTOR_H_ */
