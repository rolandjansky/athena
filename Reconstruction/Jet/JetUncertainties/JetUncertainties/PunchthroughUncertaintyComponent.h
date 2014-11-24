/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PUNCHTHROUGHUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PUNCHTHROUGHUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PunchthroughUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        PunchthroughUncertaintyComponent(const ComponentHelper& component);
        PunchthroughUncertaintyComponent(const PunchthroughUncertaintyComponent& toCopy);
        virtual PunchthroughUncertaintyComponent* clone() const;
        virtual ~PunchthroughUncertaintyComponent() {}
        
        using UncertaintyComponent::getValidity;
        using UncertaintyComponent::getUncertainty;
        using UncertaintyComponent::getValidUncertainty;

    protected:
        // Uncertainty retrieval helper methods (implementations)
        virtual bool   getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual bool   getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PunchthroughUncertaintyComponent(const std::string& name = "");
        const bool m_absEta;
        SG::AuxElement::Accessor<int> m_NSegmentAccessor;
};

} // end jet namespace

#endif

