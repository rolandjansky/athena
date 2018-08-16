/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef JETUNCERTAINTIES_RESOLUTIONHELPER_H
#define JETUNCERTAINTIES_RESOLUTIONHELPER_H

#include "JetUncertainties/UncertaintyEnum.h"
#include "JetUncertainties/UncertaintyHistogram.h"

#include "AsgTools/AsgMessaging.h"

#include <map>

class TEnv;
class TFile;

namespace jet
{

class ResolutionHelper : public asg::AsgMessaging
{
    public:
        ResolutionHelper(const std::string& name, const std::string& jetDef);
        ResolutionHelper(const ResolutionHelper& toCopy);
        virtual StatusCode initialize(TEnv& settings, TFile* histFile);
        virtual ~ResolutionHelper();

        // Information retrieval
        std::pair<const UncertaintyHistogram*,CompParametrization::TypeEnum> getNominalResolution(const CompScaleVar::TypeEnum smearType, const JetTopology::TypeEnum topology, const bool readMC) const;

    private:
        std::string m_name;
        std::string m_jetDef;
        bool m_isInit;

        // Nominal resolution histograms
        UncertaintyHistogram* m_ptNomHistData;
        CompParametrization::TypeEnum m_ptNomParamData;
        UncertaintyHistogram* m_ptNomHistMC;
        CompParametrization::TypeEnum m_ptNomParamMC;
        
        UncertaintyHistogram* m_fvNomHistData;
        CompParametrization::TypeEnum m_fvNomParamData;
        UncertaintyHistogram* m_fvNomHistMC;
        CompParametrization::TypeEnum m_fvNomParamMC;

        UncertaintyHistogram* m_mQCDNomHistData;
        CompParametrization::TypeEnum m_mQCDNomParamData;
        UncertaintyHistogram* m_mQCDNomHistMC;
        CompParametrization::TypeEnum m_mQCDNomParamMC;

        UncertaintyHistogram* m_mWZNomHistData;
        CompParametrization::TypeEnum m_mWZNomParamData;
        UncertaintyHistogram* m_mWZNomHistMC;
        CompParametrization::TypeEnum m_mWZNomParamMC;

        UncertaintyHistogram* m_mHbbNomHistData;
        CompParametrization::TypeEnum m_mHbbNomParamData;
        UncertaintyHistogram* m_mHbbNomHistMC;
        CompParametrization::TypeEnum m_mHbbNomParamMC;

        UncertaintyHistogram* m_mTopNomHistData;
        CompParametrization::TypeEnum m_mTopNomParamData;
        UncertaintyHistogram* m_mTopNomHistMC;
        CompParametrization::TypeEnum m_mTopNomParamMC;

        // Helper methods
        StatusCode parseInput(TEnv& settings, TFile* histFile, const TString& key, const TString& defaultValue, UncertaintyHistogram* hist, CompParametrization::TypeEnum& param);

};

} // end jet namespace


#endif
