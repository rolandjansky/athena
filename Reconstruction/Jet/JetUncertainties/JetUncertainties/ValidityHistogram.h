/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_VALIDITYHISTOGRAM_H
#define JETUNCERTAINTIES_VALIDITYHISTOGRAM_H

#include "xAODJet/Jet.h"

#include "JetUncertainties/UncertaintyHistogram.h"
#include "JetUncertainties/UncertaintyEnum.h"

namespace jet
{

class InfoHelper;

class ValidityHistogram : public UncertaintyHistogram
{
    public:
        // Constructor/destructor/initialization
        ValidityHistogram(const std::string& histName, const CompParametrization::TypeEnum parametrization, const float energyScale, const CompMassDef::TypeEnum massDef);
        ValidityHistogram(const TString& histName, const CompParametrization::TypeEnum parametrization, const float energyScale, const CompMassDef::TypeEnum massDef);
        ValidityHistogram(const char* histName, const CompParametrization::TypeEnum parametrization, const float energyScale, const CompMassDef::TypeEnum massDef);
        ValidityHistogram(const ValidityHistogram& toCopy);
        virtual ~ValidityHistogram();
        virtual StatusCode initialize(TFile* histFile);

        // Member retrieval methods
        CompParametrization::TypeEnum getParam() const { return m_param; }

        // Validity information access
        bool getValidity(const xAOD::Jet& jet) const;

    private:
        // Private members
        bool m_isInit;
        const CompParametrization::TypeEnum m_param;
        const float m_energyScale;
        const CompMassDef::TypeEnum m_massDef;
        InfoHelper* m_helper;
        
};

} // end jet namespace




#endif

