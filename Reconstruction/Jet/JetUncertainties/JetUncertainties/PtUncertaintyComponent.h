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

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PtUncertaintyComponent(const std::string& name = "");

};

} // end jet namespace

#endif

