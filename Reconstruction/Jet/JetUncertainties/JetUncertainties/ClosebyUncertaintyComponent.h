/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_CLOSEBYUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_CLOSEBYUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class ClosebyUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        ClosebyUncertaintyComponent(const ComponentHelper& component);
        ClosebyUncertaintyComponent(const ClosebyUncertaintyComponent& toCopy);
        virtual ClosebyUncertaintyComponent* clone() const;
        virtual ~ClosebyUncertaintyComponent() {}
        
        using UncertaintyComponent::getValidity;
        using UncertaintyComponent::getUncertainty;
        using UncertaintyComponent::getValidUncertainty;

    protected:
        // Uncertainty retrieval helper methods (implementations)
        virtual bool   getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual bool   getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        ClosebyUncertaintyComponent(const std::string& name = "");
        SG::AuxElement::Accessor<float> m_minDRaccessor;

};

} // end jet namespace

#endif

