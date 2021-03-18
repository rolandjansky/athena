/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "JetUncertainties/ConfigHelper.h"
#include "JetUncertainties/Helpers.h"
#include <stdexcept>

#include "TEnv.h"

namespace jet
{

//////////////////////////////////////////////////
//                                              //
//  ComponentHelper constructor                 //
//                                              //
//////////////////////////////////////////////////

ComponentHelper::ComponentHelper(const TString name)
    : energyScale(0)
    , name(name)
{ }

ComponentHelper::ComponentHelper(TEnv& settings, const TString& compPrefix, const TString& MCtype, const float energyScaleVal)
    : energyScale(energyScaleVal)
{
    // Read in information on the uncertainty component
    //      - Name: component name
    //      - Param: parametrization
    //      - MassDef: mass definition (default, calo, TA, comb)
    //      - Scale: the variable to scale (fourvec, pt, D12, etc), default fourvec
    //      - Interp: If false, uses fixed bin content insted of interpolation
    //      - Special: Whether this component requires special treatment, "false" if not specified
    //      - Hists: histogram name(s), comma/space-separated list (if not specified, uses name)
    //      - VHist: validity histogram name (optional)
    //      - SubComp: sub-components for simple groups
    //      - Group: the number of the group this component belongs to (0 == single-component group)
    // Overwrite MCTYPE with the specified type if applicable
    name        = TString(settings.GetValue(compPrefix+"Name","")).ReplaceAll("MCTYPE",MCtype);
    param       = settings.GetValue(compPrefix+"Param","");
    massDefStr  = settings.GetValue(compPrefix+"MassDef","");
    scale       = settings.GetValue(compPrefix+"Scale","FourVec");
    topologyStr = settings.GetValue(compPrefix+"Topology","");
    interpolStr = settings.GetValue(compPrefix+"Interp","true");
    special     = settings.GetValue(compPrefix+"Special","");
    uncNameList = TString(settings.GetValue(compPrefix+"Hists","")).ReplaceAll("MCTYPE",MCtype);
    validName   = TString(settings.GetValue(compPrefix+"VHist","")).ReplaceAll("MCTYPE",MCtype);
    subCompList = TString(settings.GetValue(compPrefix+"SubComp","")).ReplaceAll("MCTYPE",MCtype);
    splitNum    = settings.GetValue(compPrefix+"Split",0);
    groupNum    = settings.GetValue(compPrefix+"Group",0);
    combMassStr = settings.GetValue(compPrefix+"CombMassType","");
    caloMassTerm= settings.GetValue(compPrefix+"CaloMassTerm","");
    TAMassTerm  = settings.GetValue(compPrefix+"TAMassTerm","");
    caloMassDef = settings.GetValue(compPrefix+"CaloMassDef","");
    TAMassDef   = settings.GetValue(compPrefix+"TAMassDef","");
    truthLabelStr = settings.GetValue(compPrefix+"TruthLabels","");
    LargeRJetTruthLabelName = settings.GetValue(compPrefix+"LargeRJetTruthLabelName","R10TruthLabel_R21Consolidated");
    TString LargeRJetTruthLabelStrOld = settings.GetValue(compPrefix+"FatjetTruthLabels","");
    TString LargeRJetTruthLabelStrNew = settings.GetValue(compPrefix+"LargeRJetTruthLabels","");
    LargeRJetTruthLabelsForSFstr = settings.GetValue(compPrefix+"LargeRJetTruthLabelForSF","");
    RegionForSFstr = settings.GetValue(compPrefix+"RegionForSF","");
    ResultName = settings.GetValue(compPrefix+"ResultName","");

    // Get enums where appropriate
    // Leave interpreting/checking the enums to others
    parametrization = CompParametrization::stringToEnum(param);
    massDef         = CompMassDef::stringToEnum(massDefStr);
    scaleVar        = CompScaleVar::stringToEnum(scale);
    topology        = JetTopology::stringToEnum(topologyStr);
    isSpecial       = (!special.CompareTo("true",TString::kIgnoreCase)) || (!special.CompareTo("yes",TString::kIgnoreCase));
    pileupType      = PileupComp::stringToEnum(name);
    flavourType     = FlavourComp::stringToEnum(name);
    combMassType    = CombMassComp::stringToEnum(combMassStr);
    interpolate     = Interpolate::stringToEnum(interpolStr);
    uncNames        = utils::vectorize<TString>(uncNameList,", ");
    subComps        = utils::vectorize<TString>(subCompList,", ");
    truthLabels     = utils::vectorize<int>(truthLabelStr,", ");
    if (LargeRJetTruthLabelStrOld != "" && LargeRJetTruthLabelStrNew != "")
        throw std::runtime_error("ERROR: double-specification of the LargeRJetTruthLabels/FatjetTruthLabels property");
    else if (LargeRJetTruthLabelStrNew != "")
        LargeRJetTruthLabelStr = LargeRJetTruthLabelStrNew;
    else
        LargeRJetTruthLabelStr = LargeRJetTruthLabelStrOld;
    
    LargeRJetTruthLabelStrs = utils::vectorize<TString>(LargeRJetTruthLabelStr,",");
    for (const TString& aVal : LargeRJetTruthLabelStrs)
    {
        if (LargeRJetTruthLabel::stringToEnum(aVal) == LargeRJetTruthLabel::UNKNOWN)
        {
            // Note: throwing an exception here because we can't return StatusCode::FAILURE or similar and this doesn't inherit from a class with such functionality
            // This error message should anyways only occur if the CP group provides a bad config file, so this error will only be printed when we are debugging our inputs and before it gets to users
            throw std::runtime_error(Form("ERROR: Unable to convert specified LargeRJetTruthLabel to a recognized enum value, please check the configuration file for mistakes: %s",aVal.Data()));
        }
        else
            LargeRJetTruthLabels.push_back(LargeRJetTruthLabel::stringToEnum(aVal));
    }
    LargeRJetTruthLabelsForSFstrs = utils::vectorize<TString>(LargeRJetTruthLabelsForSFstr, ",");
    for (const TString& aVal : LargeRJetTruthLabelsForSFstrs)
    {
        if (CompFlavorLabelVar::stringToEnum(aVal) == CompFlavorLabelVar::UNKNOWN)
        {
	    throw std::runtime_error(Form("ERROR: Unable to convert specified LargeRJetTruthLabelForSF to a recognized enum value, please check the configuration file for mistakes: %s",aVal.Data()));
        }
        else
	    LargeRJetTruthLabelsForSF.push_back(CompFlavorLabelVar::stringToEnum(aVal));
    }
    RegionForSF     = CompTaggerRegionVar::stringToEnum(RegionForSFstr);
}


//////////////////////////////////////////////////
//                                              //
//  GroupHelper constructor                     //
//                                              //
//////////////////////////////////////////////////

GroupHelper::GroupHelper(const TString name)
    : name(name)
{ }

GroupHelper::GroupHelper(TEnv& settings, const TString& groupPrefix, const TString& MCtype)
{
    // Read in information on the uncertainty group
    //      - Name: group name
    //      - Desc: description
    //      - Type: category
    //      - Corr: correlation type (only need to specify if >1 histogram)
    //      - Split: Number of sub-components to split this component into (default 1, no split)
    //      - Reduce: Whether or not a given group is safe for eigenvector reduction
    //      - Group: The group number to link relevant components to this information
    //      - SubGroup: subgroup number(s), comma/space-separated list (if not specified, no subgroup)
    // Overwrite MCTYPE with the specified type if applicable
    name        = TString(settings.GetValue(groupPrefix+"Name","")).ReplaceAll("MCTYPE",MCtype);
    desc        = settings.GetValue(groupPrefix+"Desc","");
    cat         = settings.GetValue(groupPrefix+"Type","");
    corr        = settings.GetValue(groupPrefix+"Corr","");
    isRed	= settings.GetValue(groupPrefix+"Reducible","true"); 
    groupNum    = settings.GetValue(groupPrefix+"Group",0);
    subgroupNum = settings.GetValue(groupPrefix+"SubGroup",0);
    
    // Get enums where appropriate
    // Leave interpreting/checking the enums to others
    category        = CompCategory::stringToEnum(cat);
    correlation     = CompCorrelation::stringToEnum(corr);
    reducible	    = utils::getTypeObjFromString<bool>(isRed);
}


//////////////////////////////////////////////////
//                                              //
//  ConfigHelper constructor and methods        //
//                                              //
//////////////////////////////////////////////////

ConfigHelper::ConfigHelper()
    : asg::AsgMessaging("")
    , m_isInit(false)
    , m_confPrefix("")
    , m_MCtype("")
    , m_energyScale(0)
    , m_cInfo(NULL)
    , m_gInfo(NULL)
{
    JESUNC_NO_DEFAULT_CONSTRUCTOR;
}

ConfigHelper::ConfigHelper(const TString& confPrefix, const TString& MCtype, const float energyScaleVal)
    : asg::AsgMessaging(confPrefix.Data())
    , m_isInit(false)
    , m_confPrefix(confPrefix)
    , m_MCtype(MCtype)
    , m_energyScale(energyScaleVal)
    , m_cInfo(NULL)
    , m_gInfo(NULL)
{ }

ConfigHelper::~ConfigHelper()
{
    JESUNC_SAFE_DELETE(m_cInfo);
    JESUNC_SAFE_DELETE(m_gInfo);
}

StatusCode ConfigHelper::initialize(TEnv& settings)
{
    if (m_isInit)
    {
        ATH_MSG_ERROR("Blocking double-initialization: " << m_confPrefix.Data());
        return StatusCode::FAILURE;
    }

    m_cInfo = new ComponentHelper(settings, m_confPrefix, m_MCtype, m_energyScale);
    if (!m_cInfo)
    {
        ATH_MSG_ERROR("Failed to create ComponentHelper: " << m_confPrefix.Data());
        return StatusCode::FAILURE;
    }

    m_gInfo = new GroupHelper(settings, m_confPrefix, m_MCtype);
    if (!m_gInfo)
    {
        ATH_MSG_ERROR("Failed to create GroupHelper: " << m_confPrefix.Data());
        return StatusCode::FAILURE;
    }

    m_isInit = true;
    return StatusCode::SUCCESS;
}

bool ConfigHelper::isCompGroup() const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("ConfigHelper has not yet been initialized: " << m_confPrefix.Data());
        return false;
    }
    
    // Simple group possibilities:
    //      Single-component with all group info specified on the component, "Group" == 0 (or unspec)
    //      Multi-component "SubComp" with all group info specified on the component, "Group" == 0
    // So the defining factor of simple groups is "Group" == 0 (== unspecified)
    // It also has to have a name to ensure it's not just empty (it will be if this is a group)
    if (m_cInfo->groupNum == 0 && m_cInfo->name != "")
        return true;
    return false;
}

