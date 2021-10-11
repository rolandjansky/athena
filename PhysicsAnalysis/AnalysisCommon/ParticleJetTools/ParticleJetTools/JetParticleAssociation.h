/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// author: cpollard@cern.ch

#ifndef PARTICLEJETTOOLS_JETPARTICLEASSOCIATION_H
#define PARTICLEJETTOOLS_JETPARTICLEASSOCIATION_H

#include "AsgTools/AsgTool.h"
#include "AsgTools/PropertyWrapper.h"
#include "AsgDataHandles/WriteDecorHandleKey.h"
#include "xAODJet/JetContainer.h"
#include "xAODBase/IParticleContainer.h"
#include "JetInterface/IJetDecorator.h"

#include <vector>
#include <string>


class JetParticleAssociation : public asg::AsgTool,
                               virtual public IJetDecorator {
    ASG_TOOL_CLASS(JetParticleAssociation, IJetDecorator)

    public:

        JetParticleAssociation(const std::string& name);

        virtual StatusCode initialize() override;
        virtual StatusCode decorate(const xAOD::JetContainer& jets) const override;

        // obvs to be provided by the deriving class
        virtual const std::vector<std::vector<ElementLink<xAOD::IParticleContainer> > >*
            match(const xAOD::JetContainer&, const xAOD::IParticleContainer&) const = 0;

    private:

        // note
        // if m_particleKey is "", then an empty container will be written to the OutputDecoration.
        Gaudi::Property<std::string> m_jetContainerName{this, "JetContainer", "", "Jet collection name"};
        SG::ReadHandleKey<xAOD::IParticleContainer> m_particleKey{this, "InputParticleContainer", "", "Input particle collection name"};
        SG::WriteDecorHandleKey<xAOD::JetContainer> m_decKey{this, "OutputDecoration", "", "Output decoration name"};
};

#endif
