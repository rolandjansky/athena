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

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PunchthroughUncertaintyComponent(const std::string& name = "");
        const bool m_absEta;
        int getNumSegments(const xAOD::Jet& jet) const;
};

} // end jet namespace

#endif

