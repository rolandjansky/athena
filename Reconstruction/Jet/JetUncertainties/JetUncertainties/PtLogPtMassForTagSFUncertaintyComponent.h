/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PTLOGPTMASSFORTAGSFUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PTLOGPTMASSFORTAGSFUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"
#include "ParticleJetTools/LargeRJetLabelEnum.h"

namespace jet
{

class PtLogPtMassForTagSFUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor/initialization
        PtLogPtMassForTagSFUncertaintyComponent(const ComponentHelper& component);
        PtLogPtMassForTagSFUncertaintyComponent(const PtLogPtMassForTagSFUncertaintyComponent& toCopy);
        virtual PtLogPtMassForTagSFUncertaintyComponent* clone() const;
        virtual ~PtLogPtMassForTagSFUncertaintyComponent() {}

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        PtLogPtMassForTagSFUncertaintyComponent(const std::string& name = "");
        const CompMassDef::TypeEnum m_massDef;
        std::vector<CompFlavorLabelVar::TypeEnum> m_labels;
        std::string m_largeRJetTruthLabelName;
        const CompTaggerRegionVar::TypeEnum m_region;
        const TString m_result_name;
};

} // end jet namespace

#endif