bool ConfigHelper::isComponent() const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("ConfigHelper has not yet been initialized: " << m_confPrefix.Data());
        return false;
    }
    
    // Component means that it has a component name and that the group is specified
    if (m_cInfo->groupNum != 0 && m_cInfo->name != "")
        return true;
    return false;
}

bool ConfigHelper::isGroup() const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("ConfigHelper has not yet been initialized: " << m_confPrefix.Data());
        return false;
    }

    // Group means that it has a group name and the group is specified
    if (m_gInfo->groupNum != 0 && m_gInfo->name != "")
        return true;
    return false;
}

const ComponentHelper* ConfigHelper::getComponentInfo() const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("ConfigHelper has not yet been initialized: " << m_confPrefix.Data());
        return NULL;
    }
    return m_cInfo;
}

const GroupHelper* ConfigHelper::getGroupInfo() const
{
    if (!m_isInit)
    {
        ATH_MSG_ERROR("ConfigHelper has not yet been initialized: " << m_confPrefix.Data());
        return NULL;
    }
    return m_gInfo;
}

void ConfigHelper::enforceGroupNamePrefix(const std::string& prefix)
{
    enforceGroupNamePrefix(TString(prefix.c_str()));
}

void ConfigHelper::enforceGroupNamePrefix(const TString& prefix)
{
    // Doesn't have the prefix
    if (!m_gInfo->name.BeginsWith(prefix,TString::kIgnoreCase))
        m_gInfo->name = Form("%s%s",prefix.Data(),m_gInfo->name.Data());
    // Has the right prefix, but not the right case (enforce identical prefix)
    else if (!m_gInfo->name.BeginsWith(prefix))
        m_gInfo->name.Replace(0,prefix.Length(),prefix);
}

