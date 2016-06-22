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

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PtMassEtaUncertaintyComponent(const std::string& name = "");
        const bool m_absEta;
        const CompMassDef::TypeEnum m_massDef;
};

} // end jet namespace

#endif

