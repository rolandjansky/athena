/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_ELOGMASSUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_ELOGMASSUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class ELogMassUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        ELogMassUncertaintyComponent(const ComponentHelper& component);
        ELogMassUncertaintyComponent(const ELogMassUncertaintyComponent& toCopy);
        virtual ELogMassUncertaintyComponent* clone() const;
        virtual ~ELogMassUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        ELogMassUncertaintyComponent(const std::string& name = "");
        const CompMassDef::TypeEnum m_massDef;
};

} // end jet namespace

#endif

