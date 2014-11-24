/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PTMASSETAUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PTMASSETAUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PtMassEtaUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        PtMassEtaUncertaintyComponent(const ComponentHelper& component);
        PtMassEtaUncertaintyComponent(const PtMassEtaUncertaintyComponent& toCopy);
        virtual PtMassEtaUncertaintyComponent* clone() const;
        virtual ~PtMassEtaUncertaintyComponent() {}
        
        using UncertaintyComponent::getValidity;
        using UncertaintyComponent::getUncertainty;
        using UncertaintyComponent::getValidUncertainty;
        
    protected:
        // Uncertainty retrieval helper methods (implementations)
        virtual bool   getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual bool   getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PtMassEtaUncertaintyComponent(const std::string& name = "");
        const bool m_absEta;
};

} // end jet namespace

#endif

