/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_UNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyEnum.h"
#include "JetUncertainties/UncertaintyHistogram.h"
#include "JetUncertainties/ConfigHelper.h"

#include "xAODJet/Jet.h"
#include "xAODEventInfo/EventInfo.h"

#include "AsgMessaging/AsgMessaging.h"

#include "TString.h"

class TFile;

namespace jet
{

class UncertaintyComponent : public asg::AsgMessaging
{
    public:
        // Constructor/destructor/initialization
        UncertaintyComponent(const ComponentHelper& component, const size_t numHist = 1);
        UncertaintyComponent(const UncertaintyComponent& toCopy);
        virtual UncertaintyComponent* clone() const = 0;
        UncertaintyComponent & operator = (const UncertaintyComponent &) = delete;
        virtual ~UncertaintyComponent();
        virtual StatusCode initialize(TFile* histFile);
        
        // Member retrieval methods
        virtual TString                getName()      const { return m_uncHistName;   }
        virtual TString                getValidName() const { return m_validHistName; }
        virtual CompScaleVar::TypeEnum getScaleVar()  const { return m_scaleVar;      }

        // Helpers for special situations
        virtual bool   isAlwaysZero() const;
        
        // Uncertainty/validity retrieval methods
        virtual bool   getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)                      const;
        virtual double getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)                   const;
        virtual bool   getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;

    protected:
        // Protected members
        bool m_isInit;
        const TString m_uncHistName;
        const TString m_validHistName;
        const CompScaleVar::TypeEnum m_scaleVar;
        const float m_energyScale;
        const bool m_interpolate;
        const int m_splitNumber;
        
        int m_numExpectedHist{};
        UncertaintyHistogram* m_uncHist;
        UncertaintyHistogram* m_validHist;

        // Methods for derived classes to implement
        virtual bool   getValidityImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo)    const = 0;
        virtual double getUncertaintyImpl(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const = 0;

        // Helper methods
        virtual bool getValidBool(const double validity) const;
        virtual double getSplitFactor(const xAOD::Jet& jet) const;
        virtual double getMassOverPt(const xAOD::Jet& jet, const CompMassDef::TypeEnum massDef) const;

    private:
        UncertaintyComponent(const std::string& name = "");
};

// Sorting operators
bool operator <  (const UncertaintyComponent& componentA, const UncertaintyComponent& componentB);
bool operator == (const UncertaintyComponent& componentA, const UncertaintyComponent& componentB);

} // end jet namespace


#endif
