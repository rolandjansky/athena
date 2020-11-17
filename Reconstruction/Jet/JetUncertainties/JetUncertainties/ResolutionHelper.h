/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_RESOLUTIONHELPER_H
#define JETUNCERTAINTIES_RESOLUTIONHELPER_H

#include "JetUncertainties/UncertaintyEnum.h"
#include "JetUncertainties/UncertaintyHistogram.h"

#include "AsgMessaging/AsgMessaging.h"

#include <tuple>

class TEnv;
class TFile;

namespace jet
{

class ResolutionHelper : public asg::AsgMessaging
{
    public:
        ResolutionHelper(const std::string& name, const std::string& jetDef);
        ResolutionHelper(const ResolutionHelper& toCopy);
        virtual StatusCode initialize(TEnv& settings, TFile* histFile, const TString& MCtype);
        virtual ~ResolutionHelper();

        // Information retrieval
        std::tuple<const UncertaintyHistogram*,CompParametrization::TypeEnum,CompMassDef::TypeEnum> getNominalResolution(const CompScaleVar::TypeEnum smearType, const JetTopology::TypeEnum topology, const bool readMC) const;

        bool smearOnlyMC()    const { return  m_smearOnlyMC; }
        bool smearDataAndMC() const { return !m_smearOnlyMC; }

        bool hasRelevantInfo(const CompScaleVar::TypeEnum type, const JetTopology::TypeEnum topology) const;

    private:
        std::string m_name;
        std::string m_jetDef;
        bool m_isInit;

        bool m_smearOnlyMC;

        // Nominal resolution histograms
        UncertaintyHistogram* m_ptNomHistData;
        CompParametrization::TypeEnum m_ptNomParamData;
        CompMassDef::TypeEnum m_ptNomMassDefData;
        UncertaintyHistogram* m_ptNomHistMC;
        CompParametrization::TypeEnum m_ptNomParamMC;
        CompMassDef::TypeEnum m_ptNomMassDefMC;
        
        UncertaintyHistogram* m_fvNomHistData;
        CompParametrization::TypeEnum m_fvNomParamData;
        CompMassDef::TypeEnum m_fvNomMassDefData;
        UncertaintyHistogram* m_fvNomHistMC;
        CompParametrization::TypeEnum m_fvNomParamMC;
        CompMassDef::TypeEnum m_fvNomMassDefMC;

        UncertaintyHistogram* m_mQCDNomHistData;
        CompParametrization::TypeEnum m_mQCDNomParamData;
        CompMassDef::TypeEnum m_mQCDNomMassDefData;
        UncertaintyHistogram* m_mQCDNomHistMC;
        CompParametrization::TypeEnum m_mQCDNomParamMC;
        CompMassDef::TypeEnum m_mQCDNomMassDefMC;

        UncertaintyHistogram* m_mWZNomHistData;
        CompParametrization::TypeEnum m_mWZNomParamData;
        CompMassDef::TypeEnum m_mWZNomMassDefData;
        UncertaintyHistogram* m_mWZNomHistMC;
        CompParametrization::TypeEnum m_mWZNomParamMC;
        CompMassDef::TypeEnum m_mWZNomMassDefMC;

        UncertaintyHistogram* m_mHbbNomHistData;
        CompParametrization::TypeEnum m_mHbbNomParamData;
        CompMassDef::TypeEnum m_mHbbNomMassDefData;
        UncertaintyHistogram* m_mHbbNomHistMC;
        CompParametrization::TypeEnum m_mHbbNomParamMC;
        CompMassDef::TypeEnum m_mHbbNomMassDefMC;

        UncertaintyHistogram* m_mTopNomHistData;
        CompParametrization::TypeEnum m_mTopNomParamData;
        CompMassDef::TypeEnum m_mTopNomMassDefData;
        UncertaintyHistogram* m_mTopNomHistMC;
        CompParametrization::TypeEnum m_mTopNomParamMC;
        CompMassDef::TypeEnum m_mTopNomMassDefMC;

        // Helper methods
        StatusCode parseInput(TEnv& settings, TFile* histFile, const TString& key, const TString& defaultValue, UncertaintyHistogram*& hist, CompParametrization::TypeEnum& param, CompMassDef::TypeEnum& massDef,const TString& MCtype);

};

} // end jet namespace


#endif
