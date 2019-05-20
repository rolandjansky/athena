/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_ELOGPTMASSFORTAGSFUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_ELOGPTMASSFORTAGSFUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class ELogPtMassForTagSFUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        ELogPtMassForTagSFUncertaintyComponent(const ComponentHelper& component);
        ELogPtMassForTagSFUncertaintyComponent(const ELogPtMassForTagSFUncertaintyComponent& toCopy);
        virtual ELogPtMassForTagSFUncertaintyComponent* clone() const;
        virtual ~ELogPtMassForTagSFUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        ELogPtMassForTagSFUncertaintyComponent(const std::string& name = "");
        const CompMassDef::TypeEnum m_massDef;
	const TString m_label;
};

} // end jet namespace

#endif

