/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARGERTOPOLOGYUNCERTAINTYCOMPONENT_H
#define LARGERTOPOLOGYUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"
#include "ParticleJetTools/LargeRJetLabelEnum.h"

namespace jet
{

class LargeRTopologyUncertaintyComponent : public UncertaintyComponent
{

    public:
        // Constructor/destructor/initialization
        LargeRTopologyUncertaintyComponent(const ComponentHelper& component);
        LargeRTopologyUncertaintyComponent(const LargeRTopologyUncertaintyComponent& toCopy);
        virtual LargeRTopologyUncertaintyComponent* clone() const;
        virtual ~LargeRTopologyUncertaintyComponent() {}

    protected:
        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        LargeRTopologyUncertaintyComponent(const std::string& name = "");
        const bool m_absEta;
        std::string m_truthLabelName;
        std::vector<LargeRJetTruthLabel::TypeEnum> m_truthLabels;
}; 

} // end jet namespace

#endif

