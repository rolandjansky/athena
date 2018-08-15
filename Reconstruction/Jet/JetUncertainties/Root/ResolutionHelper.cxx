/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUncertainties/ResolutionHelper.h"
#include "JetUncertainties/Helpers.h"

#include "TEnv.h"

namespace jet
{

ResolutionHelper::ResolutionHelper(std::string name)
    : asg::AsgMessaging(name)
    , m_isInit(false)
    , m_ptNomHistData(NULL)
    , m_ptNomParamData(CompParametrization::UNKNOWN)
    , m_ptNomHistMC(NULL)
    , m_ptNomParamMC(CompParametrization::UNKNOWN)
    , m_fvNomHistData(NULL)
    , m_fvNomParamData(CompParametrization::UNKNOWN)
    , m_fvNomHistMC(NULL)
    , m_fvNomParamMC(CompParametrization::UNKNOWN)
    , m_mNomHistData()
    , m_mNomParamData()
    , m_mNomHistMC()
    , m_mNomParamMC()
{ }

ResolutionHelper::ResolutionHelper(const ResolutionHelper& toCopy)
    : asg::AsgMessaging("copied")
    , m_isInit(toCopy.m_isInit)
    , m_ptNomHistData(!toCopy.m_ptNomHistData ? NULL : new UncertaintyHistogram(*toCopy.m_ptNomHistData))
    , m_ptNomParamData(toCopy.m_ptNomParamData)
    , m_ptNomHistMC(!toCopy.m_ptNomHistMC ? NULL : new UncertaintyHistogram(*toCopy.m_ptNomHistMC))
    , m_ptNomParamMC(toCopy.m_ptNomParamMC)
    , m_fvNomHistData(!toCopy.m_fvNomHistData ? NULL : new UncertaintyHistogram(*toCopy.m_fvNomHistData))
    , m_fvNomParamData(toCopy.m_fvNomParamData)
    , m_fvNomHistMC(!toCopy.m_fvNomHistMC ? NULL : new UncertaintyHistogram(*toCopy.m_fvNomHistMC))
    , m_fvNomParamMC(toCopy.m_fvNomParamMC)
    , m_mNomHistData()
    , m_mNomParamData(toCopy.m_mNomParamData)
    , m_mNomHistMC()
    , m_mNomParamMC(toCopy.m_mNomParamMC)
{
    std::map<JetTopology::TypeEnum, UncertaintyHistogram*>::const_iterator iter;
    for (iter = toCopy.m_mNomHistData.begin(); iter != toCopy.m_mNomHistData.end(); ++iter)
    {
        if (iter->second)
            m_mNomHistData.insert(std::pair<JetTopology::TypeEnum,UncertaintyHistogram*>(iter->first,new UncertaintyHistogram(*(iter->second))));
        else
            m_mNomHistData.insert(std::pair<JetTopology::TypeEnum,UncertaintyHistogram*>(iter->first,NULL));
    }
    
    for (iter = toCopy.m_mNomHistMC.begin(); iter != toCopy.m_mNomHistMC.end(); ++iter)
    {
        if (iter->second)
            m_mNomHistMC.insert(std::pair<JetTopology::TypeEnum,UncertaintyHistogram*>(iter->first,new UncertaintyHistogram(*(iter->second))));
        else
            m_mNomHistMC.insert(std::pair<JetTopology::TypeEnum,UncertaintyHistogram*>(iter->first,NULL));
    }
}

ResolutionHelper::~ResolutionHelper()
{
    JESUNC_SAFE_DELETE(m_ptNomHistData);
    JESUNC_SAFE_DELETE(m_ptNomHistMC);
    JESUNC_SAFE_DELETE(m_fvNomHistData);
    JESUNC_SAFE_DELETE(m_fvNomHistMC);

    std::map<JetTopology::TypeEnum, UncertaintyHistogram*>::iterator iter;
    for (iter = m_mNomHistData.begin(); iter != m_mNomHistData.end(); ++iter)
        JESUNC_SAFE_DELETE(iter->second);
    m_mNomHistData.clear();
    
    for (iter = m_mNomHistMC.begin(); iter != m_mNomHistMC.end(); ++iter)
        JESUNC_SAFE_DELETE(iter->second);
    m_mNomHistMC.clear();
}

StatusCode ResolutionHelper::initialize(TEnv& settings)
{
    // TODO
    return StatusCode::SUCCESS;
}

std::pair<const UncertaintyHistogram*,CompParametrization::TypeEnum> ResolutionHelper::getNominalResolution(const CompScaleVar::TypeEnum smearType, const JetTopology::TypeEnum topology, const bool readMC) const
{
    // First get the resolution histogram and parametrization
    const jet::UncertaintyHistogram* resolution = NULL;
    CompParametrization::TypeEnum param         = CompParametrization::UNKNOWN;
    
    switch (smearType)
    {
        case CompScaleVar::MassRes:
        case CompScaleVar::MassResAbs:
            if (topology == JetTopology::UNKNOWN)
            {
                // We shouldn't read this, as it was checked at a higher level
                // Just to be safe, check it and return error code
                ATH_MSG_ERROR("Mass resolution depends on the jet topology, which was not specified");
                return std::pair<UncertaintyHistogram*,CompParametrization::TypeEnum>(NULL,CompParametrization::UNKNOWN);
            }
            if (topology == JetTopology::MIXED)
            {
                // We shouldn't read this, as it was checked at a higher level
                // Just to be safe, check it and return error code
                ATH_MSG_ERROR("Mass resolution depends on a single jet topology, not a mixed topology");
                return std::pair<UncertaintyHistogram*,CompParametrization::TypeEnum>(NULL,CompParametrization::UNKNOWN);
            }

            if (readMC)
            {
                std::map<JetTopology::TypeEnum,UncertaintyHistogram*>::const_iterator iterHist = m_mNomHistMC.find(topology);
                if (iterHist != m_mNomHistMC.end())
                    resolution = iterHist->second;
                
                std::map<JetTopology::TypeEnum,CompParametrization::TypeEnum>::const_iterator iterParam = m_mNomParamMC.find(topology);
                if (iterParam != m_mNomParamMC.end())
                    param = iterParam->second;
            }
            else
            {
                std::map<JetTopology::TypeEnum,UncertaintyHistogram*>::const_iterator iterHist = m_mNomHistData.find(topology);
                if (iterHist != m_mNomHistData.end())
                    resolution = iterHist->second;
                
                std::map<JetTopology::TypeEnum,CompParametrization::TypeEnum>::const_iterator iterParam = m_mNomParamData.find(topology);
                if (iterParam != m_mNomParamData.end())
                    param = iterParam->second;
            }
            break;

        case CompScaleVar::PtRes:
        case CompScaleVar::PtResAbs:
            if (readMC)
            {
                resolution = m_ptNomHistMC;
                param      = m_ptNomParamMC;
            }
            else
            {
                resolution = m_ptNomHistData;
                param      = m_ptNomParamData;
            }
            break;
        
        case CompScaleVar::FourVecRes:
        case CompScaleVar::FourVecResAbs:
            if (readMC)
            {
                resolution = m_fvNomHistMC;
                param      = m_fvNomParamMC;
            }
            else
            {
                resolution = m_fvNomHistData;
                param      = m_fvNomParamData;
            }
            break;

        default:
            // This is not a resolution uncertainty component
            // We should not get here
            // Print an erorr and return error code
            ATH_MSG_ERROR("Asked for the smearing factor for a non-resolution component");
            return std::pair<UncertaintyHistogram*,CompParametrization::TypeEnum>(NULL,CompParametrization::UNKNOWN);
    }
    return std::pair<const UncertaintyHistogram*,CompParametrization::TypeEnum>(resolution,param);
}

} // end jet namespace

