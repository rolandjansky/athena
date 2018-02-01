/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#ifndef PARTICLEJETTOOLS_JETPARTICLEFIXEDCONEASSOCIATION_H
#define PARTICLEJETTOOLS_JETPARTICLEFIXEDCONEASSOCIATION_H

#include "ParticleJetTools/JetParticleAssociation.h"

#include <vector>
#include <string>

class JetParticleFixedConeAssociation : public JetParticleAssociation {
    ASG_TOOL_CLASS(JetParticleFixedConeAssociation, JetParticleAssociation)
    public:

        JetParticleFixedConeAssociation(const std::string& name);

        const std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > >*
            match(const xAOD::JetContainer&) const;


    private:
        std::string m_InputParticleCollectionName;
        double m_ConeSize;
};

#endif
