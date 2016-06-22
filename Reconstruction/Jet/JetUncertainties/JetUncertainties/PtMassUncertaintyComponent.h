/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PTMASSUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PTMASSUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PtMassUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        PtMassUncertaintyComponent(const ComponentHelper& component);
        PtMassUncertaintyComponent(const PtMassUncertaintyComponent& toCopy);
        virtual PtMassUncertaintyComponent* clone() const;
        virtual ~PtMassUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PtMassUncertaintyComponent(const std::string& name = "");
        const CompMassDef::TypeEnum m_massDef;
};

} // end jet namespace

#endif

