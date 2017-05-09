/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#ifndef JETTRACKTOOLS_JETTRACKSHRINKINGCONEASSOCIATION_H
#define JETTRACKTOOLS_JETTRACKSHRINKINGCONEASSOCIATION_H

#include "ParticleJetTools/JetTrackAssociation.h"

#include <vector>
#include <string>

namespace Analysis {

    class JetTrackShrinkingConeAssociation : public JetTrackAssociation {
        ASG_TOOL_CLASS2(JetTrackShrinkingConeAssociation, JetTrackAssociation, IJetModifier)
        public:

            JetTrackShrinkingConeAssociation(const std::string& name);

            const std::vector<std::vector<const xAOD::TrackParticle*> >*
                match(xAOD::JetContainer&) const;

            inline double coneSize(double pt) const {
                return (m_coneSizeFitPar1 + exp(m_coneSizeFitPar2 + m_coneSizeFitPar3*pt));
            }


        private:
            std::string m_inputTrackCollectionName;
            double m_coneSizeFitPar1;
            double m_coneSizeFitPar2;
            double m_coneSizeFitPar3;
    };

}
#endif

