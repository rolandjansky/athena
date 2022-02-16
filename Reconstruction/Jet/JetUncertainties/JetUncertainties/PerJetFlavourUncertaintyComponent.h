/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_PERJETFLAVOURUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_PERJETFLAVOURUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

namespace jet
{

class PerJetFlavourUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor
        PerJetFlavourUncertaintyComponent(const ComponentHelper& component);
        PerJetFlavourUncertaintyComponent(const PerJetFlavourUncertaintyComponent& toCopy);
        virtual PerJetFlavourUncertaintyComponent* clone() const;
        virtual ~PerJetFlavourUncertaintyComponent() {}
        virtual StatusCode initialize(TFile* histFile);

        // Extra information retrieval methods
        virtual std::vector<int> getRelevantLabels() const { return m_labels; }

    protected:
        
        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        // Default constructor is forbidden
        PerJetFlavourUncertaintyComponent(const std::string& name = "");

        // Additional private members
        const bool    m_absEta;
        const std::vector<int> m_labels;
        const FlavourComp::TypeEnum m_flavourType;

        // Wrappers for special flavour histograms
        double getFlavourResponseUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        bool   isSupportedLabel(const int label) const;
        bool   checkTruthLabel(const xAOD::Jet& jet) const;

};

} // end jet namespace


#endif

