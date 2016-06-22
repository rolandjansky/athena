/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_FLAVOURUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_FLAVOURUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"

#include <vector>

namespace jet
{

class FlavourUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor
        FlavourUncertaintyComponent(const ComponentHelper& component,
                                    const TString jetType,
                                    const TString analysisRootFileName,
                                    const TString path
                                    );
        FlavourUncertaintyComponent(const FlavourUncertaintyComponent& toCopy);
        virtual FlavourUncertaintyComponent* clone() const;
        virtual ~FlavourUncertaintyComponent();
        virtual StatusCode initialize(TFile* histFile);

        // Extra information retrieval methods
        virtual FlavourComp::TypeEnum   getFlavourType() const  { return m_flavourType; }

    protected:

        // Uncertainty/validity retrieval helper methods
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        // Default constructor is forbidden
        FlavourUncertaintyComponent(const std::string& name = "");

        enum FlavourRespType { FlavourResp_UNKNOWN, FlavourResp_GLUON, FlavourResp_QUARK };

        // Additional private members
        const FlavourComp::TypeEnum m_flavourType;
        const TString m_jetType;
        const TString m_analysisFileName;
        const TString m_path;
        const bool m_absEta;
        const TString m_secondUncName;
        
        UncertaintyHistogram* m_secondUncHist;
        FlavourRespType m_respType;
        FlavourRespType m_secondRespType;
        SG::AuxElement::Accessor<char> m_BjetAccessor;
        SG::AuxElement::Accessor<int>  m_NjetAccessor;

        // Analysis histograms from analysis root file
        std::vector<UncertaintyHistogram*> m_gluonFractionHists;
        std::vector<UncertaintyHistogram*> m_gluonFractionErrorHists;

        // Wrappers for special flavour histograms
        double getFlavourResponseUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        double getFlavourCompositionUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        double getBJESUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        double getGluonFraction(const double pT, const double eta, const int nJets) const;
        double getGluonFractionError(const double pT, const double eta, const int nJets) const;
        double getGluonResponseDifference(const double pT, const double eta) const;
        double getGluonResponseBaseline(const double pT, const double eta) const;
        double getQuarkResponseBaseline(const double pT, const double eta) const;
        
        // Private helper indices and functions
        StatusCode readNjetsHistograms(std::vector<UncertaintyHistogram*>& hists, const std::vector<TString>& histKeys);
        StatusCode getNjetFromKey(const TString& key, int& nJets) const;
        StatusCode checkNjetsInput(int& nJets) const;
        bool isBjet(const xAOD::Jet& jet) const;
};

} // end jet namespace

#endif
