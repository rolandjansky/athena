/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_LOGPTMASSUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_LOGPTMASSUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class LogPtMassUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        LogPtMassUncertaintyComponent(const ComponentHelper& component);
        LogPtMassUncertaintyComponent(const LogPtMassUncertaintyComponent& toCopy);
        virtual LogPtMassUncertaintyComponent* clone() const;
        virtual ~LogPtMassUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        LogPtMassUncertaintyComponent(const std::string& name = "");
        const CompMassDef::TypeEnum m_massDef;
};

} // end jet namespace

#endif

