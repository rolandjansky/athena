/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYCOMPONENTGROUP_H
#define JETUNCERTAINTIES_UNCERTAINTYCOMPONENTGROUP_H

#include "JetUncertainties/UncertaintyComponent.h"

#include <vector>

namespace jet
{

class UncertaintyComponentGroup : public UncertaintyComponent
{
    public:
        // Constructor/destructor
        UncertaintyComponentGroup(const ComponentHelper& component);
        UncertaintyComponentGroup(const UncertaintyComponentGroup& toCopy);
        virtual UncertaintyComponentGroup* clone() const;
        virtual ~UncertaintyComponentGroup();

        // Pre-initialization and initialization
        // Note that this class takes control of memory management for passed components
        // Note that passed components should be pre-initialized
        virtual StatusCode addComponent(UncertaintyComponent* component);
        virtual StatusCode initialize();

        // Old initialization methods to block
        virtual StatusCode initialize(const std::vector<TString>& histNames, TFile* histFile);
        virtual StatusCode initialize(const std::vector<TString>& histNames, const std::vector<TString>& validHistNames, TFile* histFile);

        // Extra information retrieval methods
        virtual TString getDesc()          const { return TString(Form("%s (%zu components)",m_desc.Data(),m_components.size())); }
        virtual size_t  getNumComponents() const { return m_components.size(); }

        // Uncertainty retrieval methods (overwrite wrappers)
        virtual bool   getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual bool   getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        
        using UncertaintyComponent::getValidity;
        using UncertaintyComponent::getUncertainty;
        using UncertaintyComponent::getValidUncertainty;

    protected:
        // Uncertainty retrieval helper methods (implementations, blocked)
        virtual bool   getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual bool   getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        // Default constructor is forbidden
        UncertaintyComponentGroup(const std::string& name = "");

        // Additional private members
        std::vector<UncertaintyComponent*> m_components;
};

} // end jet namespace

#endif
