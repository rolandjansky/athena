/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PTUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PTUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PtUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        PtUncertaintyComponent(const ComponentHelper& component);
        PtUncertaintyComponent(const PtUncertaintyComponent& toCopy);
        virtual PtUncertaintyComponent* clone() const;
        virtual ~PtUncertaintyComponent() {}
    
        using UncertaintyComponent::getValidity;
        using UncertaintyComponent::getUncertainty;
        using UncertaintyComponent::getValidUncertainty;

    protected:
        // Uncertainty retrieval helper methods (implementations)
        virtual bool   getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual bool   getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PtUncertaintyComponent(const std::string& name = "");

};

} // end jet namespace

#endif

