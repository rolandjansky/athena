/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: jie.yu@cern.ch

#ifndef PARTICLEJETTOOLS_JETPARTICLECENTEROFMASSASSOCIATION_H
#define PARTICLEJETTOOLS_JETPARTICLECENTEROFMASSASSOCIATION_H

#include "AsgTools/AsgTool.h"
#include "ParticleJetTools/JetParticleAssociation.h"

#include <vector>
#include <string>

class JetParticleCenterOfMassAssociation : public JetParticleAssociation {
    ASG_TOOL_CLASS(JetParticleCenterOfMassAssociation, JetParticleAssociation)
    public:

        JetParticleCenterOfMassAssociation(const std::string& name);

        const std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > >*
            match(const xAOD::JetContainer&) const;

        inline double getAngleSize(const double& par_R) const{ 
            double result = acos(1-par_R*0.5);
            return result;
        }


    private:
        std::string m_inputTrackCollectionName;
        double m_partMatchCone;
        double m_parentJetCone;
};

#endif
