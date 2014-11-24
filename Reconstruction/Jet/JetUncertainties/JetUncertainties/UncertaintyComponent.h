/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_UNCERTAINTYCOMPONENT_H
#define JETUNCERTAINTIES_UNCERTAINTYCOMPONENT_H

#include "JetUncertainties/UncertaintyEnum.h"
#include "JetUncertainties/UncertaintyHistogram.h"
#include "JetUncertainties/ComponentHelper.h"

#include "xAODJet/Jet.h"
#include "xAODEventInfo/EventInfo.h"

#include "AsgTools/AsgMessaging.h"

#include "TString.h"

class TFile;

namespace jet
{

class UncertaintyComponent : public asg::AsgMessaging
{
    public:
        // Constructor/destructor/initialization
        UncertaintyComponent(const ComponentHelper& component);
        UncertaintyComponent(const UncertaintyComponent& toCopy);
        virtual UncertaintyComponent* clone() const = 0;
        virtual ~UncertaintyComponent();
        virtual StatusCode SetSplitFactor(const int splitCompNumber);
        virtual StatusCode Initialize(const std::vector<TString>& histNames, TFile* histFile);
        virtual StatusCode Initialize(const std::vector<TString>& histNames, const std::vector<TString>& validHistNames, TFile* histFile);

        // Information retrieval methods
        virtual TString                 getName()       const   { return m_name; }
        virtual TString                 getDesc()       const   { return m_desc; }
        virtual CompCategory::TypeEnum  getCategory()   const   { return m_category; }
        virtual CompScaleVar::TypeEnum  getScaleVar()   const   { return m_scaleVar; }

        // Uncertainty retrieval methods (wrappers)
        virtual bool   getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual double getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;
        virtual bool   getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const;


    protected:
        // Protected members
        bool  m_isInit;
        const TString m_name;
        const TString m_desc;
        const CompCategory::TypeEnum m_category;
        const CompCorrelation::TypeEnum m_corrType;
        const CompScaleVar::TypeEnum m_scaleVar;
        const bool m_interpolate;
        int m_splitNumber;
        std::vector<UncertaintyHistogram*> m_histos;

        // Uncertainty retrieval helper methods (pure abstract)
        virtual bool   getValidity(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const = 0;
        virtual double getUncertainty(const UncertaintyHistogram* histo, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const = 0;
        virtual bool   getValidUncertainty(const UncertaintyHistogram* histo, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const = 0;


        // Split factor methods
        virtual double getSplitFactorLinear(const UncertaintyHistogram* histo, const xAOD::Jet& jet) const;

    private:
        UncertaintyComponent(const std::string& name = "");
        
};

// Sorting operators
bool operator <  (const UncertaintyComponent& compA, const UncertaintyComponent& compB);
bool operator == (const UncertaintyComponent& compA, const UncertaintyComponent& compB);

} // end jet namespace


#endif
