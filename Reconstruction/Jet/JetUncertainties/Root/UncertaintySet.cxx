/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUncertainties/UncertaintySet.h"
#include "JetUncertainties/Helpers.h"

#include <set>

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

UncertaintySet::UncertaintySet(const std::string& name)
    : asg::AsgMessaging(name)
    , m_name(name)
    , m_isInit(false)
    , m_groups()
    , m_shiftWeights()
{ }

UncertaintySet::~UncertaintySet()
{ }

StatusCode UncertaintySet::initialize(const CP::SystematicSet& systConfig, const std::vector<UncertaintyGroup*>& groups)
{
    // Ensure it wasn't already initialized
    if (m_isInit)
    {
        ATH_MSG_ERROR("Blocking double-initialization for: " << m_name.c_str());
        return StatusCode::FAILURE;
    }

    // Parse all of the names to get the groups
    std::set<UncertaintyGroup*> compSet;
    for (CP::SystematicSet::const_iterator iter = systConfig.begin(); iter != systConfig.end(); ++iter)
        for (size_t iGroup = 0; iGroup < groups.size(); ++iGroup)
            if (groups.at(iGroup)->getName().CompareTo(iter->basename().c_str()) == 0)
            {
                // Watch for double-specified elements
                if (compSet.count(groups.at(iGroup)))
                {
                    ATH_MSG_ERROR("Multiple independent shifts requested for a single group: " << groups.at(iGroup)->getName().Data());
                    return StatusCode::FAILURE;
                }
                compSet.insert(groups.at(iGroup));
                
                m_groups.push_back(groups.at(iGroup));
                m_shiftWeights.push_back(iter->parameter());
                break;
            }

    
    // Ensure all of the elements were found
    if (systConfig.size() != m_groups.size())
    {
        ATH_MSG_ERROR(Form("Input SystematicSet is %zu elements, but only found %zu UncertaintyGroups",systConfig.size(),m_groups.size()));
        return StatusCode::FAILURE;
    }
    
    // All good
    m_isInit = true;
    return StatusCode::SUCCESS;
}

std::string UncertaintySet::getName() const
{
    return m_name;
}

//////////////////////////////////////////////////
//                                              //
//  Uncertainty/validity retrieval methods      //
//                                              //
//////////////////////////////////////////////////

CompScaleVar::TypeEnum UncertaintySet::getSingleVar() const
{
    std::set<CompScaleVar::TypeEnum> vars = m_groups.at(0)->getScaleVars();
    if (vars.size() != 1)
        return CompScaleVar::UNKNOWN;
    const CompScaleVar::TypeEnum singleVar = *(vars.begin());
    
    for (size_t iGroup = 1; iGroup < m_groups.size(); ++iGroup)
    {
        std::set<CompScaleVar::TypeEnum> vars2 = m_groups.at(iGroup)->getScaleVars();
        if (vars2.size() != 1 || singleVar != *(vars2.begin()))
        {
            return CompScaleVar::UNKNOWN;
        }
    }

    return singleVar;
}

bool UncertaintySet::getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const
{
    if (!m_groups.size())
        return true;

    // If the scale var wasn't specified, need to ensure all groups match
    const bool checkScaleVar = scaleVar == CompScaleVar::UNKNOWN;
    const CompScaleVar::TypeEnum scaleVariable = checkScaleVar ? getSingleVar() : scaleVar;
    if (scaleVariable == CompScaleVar::UNKNOWN)
    {
        ATH_MSG_ERROR("Asked for validity of a multi-scale variable set without specifying any scale variable: " << m_name);
        return false;
    }

    // Checked conditions, now get the validity
    for (size_t iGroup = 0; iGroup < m_groups.size(); ++iGroup)
        if (!m_groups.at(iGroup)->getValidity(jet,eInfo,scaleVariable))
            return false;
    return true;
}

double UncertaintySet::getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const
{
    // This is the sum of shifts multiple uncorrelated groups
    // This could be a positive or negative number
    // Each individual group uncertainty will always be positive (quad sum of subgroups)
    // But the weight may be positive or negative (+N sigma or -N sigma variations)
    
    if (!m_groups.size())
        return 0;

    // If the scale var wasn't specified, need to ensure all groups match
    const bool checkScaleVar = scaleVar == CompScaleVar::UNKNOWN;
    const CompScaleVar::TypeEnum scaleVariable = checkScaleVar ? getSingleVar() : scaleVar;
    if (scaleVariable == CompScaleVar::UNKNOWN)
    {
        ATH_MSG_ERROR("Asked for uncertainty of a multi-scale-variable set without specifying any scale variable: " << m_name);
        return JESUNC_ERROR_CODE;
    }
    
    // Checked conditions, now get the uncertainty
    double unc = 0;
    for (size_t iGroup = 0; iGroup < m_groups.size(); ++iGroup)
    {
        unc += m_groups.at(iGroup)->getUncertainty(jet,eInfo,scaleVariable)*m_shiftWeights.at(iGroup);
    }

    return unc;
}

