/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PTABSMASSUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PTABSMASSUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PtAbsMassUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        PtAbsMassUncertaintyComponent(const ComponentHelper& component);
        PtAbsMassUncertaintyComponent(const PtAbsMassUncertaintyComponent& toCopy);
        virtual PtAbsMassUncertaintyComponent* clone() const;
        virtual ~PtAbsMassUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PtAbsMassUncertaintyComponent(const std::string& name = "");
        const CompMassDef::TypeEnum m_massDef;

        // In rare cases, this may only be applied for a given truth label
        std::string m_truthLabelName;
        std::vector<LargeRJetTruthLabel::TypeEnum> m_truthLabels;
};

} // end jet namespace

#endif


