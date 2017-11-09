/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#ifndef PARTICLEJETTOOLS_JETPARTICLEASSOCIATION_H
#define PARTICLEJETTOOLS_JETPARTICLEASSOCIATION_H

#include "AsgTools/AsgTool.h"
#include "JetInterface/IJetModifier.h"
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticle.h"

#include <vector>
#include <string>


class JetParticleAssociation : public IJetModifier, public asg::AsgTool {
    ASG_TOOL_INTERFACE(JetParticleAssociation)
    public:

        JetParticleAssociation(const std::string& name);

        StatusCode initialize();

        // obvs to be provided by the deriving class
        virtual const std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > >*
            match(const xAOD::JetContainer&) const = 0;

        int modify(xAOD::JetContainer& jets) const;

    private:
        std::string m_OutputCollectionName;
        SG::AuxElement::Decorator<std::vector<ElementLink<xAOD::IParticleContainer> > > dec;
};

#endif
