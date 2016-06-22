/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_CLOSEBYUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_CLOSEBYUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class ClosebyUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        ClosebyUncertaintyComponent(const ComponentHelper& component);
        ClosebyUncertaintyComponent(const ClosebyUncertaintyComponent& toCopy);
        virtual ClosebyUncertaintyComponent* clone() const;
        virtual ~ClosebyUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        ClosebyUncertaintyComponent(const std::string& name = "");

        float getMinDR(const xAOD::Jet& jet) const;

};

} // end jet namespace

#endif

