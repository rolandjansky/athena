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

namespace jet
{

class ResolutionHelper : public asg::AsgMessaging
{
    public:
        ResolutionHelper(std::string name = "ResolutionHelper");
        ResolutionHelper(const ResolutionHelper& toCopy);
        virtual StatusCode initialize(TEnv& settings);
        virtual ~ResolutionHelper();

        // Information retrieval
        std::pair<const UncertaintyHistogram*,CompParametrization::TypeEnum> getNominalResolution(const CompScaleVar::TypeEnum smearType, const JetTopology::TypeEnum topology, const bool readMC) const;

    private:
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

        std::map<JetTopology::TypeEnum,UncertaintyHistogram*> m_mNomHistData;
        std::map<JetTopology::TypeEnum,CompParametrization::TypeEnum> m_mNomParamData;
        std::map<JetTopology::TypeEnum,UncertaintyHistogram*> m_mNomHistMC;
        std::map<JetTopology::TypeEnum,CompParametrization::TypeEnum> m_mNomParamMC;


};

} // end jet namespace


#endif
