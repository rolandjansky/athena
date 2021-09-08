/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_COMBINEDMASSUNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_COMBINEDMASSUNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyComponent.h"
#include "JetUncertainties/Helpers.h"
#include "ParticleJetTools/LargeRJetLabelEnum.h"

namespace jet
{

class UncertaintyGroup;

class CombinedMassUncertaintyComponent : public UncertaintyComponent
{
    public:
        // Constructor/destructor
        CombinedMassUncertaintyComponent(const ComponentHelper& component);
        CombinedMassUncertaintyComponent(const CombinedMassUncertaintyComponent& toCopy);
        virtual CombinedMassUncertaintyComponent* clone() const;
        virtual ~CombinedMassUncertaintyComponent();
        virtual StatusCode setCaloTerm(UncertaintyGroup* caloComp);
        virtual StatusCode setTATerm(  UncertaintyGroup* TAComp);
        virtual StatusCode setCaloWeights(const UncertaintyHistogram* caloWeights);
        virtual StatusCode setTAWeights(  const UncertaintyHistogram* TAWeights);
        virtual StatusCode setCombWeightMassDefs(const CompMassDef::TypeEnum caloMassDef, const CompMassDef::TypeEnum TAMassDef);
        virtual StatusCode setCombWeightParam(const CompParametrization::TypeEnum param);
        virtual StatusCode initialize(TFile* histFile);

        // Extra information retrieval methods
        virtual CombMassComp::TypeEnum  getCombMassType() const { return m_combMassType; }

        // Methods for calo-only and TA-only uncertainty and weight retrieval
        virtual bool   getValidityCalo(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)                      const;
        virtual bool   getValidityTA(  const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)                      const;
        virtual double getUncertaintyCalo(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)                   const;
        virtual double getUncertaintyTA(  const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)                   const;
        virtual bool   getValidUncertaintyCalo(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual bool   getValidUncertaintyTA(  double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

        // Method overrides
        virtual bool   isAlwaysZero() const;

    protected:
        // Uncertainty/validity retrieval helper methods
        virtual bool getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    private:
        // Default constructor is forbidden
        CombinedMassUncertaintyComponent(const std::string& name = "");

        // Additional private members
        const CombMassComp::TypeEnum m_combMassType;
        bool m_setWeightMassDefs;

        // The two possible sub-components/groups
        UncertaintyGroup* m_caloMassComp;
        UncertaintyGroup* m_TAMassComp;
        // The two possible weight histograms
        const UncertaintyHistogram* m_caloMassWeight;
        const UncertaintyHistogram* m_TAMassWeight;
        // The two possible scale helpers
        JetFourMomAccessor m_caloMassScale_weights;
        JetFourMomAccessor m_TAMassScale_weights;
        // The weight parametrization
        CompParametrization::TypeEnum m_weightParam;

        // In rare cases, this may only be applied for a given truth label
        std::string m_truthLabelName;
        std::vector<LargeRJetTruthLabel::TypeEnum> m_truthLabels;

        // Helper functions
        double readHistoFromParam(const xAOD::JetFourMom_t& jet4vec, const UncertaintyHistogram& histo, const CompParametrization::TypeEnum param, const double massShiftFactor) const;
        virtual double getWeightFactorCalo(const xAOD::Jet& jet, const double shiftFactor) const;
        virtual double getWeightFactorTA(  const xAOD::Jet& jet, const double shiftFactor) const;
        StatusCode calculateCombinedMass(const xAOD::Jet& jet, const double shiftFactorCalo, const double shiftFactorTA, double& combMass) const;
};

} // end jet namespace

#endif
