/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_ELOGMASSETAUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_ELOGMASSETAUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class ELogMassEtaUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        ELogMassEtaUncertaintyComponent(const ComponentHelper& component);
        ELogMassEtaUncertaintyComponent(const ELogMassEtaUncertaintyComponent& toCopy);
        virtual ELogMassEtaUncertaintyComponent* clone() const;
        virtual ~ELogMassEtaUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        ELogMassEtaUncertaintyComponent(const std::string& name = "");
        const bool m_absEta;
        const CompMassDef::TypeEnum m_massDef;
};

} // end jet namespace

#endif

