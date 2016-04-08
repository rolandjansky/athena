/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/UncertaintyComponentGroup.h"
#include "JetUncertainties/Helpers.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

UncertaintyComponentGroup::UncertaintyComponentGroup(const std::string& name)
    : UncertaintyComponent(ComponentHelper(name))
    , m_components()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

UncertaintyComponentGroup::UncertaintyComponentGroup(const ComponentHelper& component)
    : UncertaintyComponent(component)
    , m_components()
{
    ATH_MSG_DEBUG("Creating UncertaintyComponentGroup named " << m_name.Data());
}

UncertaintyComponentGroup::UncertaintyComponentGroup(const UncertaintyComponentGroup& toCopy)
    : UncertaintyComponent(toCopy)
    , m_components()
{
    for (size_t iComp = 0; iComp < toCopy.m_components.size(); ++iComp)
        m_components.push_back(m_components.at(iComp)->clone());
}

UncertaintyComponentGroup::~UncertaintyComponentGroup()
{
    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        JESUNC_SAFE_DELETE(m_components.at(iComp));
    m_components.clear();
}

UncertaintyComponentGroup* UncertaintyComponentGroup::clone() const
{
    return new UncertaintyComponentGroup(*this);
}

StatusCode UncertaintyComponentGroup::addComponent(UncertaintyComponent* component)
{
    // Check for initialization
    if (m_isInit)
    {
        ATH_MSG_ERROR("Cannot add components after initialization");
        return StatusCode::FAILURE;
    }

    // Ensure the group number matches
    if (component->getGroupNum() != getGroupNum())
    {
        ATH_MSG_ERROR(Form("Group numbers do not match when adding component %s to group %s",component->getName().Data(),m_name.Data()));
        return StatusCode::FAILURE;
    }

    // Ensure the variable being scaled matches
    if (component->getScaleVar() != getScaleVar())
    {
        ATH_MSG_ERROR(Form("The variable to scale does not match when adding component %s to group %s",component->getName().Data(),m_name.Data()));
        return StatusCode::FAILURE;
    }

    m_components.push_back(component);
    return StatusCode::SUCCESS;
}

StatusCode UncertaintyComponentGroup::initialize()
{
    // Ensure correlation was specified
    if (m_corrType == CompCorrelation::UNKNOWN)
    {
        ATH_MSG_ERROR(Form("Didn't specify the correlation for group %s",m_name.Data()));
        return StatusCode::FAILURE;
    }

    m_isInit = true;
    return StatusCode::SUCCESS;
}

StatusCode UncertaintyComponentGroup::initialize(const std::vector<TString>&, TFile*)
{
    // Function does not apply for groups!
    ATH_MSG_ERROR("Method signature is blocked as it is inappropriate for groups");
    return StatusCode::FAILURE;
}

StatusCode UncertaintyComponentGroup::initialize(const std::vector<TString>&, const std::vector<TString>&, TFile*)
{
    // Function does not apply for groups!
    ATH_MSG_ERROR("Method signature is blocked as it is inappropriate for groups");
    return StatusCode::FAILURE;
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool UncertaintyComponentGroup::getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    // Valid only if all component(s) are valid
    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        if (!m_components.at(iComp)->getValidity(jet,eInfo))
            return false;
    return true;
}

bool UncertaintyComponentGroup::getValidity(const UncertaintyHistogram*, const xAOD::Jet&, const xAOD::EventInfo&) const
{
    ATH_MSG_ERROR("Blocked - this method doesn't make sense for component groups like " << m_name.Data());
    return false;
}

double UncertaintyComponentGroup::getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    double unc = 0;

    // Check for groups with only one component
    // In this case, just use the uncertainty itself
    // This preserves correlation info, while the below might discard it: sqrt(unc^2) throws away sign
    if (m_components.size() == 1)
        unc = m_components.at(0)->getUncertainty(jet,eInfo);
    else
    {
        if (m_corrType == CompCorrelation::Uncorrelated)
        {
            for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
                unc += pow(m_components.at(iComp)->getUncertainty(jet,eInfo),2);
            unc = sqrt(unc);
            //for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
            //{
            //    const double compUnc = m_components.at(iComp)->getUncertainty(jet,eInfo);
            //    unc += fabs(compUnc)*compUnc;
            //}
            //unc = (unc<0?-1:1)*sqrt(fabs(unc));
        }
        else
            for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
                unc += m_components.at(iComp)->getUncertainty(jet,eInfo);
    }

    return unc;
}

double UncertaintyComponentGroup::getUncertainty(const UncertaintyHistogram*, const xAOD::Jet&, const xAOD::EventInfo&) const
{
    ATH_MSG_ERROR("Blocked - this method doesn't make sense for component groups like " << m_name.Data());
    return JESUNC_ERROR_CODE;
}

bool UncertaintyComponentGroup::getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    const bool isValid = getValidity(jet,eInfo);
    if (isValid)
        unc = getUncertainty(jet,eInfo);
    return isValid;
}

bool UncertaintyComponentGroup::getValidUncertainty(const UncertaintyHistogram*, double&, const xAOD::Jet&, const xAOD::EventInfo&) const
{
    ATH_MSG_ERROR("Blocked - this method doesn't make sense for component groups like " << m_name.Data());
    return false;
}


} // end jet namespace

