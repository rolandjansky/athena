/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PTETAUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PTETAUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PtEtaUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        PtEtaUncertaintyComponent(const ComponentHelper& component);
        PtEtaUncertaintyComponent(const PtEtaUncertaintyComponent& toCopy);
        virtual PtEtaUncertaintyComponent* clone() const;
        virtual ~PtEtaUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PtEtaUncertaintyComponent(const std::string& name = "");
        const bool m_absEta;
};

} // end jet namespace

#endif