void ConfigHelper::setComponentJetDefSuffix(const std::string& suffix)
{
    setComponentJetDefSuffix(TString(suffix.c_str()));
}

void ConfigHelper::setComponentJetDefSuffix(const TString& suffix)
{
    if (!m_cInfo->uncNames.size() && !m_cInfo->subComps.size())
        m_cInfo->uncNames.push_back(m_cInfo->name+"_"+suffix);
    else if (m_cInfo->uncNames.size())
        for (size_t iName = 0; iName < m_cInfo->uncNames.size(); ++iName)
            m_cInfo->uncNames[iName] = m_cInfo->uncNames[iName] + "_" + suffix;
    else if (m_cInfo->subComps.size())
        for (size_t iSubComp = 0; iSubComp < m_cInfo->subComps.size(); ++iSubComp)
            m_cInfo->subComps[iSubComp] = m_cInfo->subComps[iSubComp] + "_" + suffix;
    
    if (m_cInfo->validName != "")
        m_cInfo->validName = m_cInfo->validName+"_"+suffix;
}

void ConfigHelper::addValidityHistogram(const std::string& histName)
{
    addValidityHistogram(TString(histName.c_str()));
}

void ConfigHelper::addValidityHistogram(const TString& histName)
{
    if (histName != "")
    {
        if (m_cInfo->validName != "")
        {
            ATH_MSG_WARNING(Form("Ignoring request to add validity histogram to: %s",m_cInfo->name.Data()));
            ATH_MSG_WARNING(Form("The following validity histogram was already specified: %s",m_cInfo->validName.Data()));
            ATH_MSG_WARNING(Form("Blocking overwrite with a new validity histogram: %s",histName.Data()));
        }
        else
            m_cInfo->validName = histName;
    }
}


} // end jet namespace

