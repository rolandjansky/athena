/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PTABSMASSETAUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PTABSMASSETAUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PtAbsMassEtaUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        PtAbsMassEtaUncertaintyComponent(const ComponentHelper& component);
        PtAbsMassEtaUncertaintyComponent(const PtAbsMassEtaUncertaintyComponent& toCopy);
        virtual PtAbsMassEtaUncertaintyComponent* clone() const;
        virtual ~PtAbsMassEtaUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PtAbsMassEtaUncertaintyComponent(const std::string& name = "");
        const bool m_absEta;
        const CompMassDef::TypeEnum m_massDef;
};

} // end jet namespace

#endif


