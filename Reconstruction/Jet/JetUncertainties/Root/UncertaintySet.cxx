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
    , m_comps()
    , m_shiftWeights()
{ }

UncertaintySet::~UncertaintySet()
{ }

StatusCode UncertaintySet::Initialize(const CP::SystematicSet& systConfig, const std::vector<UncertaintyComponent*>& components)
{
    // Ensure it wasn't already initialized
    if (m_isInit)
    {
        ATH_MSG_ERROR("Blocking double-initialization for: " << m_name.c_str());
        return StatusCode::FAILURE;
    }

    // Parse all of the names to get the components
    std::set<UncertaintyComponent*> compSet;
    for (CP::SystematicSet::const_iterator iter = systConfig.begin(); iter != systConfig.end(); ++iter)
        for (size_t iComp = 0; iComp < components.size(); ++iComp)
            if (components.at(iComp)->getName().CompareTo(iter->basename().c_str()) == 0)
            {
                // Watch for double-specified elements
                if (compSet.count(components.at(iComp)))
                {
                    ATH_MSG_ERROR("Multiple independent shifts requested for a single component: " << components.at(iComp)->getName().Data());
                    return StatusCode::FAILURE;
                }
                compSet.insert(components.at(iComp));
                
                m_comps.push_back(components.at(iComp));
                m_shiftWeights.push_back(iter->parameter());
                break;
            }

    
    // Ensure all of the elements were found
    if (systConfig.size() != m_comps.size())
    {
        ATH_MSG_ERROR(Form("Input SystematicSet is %zu elements, but only found %zu UncertaintyComponents",systConfig.size(),m_comps.size()));
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

bool UncertaintySet::getValidity(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const
{
    if (!m_comps.size())
        return true;

    // If the scale var wasn't specified, need to ensure all components match
    const bool checkScaleVar = scaleVar == CompScaleVar::UNKNOWN;
    const CompScaleVar::TypeEnum scaleVariable = checkScaleVar ? m_comps.at(0)->getScaleVar() : scaleVar;
    if (checkScaleVar)
        for (size_t iComp = 1; iComp < m_comps.size(); ++iComp)
            if (scaleVariable != m_comps.at(iComp)->getScaleVar())
            {
                ATH_MSG_ERROR("Asked for validity of a multi-scale-variable set without specifying any scale variable: " << m_name);
                return false;
            }
    
    // Checked conditions, now get the validity
    for (size_t iComp = 0; iComp < m_comps.size(); ++iComp)
        if (scaleVariable != m_comps.at(iComp)->getScaleVar())
            continue;
        else if (!m_comps.at(iComp)->getValidity(jet,eInfo))
            return false;
    return true;
}

double UncertaintySet::getUncertainty(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const
{
    // This is the sum of shifts multiple uncorrelated components
    // This could be a positive or negative number
    // Each individual component uncertainty will always be positive (quad sum of subcomps)
    // But the weight may be positive or negative (+N sigma or -N sigma variations)
    
    if (!m_comps.size())
        return 0;

    // If the scale var wasn't specified, need to ensure all components match
    const bool checkScaleVar = scaleVar == CompScaleVar::UNKNOWN;
    const CompScaleVar::TypeEnum scaleVariable = checkScaleVar ? m_comps.at(0)->getScaleVar() : scaleVar;
    if (checkScaleVar)
        for (size_t iComp = 1; iComp < m_comps.size(); ++iComp)
            if (scaleVariable != m_comps.at(iComp)->getScaleVar())
            {
                ATH_MSG_ERROR("Asked for validity of a multi-scale-variable set without specifying any scale variable: " << m_name);
                return false;
            }
    
    // Checked conditions, now get the uncertainty
    double unc = 0;
    for (size_t iComp = 0; iComp < m_comps.size(); ++iComp)
    {
        if (scaleVariable != m_comps.at(iComp)->getScaleVar())
            continue;
        unc += m_comps.at(iComp)->getUncertainty(jet,eInfo)*m_shiftWeights.at(iComp);
    }

    return unc;
}

bool UncertaintySet::getValidUncertainty(double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo, const CompScaleVar::TypeEnum scaleVar) const
{
    // See getUncertainty for discussion
    // This is not just calling the other two methods for speed, O(n) instead of O(2n)
    
    if (!m_comps.size())
    {
        unc = 0;
        return true;
    }

    // If the scale var wasn't specified, need to ensure all components match
    const bool checkScaleVar = scaleVar == CompScaleVar::UNKNOWN;
    const CompScaleVar::TypeEnum scaleVariable = checkScaleVar ? m_comps.at(0)->getScaleVar() : scaleVar;
    if (checkScaleVar)
        for (size_t iComp = 1; iComp < m_comps.size(); ++iComp)
            if (scaleVariable != m_comps.at(iComp)->getScaleVar())
            {
                ATH_MSG_ERROR("Asked for validity of a multi-scale-variable set without specifying any scale variable: " << m_name);
                return false;
            }
    
    // Checked conditions, now get the uncertainty
    double localUnc = 0;
    for (size_t iComp = 0; iComp < m_comps.size(); ++iComp)
    {
        if (scaleVariable != m_comps.at(iComp)->getScaleVar())
            continue;
        else if (!m_comps.at(iComp)->getValidity(jet,eInfo))
            return false;
        localUnc += m_comps.at(iComp)->getUncertainty(jet,eInfo)*m_shiftWeights.at(iComp);
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
    std::vector<CompScaleVar::TypeEnum> scaleVars;
    for (size_t iComp = 0; iComp < m_comps.size(); ++iComp)
    {
        // Get the scale variable
        CompScaleVar::TypeEnum scaleVar = m_comps.at(iComp)->getScaleVar();

        // Check if we already have it
        size_t index = scaleVars.size();
        for (size_t iVar = 0; iVar < scaleVars.size(); ++iVar)
            if (scaleVar == scaleVars.at(iVar))
            {
                index = iVar;
                break;
            }

        // If we don't already have it, then add it
        if (index >= scaleVars.size())
            scaleVars.push_back(scaleVar);
    }

    return scaleVars;
}

std::vector< std::pair<CompScaleVar::TypeEnum,bool> > UncertaintySet::getValiditySet(const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    std::vector< std::pair<CompScaleVar::TypeEnum,bool> > validity;

    // Simple case
    if (!m_comps.size())
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
    if (!m_comps.size())
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
    if (!m_comps.size())
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