bool UncertaintySet::getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const
{
    // See getUncertainty for discussion
    // This is not just calling the other two methods for speed, O(n) instead of O(2n)
    
    if (!m_groups.size())
    {
        unc = 0;
        return true;
    }

    // If the scale var wasn't specified, need to ensure all groups match
    const bool checkScaleVar = scaleVar == CompScaleVar::UNKNOWN;
    const CompScaleVar::TypeEnum scaleVariable = checkScaleVar ? getSingleVar() : scaleVar;
    if (scaleVariable == CompScaleVar::UNKNOWN)
    {
        ATH_MSG_ERROR("Asked for valid uncertainty of a multi-scale-variable set without specifying any scale variable: " << m_name);
        return false;
    }
    
    // Checked conditions, now get the uncertainty and validity
    double localUnc = 0;
    for (size_t iGroup = 0; iGroup < m_groups.size(); ++iGroup)
    {
        if (!m_groups.at(iGroup)->getValidity(jet,eInfo,scaleVariable))
            return false;
        localUnc += m_groups.at(iGroup)->getUncertainty(jet,eInfo,scaleVariable)*m_shiftWeights.at(iGroup);
    }
    unc = localUnc;
    return true;
}

//////////////////////////////////////////////////
//                                              //
//  Advanced uncertainty/validity methods       //
//                                              //
//////////////////////////////////////////////////

std::vector<CompScaleVar::TypeEnum> UncertaintySet::getScaleVars() const
{
    // Make a super-set and fill it from each group
    std::set<CompScaleVar::TypeEnum> scaleVarSet;
    for (size_t iGroup = 0; iGroup < m_groups.size(); ++iGroup)
    {
        std::set<CompScaleVar::TypeEnum> localSet = m_groups.at(iGroup)->getScaleVars();
        scaleVarSet.insert(localSet.begin(),localSet.end());
    }

    // Convert to a vector
    std::vector<CompScaleVar::TypeEnum> scaleVars;
    std::set<CompScaleVar::TypeEnum>::const_iterator iter;
    for (iter = scaleVarSet.begin(); iter != scaleVarSet.end(); ++iter)
        scaleVars.push_back(*(iter));
    return scaleVars;
}

std::vector< std::pair<CompScaleVar::TypeEnum,bool> > UncertaintySet::getValiditySet(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    std::vector< std::pair<CompScaleVar::TypeEnum,bool> > validity;

    // Simple case
    if (!m_groups.size())
        return validity;

    // Get the sets
    const std::vector<CompScaleVar::TypeEnum> scaleVars = getScaleVars();

    // Process the sets one by one
    for (size_t iVar = 0; iVar < scaleVars.size(); ++iVar)
        validity.push_back(std::make_pair(scaleVars.at(iVar),getValidity(jet,eInfo,scaleVars.at(iVar))));

    // Done, return
    return validity;
}

std::vector< std::pair<CompScaleVar::TypeEnum,double> > UncertaintySet::getUncertaintySet(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    std::vector< std::pair<CompScaleVar::TypeEnum,double> > unc;

    // Simple case
    if (!m_groups.size())
        return unc;

    // Get the sets
    const std::vector<CompScaleVar::TypeEnum> scaleVars = getScaleVars();

    // Process the sets one by one
    for (size_t iVar = 0; iVar < scaleVars.size(); ++iVar)
        unc.push_back(std::make_pair(scaleVars.at(iVar),getUncertainty(jet,eInfo,scaleVars.at(iVar))));

    // Done, return
    return unc;
}

std::vector< std::pair<CompScaleVar::TypeEnum,bool> > UncertaintySet::getValidUncertaintySet(std::vector< std::pair<CompScaleVar::TypeEnum,double> >& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    std::vector< std::pair<CompScaleVar::TypeEnum,bool> > validity;
    std::vector< std::pair<CompScaleVar::TypeEnum,double> > localUnc;

    // Simple case
    if (!m_groups.size())
    {
        unc = localUnc;
        return validity;
    }

    // Get the sets
    const std::vector<CompScaleVar::TypeEnum> scaleVars = getScaleVars();

    // Process the sets one by one
    for (size_t iVar = 0; iVar < scaleVars.size(); ++iVar)
    {
        double localUncValue = JESUNC_ERROR_CODE;
        validity.push_back(std::make_pair(scaleVars.at(iVar),getValidUncertainty(localUncValue,jet,eInfo,scaleVars.at(iVar))));
        localUnc.push_back(std::make_pair(scaleVars.at(iVar),localUncValue));
    }

    // Done, return
    unc = localUnc;
    return validity;
}

} // end jet namespace

