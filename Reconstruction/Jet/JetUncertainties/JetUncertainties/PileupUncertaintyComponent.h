/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PILEUPUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PILEUPUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PileupUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor
        PileupUncertaintyComponent( const ComponentHelper& component,
                                    const float refNPV,
                                    const float refMu
                                    );
        PileupUncertaintyComponent(const PileupUncertaintyComponent& toCopy);
        virtual PileupUncertaintyComponent* clone() const;
        virtual ~PileupUncertaintyComponent() {}
        virtual StatusCode Initialize(const std::vector<TString>& histNames, TFile* histFile);
        virtual StatusCode Initialize(const std::vector<TString>& histNames, const std::vector<TString>& validHistNames, TFile* histFile);
        
        // Extra information retrieval methods
        virtual PileupComp::TypeEnum    getPileupType() const   { return m_pileupType; }
        virtual float                   getNPVRef()     const   { return m_refNPV;     }
        virtual float                   getMuRef()      const   { return m_refMu;      }

        using UncertaintyComponent::getValidity;
        using UncertaintyComponent::getUncertainty;
        using UncertaintyComponent::getValidUncertainty;

    protected:
        // Uncertainty retrieval helper methods (implementations)
        virtual bool   getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual bool   getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        // Default constructor is forbidden
        PileupUncertaintyComponent(const std::string& name = "");

        // Additional private members
        const PileupComp::TypeEnum m_pileupType;
        const float m_refNPV;
        const float m_refMu;
        const bool m_absEta;
        SG::AuxElement::Accessor<float> m_NPVaccessor;

        // Helper indices/methods
        static const size_t PT_TERM_NPV;
        static const size_t PT_TERM_MU;
        double getPileupWeight(const UncertaintyHistogram* histo, const xAOD::EventInfo& eInfo) const;
        //unsigned getDefaultNPV() const;
};

} // end jet namespace

#endif
