/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// author: jie.yu@cern.ch

#ifndef PARTICLEJETTOOLS_JETPARTICLECENTEROFMASSASSOCIATION_H
#define PARTICLEJETTOOLS_JETPARTICLECENTEROFMASSASSOCIATION_H

#include "ParticleJetTools/JetParticleAssociation.h"

#include <vector>
#include <string>

class JetParticleCenterOfMassAssociation : public JetParticleAssociation {
    ASG_TOOL_CLASS(JetParticleCenterOfMassAssociation, IJetDecorator)
    public:

        JetParticleCenterOfMassAssociation(const std::string& name);

        virtual const std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > >*
            match(const xAOD::JetContainer&, const xAOD::IParticleContainer&) const override;

        inline double getAngleSize(const double& par_R) const{ 
            double result = acos(1-par_R*0.5);
            return result;
        }


    private:
        double m_partMatchCone;
        double m_parentJetCone;
};

#endif
