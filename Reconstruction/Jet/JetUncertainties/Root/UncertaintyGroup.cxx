/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/UncertaintyGroup.h"
#include "JetUncertainties/Helpers.h"

#include "TFile.h"

namespace jet
{

bool operator <  (const UncertaintyGroup& groupA, const UncertaintyGroup& groupB)
{
    return groupA.getName() < groupB.getName();
}

bool operator == (const UncertaintyGroup& groupA, const UncertaintyGroup& groupB)
{
    return groupA.getName() == groupB.getName();
}

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

UncertaintyGroup::UncertaintyGroup(const std::string& name)
    : asg::AsgMessaging(name)
    , m_isInit(false)
    , m_name(name.c_str())
    , m_desc("")
    , m_groupNum(0)
    , m_subgroupNum(0)
    , m_category(CompCategory::UNKNOWN)
    , m_corrType(CompCorrelation::UNKNOWN)
    , m_isReducible(false)
    , m_components()
    , m_subgroups()
    , m_compScaleMap()
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

UncertaintyGroup::UncertaintyGroup(const GroupHelper& group)
    : asg::AsgMessaging(group.name.Data())
    , m_isInit(false)
    , m_name(group.name)
    , m_desc(group.desc)
    , m_groupNum(group.groupNum)
    , m_subgroupNum(group.subgroupNum)
    , m_category(group.category)
    , m_corrType(group.correlation)
    , m_isReducible(group.reducible)
    , m_components()
    , m_subgroups()
    , m_compScaleMap()
{
    ATH_MSG_DEBUG(Form("Creating UncertaintyGroup named %s",m_name.Data()));
    //if (m_category == CompCategory::UNKNOWN)
    //    ATH_MSG_FATAL("UNKNOWN category for UncertaintyGroup named " << m_name.Data());
    // Acceptible to not exist if it's only one component, check during initialize()
    //if (m_corrType == CompCorrelation::UNKNOWN)
    //    ATH_MSG_FATAL(Form("UNKNOWN correlation for UncertaintyGroup named %s",m_name.Data()));
}

UncertaintyGroup::UncertaintyGroup(const UncertaintyGroup& toCopy)
    : asg::AsgMessaging(Form("%s_copy",toCopy.m_name.Data()))
    , m_isInit(toCopy.m_isInit)
    , m_name(toCopy.m_name)
    , m_desc(toCopy.m_desc)
    , m_groupNum(toCopy.m_groupNum)
    , m_subgroupNum(toCopy.m_subgroupNum)
    , m_category(toCopy.m_category)
    , m_corrType(toCopy.m_corrType)
    , m_isReducible(toCopy.m_isReducible)
    , m_components()
    , m_subgroups()
    , m_compScaleMap()
{
    for (size_t iComp = 0; iComp < toCopy.m_components.size(); ++iComp)
        if (toCopy.m_components.at(iComp))
            m_components.push_back(toCopy.m_components.at(iComp)->clone());
}

UncertaintyGroup::~UncertaintyGroup()
{
    ATH_MSG_DEBUG(Form("Deleting UncertaintyGroup named %s",m_name.Data()));
    
    std::map<CompScaleVar::TypeEnum,std::vector<UncertaintyComponent*>*>::iterator iter;
    for (iter = m_compScaleMap.begin(); iter != m_compScaleMap.end(); ++iter)
        JESUNC_SAFE_DELETE(iter->second);
    m_compScaleMap.clear();

    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        JESUNC_SAFE_DELETE(m_components.at(iComp));
    m_components.clear();

    for (size_t iSubGroup = 0; iSubGroup < m_subgroups.size(); ++iSubGroup)
        JESUNC_SAFE_DELETE(m_subgroups.at(iSubGroup));
    m_subgroups.clear();
}

StatusCode UncertaintyGroup::addComponent(UncertaintyComponent* component)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("Cannot add components to a group after initialization: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Ensure the component is not NULL
    if (!component)
    {
        ATH_MSG_ERROR("Component specified for addition to the group is NULL: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Add it to the vector of components
    m_components.push_back(component);

    // Now add it to the map with its scale variable
    std::map<CompScaleVar::TypeEnum,std::vector<UncertaintyComponent*>*>::iterator iter = m_compScaleMap.find(component->getScaleVar());
    if (iter != m_compScaleMap.end())
        iter->second->push_back(component);
    else
    {
        std::vector<UncertaintyComponent*>* newScaleVarVec = new std::vector<UncertaintyComponent*>();
        newScaleVarVec->push_back(component);
        m_compScaleMap.insert(std::make_pair(component->getScaleVar(),newScaleVarVec));
    }

    return StatusCode::SUCCESS;
}

StatusCode UncertaintyGroup::addSubgroup(UncertaintyGroup* subgroup)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("Cannot add subgroups to a group after initialization: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Ensure the subgroup is not NULL
    if (!subgroup)
    {
        ATH_MSG_ERROR("Subgroup specified for addition to the group is NULL: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Add it to the vector of subgroups
    m_subgroups.push_back(subgroup);

    return StatusCode::SUCCESS;
}

StatusCode UncertaintyGroup::initialize(TFile* histFile)
{
    // Prevent double-initialization
    if (m_isInit)
    {
        ATH_MSG_ERROR("Group is already initialized: " << getName().Data());
        return StatusCode::FAILURE;
    }
    
    // Ensure the group has component(s) and/or subgroup(s)
    if (!m_components.size() && !m_subgroups.size())
    {
        ATH_MSG_ERROR("Group is empty: " << getName().Data());
        return StatusCode::FAILURE;
    }

    // Initialize the subgroups (has to happen before we check parametrizations below)
    for (size_t iSubgroup = 0; iSubgroup < m_subgroups.size(); ++iSubgroup)
        if(m_subgroups.at(iSubgroup)->initialize(histFile).isFailure())
            return StatusCode::FAILURE;

    // If the group has multiple components of the same parametrization, ensure there is a correlation type specified
    if (m_corrType == CompCorrelation::UNKNOWN && m_components.size() >= 1 && m_subgroups.size() >= 1)
    {
        // First check the raw components
        std::set<CompScaleVar::TypeEnum> vars;
        for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
            vars.insert(m_components.at(iComp)->getScaleVar());
        if (vars.size() != m_components.size())
        {
            ATH_MSG_ERROR("Correlation type is set to UNKNOWN for multiple components scaling the same variable: " << getName().Data());
            return StatusCode::FAILURE;
        }

        // Now check the subgroup components
        std::set<CompScaleVar::TypeEnum> groupVars;
        size_t numSubComps = 0;
        for (size_t iSubgroup = 0; iSubgroup < m_subgroups.size(); ++iSubgroup)
        {
            const std::set<CompScaleVar::TypeEnum> subVars = m_subgroups.at(iSubgroup)->getScaleVars();
            groupVars.insert(subVars.begin(),subVars.end());
            numSubComps += m_subgroups.at(iSubgroup)->getNumComponents();
        }
        if (groupVars.size() != numSubComps)
        {
            ATH_MSG_ERROR("Correlation type is set to UNKNOWN for multiple subgroups scaling the same variable: " << getName().Data());
            return StatusCode::FAILURE;
        }

        // Now check if any of the raw component scales overlap with subgroup scales
        for (std::set<CompScaleVar::TypeEnum>::const_iterator iter = vars.begin(); iter != vars.end(); ++iter)
        {
            if (groupVars.count(*iter))
            {
                ATH_MSG_ERROR("Correlation type is set to UNKNOWN for a component and subgroup scaling the same variable: " << getName().Data());
                return StatusCode::FAILURE;
            }
        }
    }


    // Initialize the components
    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        if (m_components.at(iComp)->initialize(histFile).isFailure())
            return StatusCode::FAILURE;

    // Print a summary
    ATH_MSG_DEBUG("Successfully initialized UncertaintyGroup named " << getName().Data());

    m_isInit = true;
    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Methods to access underlying constituents   //
//                                              //
//////////////////////////////////////////////////

size_t UncertaintyGroup::getNumComponents() const
{
    size_t numComps = m_components.size();
    for (size_t iSubgroup = 0; iSubgroup < m_subgroups.size(); ++iSubgroup)
        numComps += m_subgroups.at(iSubgroup)->getNumComponents();

    return numComps;
}

std::set<CompScaleVar::TypeEnum> UncertaintyGroup::getScaleVars() const
{
    std::set<CompScaleVar::TypeEnum> vars;
    if (!m_isInit)
    {
        ATH_MSG_ERROR("Cannot call method before initialization, group: " << getName().Data());
        return vars;
    }

    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        vars.insert(m_components.at(iComp)->getScaleVar());

    for (size_t iSubgroup = 0; iSubgroup < m_subgroups.size(); ++iSubgroup)
    {
        const std::set<CompScaleVar::TypeEnum> subVars = m_subgroups.at(iSubgroup)->getScaleVars();
        vars.insert(subVars.begin(),subVars.end());
    }

    return vars;
}


//////////////////////////////////////////////////
//                                              //
//  Methods to test for special cases           //
//                                              //
//////////////////////////////////////////////////

bool UncertaintyGroup::isAlwaysZero() const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("Cannot call method before initialization, group: "<<getName().Data());
        return false;
    }

    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
    {
        if (!m_components.at(iComp)->isAlwaysZero())
            return false;
    }
    
    for (size_t iSubgroup = 0; iSubgroup < m_subgroups.size(); ++iSubgroup)
    {
        if (!m_subgroups.at(iSubgroup)->isAlwaysZero())
            return false;
    }

    return true;
}


//////////////////////////////////////////////////
//                                              //
//  Validity and uncertainty retrieval          //
//                                              //
//////////////////////////////////////////////////

bool UncertaintyGroup::getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const
{
    // Search for the component in question
    std::map<CompScaleVar::TypeEnum,std::vector<UncertaintyComponent*>*>::const_iterator iter = m_compScaleMap.find(scaleVar);
    if (iter == m_compScaleMap.end())
        return true;
    const std::vector<UncertaintyComponent*>* relevantComponents = iter->second;
    
    // Valid only if all component(s) are valid
    if (!m_isInit)
    {
        ATH_MSG_ERROR("Group hasn't been initialized: " << getName().Data());
        return false;
    }
    for (size_t iComp = 0; iComp < relevantComponents->size(); ++iComp)
        if (!relevantComponents->at(iComp)->getValidity(jet,eInfo))
            return false;

    // Valid only if all subgroup(s) are valid
    for (size_t iSubgroup = 0; iSubgroup < m_subgroups.size(); ++iSubgroup)
        if (!m_subgroups.at(iSubgroup)->getValidity(jet,eInfo,scaleVar))
            return false;

    return true;
}


double UncertaintyGroup::getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const
{
    // Search for the component in question
    std::map<CompScaleVar::TypeEnum,std::vector<UncertaintyComponent*>*>::const_iterator iter = m_compScaleMap.find(scaleVar);
    const std::vector<UncertaintyComponent*>* relevantComponents = (iter != m_compScaleMap.end()) ? iter->second : NULL;

    // Also check if any subgroup(s) are affected
    std::vector<UncertaintyGroup*> relevantGroups;
    for (size_t iSubgroup = 0; iSubgroup < m_subgroups.size(); ++iSubgroup)
        if (m_subgroups.at(iSubgroup)->getScaleVars().count(scaleVar))
            relevantGroups.push_back(m_subgroups.at(iSubgroup));

    // Simple if it neither affects components nor subgroups
    if ((!relevantComponents || !relevantComponents->size()) && !relevantGroups.size()) return 0;

    // Simple if it's one component and doesn't affect subgroups
    if ((relevantComponents && relevantComponents->size() == 1) && !relevantGroups.size())
        return relevantComponents->at(0)->getUncertainty(jet,eInfo);

    // Simple if it's one subgroup and doesn't affect components
    if (relevantGroups.size() == 1 && (!relevantComponents || !relevantComponents->size()))
        return relevantGroups.at(0)->getUncertainty(jet,eInfo,scaleVar);


    // More complicated case where there are multiple component(s) and/or group(s)
    // Quad sum or linear sum depending on correlation type specified
    double unc = 0;
    if (m_corrType == CompCorrelation::Uncorrelated)
    {
        if (relevantComponents)
            for (size_t iComp = 0; iComp < relevantComponents->size(); ++iComp)
                unc += pow(relevantComponents->at(iComp)->getUncertainty(jet,eInfo),2);
        for (size_t iGroup = 0; iGroup < relevantGroups.size(); ++iGroup)
            unc += pow(relevantGroups.at(iGroup)->getUncertainty(jet,eInfo,scaleVar),2);
        unc = sqrt(unc);
    }
    else if (m_corrType == CompCorrelation::Correlated)
    {
        if (relevantComponents)
            for (size_t iComp = 0; iComp < relevantComponents->size(); ++iComp)
                unc += relevantComponents->at(iComp)->getUncertainty(jet,eInfo);
        for (size_t iGroup = 0; iGroup < relevantGroups.size(); ++iGroup)
            unc += relevantGroups.at(iGroup)->getUncertainty(jet,eInfo,scaleVar);
    }
    else
    {
        ATH_MSG_ERROR(Form("Unexpected correlation type for %s",m_name.Data()));
        return JESUNC_ERROR_CODE;
    }

    return unc;
}

bool UncertaintyGroup::getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const
{
    if (getValidity(jet,eInfo,scaleVar))
    {
        unc = getUncertainty(jet,eInfo,scaleVar);
        return true;
    }
    return false;
}

} // end jet namespace

