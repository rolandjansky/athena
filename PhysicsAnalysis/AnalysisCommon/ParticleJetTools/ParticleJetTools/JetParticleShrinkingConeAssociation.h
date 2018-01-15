/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#ifndef PARTICLEJETTOOLS_JETPARTICLESHRINKINGCONEASSOCIATION_H
#define PARTICLEJETTOOLS_JETPARTICLESHRINKINGCONEASSOCIATION_H

#include "ParticleJetTools/JetParticleAssociation.h"

#include <vector>
#include <string>

class JetParticleShrinkingConeAssociation : public JetParticleAssociation {
    ASG_TOOL_CLASS(JetParticleShrinkingConeAssociation, JetParticleAssociation)
    public:

        JetParticleShrinkingConeAssociation(const std::string& name);

        const std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > >*
            match(const xAOD::JetContainer&) const;

        inline double coneSize(double pt) const {
            return (m_ConeSizeFitPar1 + exp(m_ConeSizeFitPar2 + m_ConeSizeFitPar3*pt));
        }


    private:
        std::string m_InputParticleCollectionName;
        double m_ConeSizeFitPar1;
        double m_ConeSizeFitPar2;
        double m_ConeSizeFitPar3;
};

#endif
