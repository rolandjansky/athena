/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// General package includes
#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetUncertainties/Helpers.h"
#include "JetUncertainties/UncertaintyEnum.h"
#include "JetUncertainties/ComponentHelper.h"
#include "JetUncertainties/CorrelationMatrix.h"

// UncertaintyComponent types
#include "JetUncertainties/UncertaintyComponent.h"
#include "JetUncertainties/UncertaintyComponentGroup.h"
#include "JetUncertainties/UncertaintySet.h"
#include "JetUncertainties/PtUncertaintyComponent.h"
#include "JetUncertainties/PtEtaUncertaintyComponent.h"
#include "JetUncertainties/PtMassEtaUncertaintyComponent.h"
#include "JetUncertainties/PileupUncertaintyComponent.h"
#include "JetUncertainties/FlavourUncertaintyComponent.h"
#include "JetUncertainties/PunchthroughUncertaintyComponent.h"
#include "JetUncertainties/ClosebyUncertaintyComponent.h"

// xAOD includes
#include "xAODCore/ShallowCopy.h"
#include "xAODTracking/VertexContainer.h"

// CP interface includes
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"

// ROOT includes
#include "TString.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TEnv.h"
#include "TH2D.h"

// C++ includes
#include <unordered_set>

using namespace jet;

//////////////////////////////////////////////////
//                                              //
//  Constructor/destructor/initialization       //
//                                              //
//////////////////////////////////////////////////

JetUncertaintiesTool::JetUncertaintiesTool(const std::string& name)
    : asg::AsgTool(name)
    , m_isInit(false)
    , m_name(name)
    , m_energyScale(1.e-3)
    , m_release("")
    , m_jetDef("")
    , m_mcType("")
    , m_configFile("")
    , m_path("")
    , m_analysisFile("")
    , m_refNPV(-1)
    , m_refMu(-1)
    , m_refNPVHist(NULL)
    , m_refMuHist(NULL)
    , m_components()
    , m_groups()
    , m_NPVAccessor("NPV")
    , m_D12Accessor("Split12")
    , m_D23Accessor("Split23")
    , m_Tau21Accessor("Tau21")
    , m_Tau32Accessor("Tau32")
    , m_Tau1Accessor("Tau1")
    , m_Tau2Accessor("Tau2")
    , m_Tau3Accessor("Tau3")
    , m_recognizedSystematics()
    , m_recommendedSystematics()
    , m_currentSystSet()
    , m_currentUncSet(NULL)
    , m_systFilterMap()
    , m_systSetMap()
    , m_namePrefix("JET_")
{
    declareProperty("JetDefinition",m_jetDef);
    declareProperty("MCType",m_mcType);
    declareProperty("ConfigFile",m_configFile);
    declareProperty("Path",m_path);
    declareProperty("AnalysisFile",m_analysisFile);

    ATH_MSG_DEBUG("Creating JetUncertaintiesTool named "<<m_name);

    // Set dummy default systematic (do nothing)
    // Prevents NULL access if user tries to apply correction without first calling function
    if (applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok)
        ATH_MSG_ERROR(Form("Failed to pre-set applySystematicVariation to no variation"));
}

JetUncertaintiesTool::JetUncertaintiesTool(const JetUncertaintiesTool& toCopy)
    : asg::AsgTool(toCopy.m_name+"_copy")
    , m_isInit(toCopy.m_isInit)
    , m_name(toCopy.m_name+"_copy")
    , m_energyScale(1.e-3)
    , m_release(toCopy.m_release)
    , m_jetDef(toCopy.m_jetDef)
    , m_mcType(toCopy.m_mcType)
    , m_configFile(toCopy.m_configFile)
    , m_path(toCopy.m_path)
    , m_analysisFile(toCopy.m_analysisFile)
    , m_refNPV(toCopy.m_refNPV)
    , m_refMu(toCopy.m_refMu)
    , m_refNPVHist(toCopy.m_refNPVHist?new UncertaintyHistogram(*toCopy.m_refNPVHist):NULL)
    , m_refMuHist(toCopy.m_refMuHist?new UncertaintyHistogram(*toCopy.m_refMuHist):NULL)
    , m_components()
    , m_groups()
    , m_NPVAccessor(toCopy.m_NPVAccessor)
    , m_D12Accessor(toCopy.m_D12Accessor)
    , m_D23Accessor(toCopy.m_D23Accessor)
    , m_Tau21Accessor(toCopy.m_Tau21Accessor)
    , m_Tau32Accessor(toCopy.m_Tau32Accessor)
    , m_Tau1Accessor(toCopy.m_Tau1Accessor)
    , m_Tau2Accessor(toCopy.m_Tau2Accessor)
    , m_Tau3Accessor(toCopy.m_Tau3Accessor)
    , m_recognizedSystematics(toCopy.m_recognizedSystematics)
    , m_recommendedSystematics(toCopy.m_recommendedSystematics)
    , m_currentSystSet(toCopy.m_currentSystSet)
    , m_currentUncSet(NULL)
    , m_systFilterMap()
    , m_systSetMap()
    , m_namePrefix(toCopy.m_namePrefix)
{
    ATH_MSG_DEBUG("Creating copy of JetUncertaintiesTool named "<<m_name);

    for (size_t iComp = 0; iComp < toCopy.m_components.size(); ++iComp)
        m_components.push_back(m_components.at(iComp)->clone());

    for (size_t iGroup = 0; iGroup < toCopy.m_groups.size(); ++iGroup)
        m_groups.push_back(m_groups.at(iGroup)->clone());

    if (applySystematicVariation(m_currentSystSet) != CP::SystematicCode::Ok)
        ATH_MSG_ERROR(Form("Failed to re-set applySystematicVariation in new tool copy"));
}

JetUncertaintiesTool::~JetUncertaintiesTool()
{
    ATH_MSG_DEBUG(Form("Deleting JetUncertaintiesTool named %s",m_name.c_str()));
    
    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        JESUNC_SAFE_DELETE(m_components.at(iComp));
    m_components.clear();
    m_groups.clear(); // No need to free - already done in the components list
    
    JESUNC_SAFE_DELETE(m_refNPVHist);
    JESUNC_SAFE_DELETE(m_refMuHist);

    m_currentUncSet  = NULL;
    
    m_systFilterMap.clear();

    std::unordered_map<CP::SystematicSet,UncertaintySet*>::iterator iter;
    for (iter = m_systSetMap.begin(); iter != m_systSetMap.end(); ++iter)
        JESUNC_SAFE_DELETE(iter->second);
    m_systSetMap.clear();
}

StatusCode JetUncertaintiesTool::setScaleToMeV()
{
    // Ensure it hasn't been initialized yet
    if (m_isInit)
    {
        ATH_MSG_FATAL("Cannot set the energy scale after initialization of tool: " << m_name);
        return StatusCode::FAILURE;
    }

    m_energyScale = 1.e-3;
    return StatusCode::SUCCESS;
}

StatusCode JetUncertaintiesTool::setScaleToGeV()
{
    // Ensure it hasn't been initialized yet
    if (m_isInit)
    {
        ATH_MSG_FATAL("Cannot set the energy scale after initialization of tool: " << m_name);
        return StatusCode::FAILURE;
    }

    m_energyScale = 1;
    return StatusCode::SUCCESS;
}

StatusCode JetUncertaintiesTool::initialize()
{
    // Ensure it hasn't been initialized already
    if (m_isInit)
    {
        ATH_MSG_FATAL(Form("Blocking re-initialization of tool named %s",m_name.c_str()));
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO(Form("Preparing to initialize the JetUncertaintiesTool named %s",m_name.c_str()));

    // Cache the current directory
    TDirectory* currentDir = gDirectory;
    gROOT->cd();

    // Read the config file
    const TString configFilePath = jet::utils::findFilePath(m_configFile.c_str(),m_path.c_str());
    if (configFilePath == "")
    {
        ATH_MSG_ERROR("Cannot find config file: " << m_configFile << " (path is " << m_path << ")");
        return StatusCode::FAILURE;
    }

    TEnv settings;
    if (settings.ReadFile(configFilePath.Data(),kEnvGlobal))
    {
        ATH_MSG_ERROR("Cannot read config file: " << configFilePath.Data());
        return StatusCode::FAILURE;
    }

    // We can read it - start printing
    ATH_MSG_INFO(Form("================================================"));
    ATH_MSG_INFO(Form("  Initializing the JetUncertaintiesTool named %s",m_name.c_str()));
    ATH_MSG_INFO(Form("  Path is: %s",m_path.c_str()));
    ATH_MSG_INFO(Form("  Configuration read in from:" ));
    ATH_MSG_INFO(Form("    %s",configFilePath.Data()));
    
    
    // Get the uncertainty release
    m_release = settings.GetValue("UncertaintyRelease","UNKNOWN");
    ATH_MSG_INFO(Form("  Uncertainty release: %s",m_release.c_str()));

    // Check the jet definition
    TString allowedJetDefStr = settings.GetValue("SupportedJetDefs","");
    if (allowedJetDefStr == "")
    {
        ATH_MSG_ERROR("Cannot find supported jet definitions in config");
        return StatusCode::FAILURE;
    }
    std::vector<TString> allowedJetDefs = jet::utils::vectorize<TString>(allowedJetDefStr," ,");
    bool foundJetDef = false;
    for (size_t iDef = 0; iDef < allowedJetDefs.size(); ++iDef)
        if (!allowedJetDefs.at(iDef).CompareTo(m_jetDef.c_str(),TString::kIgnoreCase))
        {
            foundJetDef = true;
            m_jetDef = allowedJetDefs.at(iDef); // To ensure right capitalization
            break;
        }
    if (!foundJetDef)
    {
        ATH_MSG_ERROR("Unsupported jet definition: " << m_jetDef);
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(Form("  Jet definition: %s",m_jetDef.c_str()));

    // Check the MC type
    TString allowedMCtypeStr = settings.GetValue("SupportedMCTypes","");
    if (allowedMCtypeStr == "")
    {
        ATH_MSG_ERROR("Cannot find supported MC types in config");
        return StatusCode::FAILURE;
    }
    std::vector<TString> allowedMCtypes = jet::utils::vectorize<TString>(allowedMCtypeStr," ,");
    bool foundMCtype = false;
    for (size_t iType = 0; iType < allowedMCtypes.size(); ++iType)
        if (!allowedMCtypes.at(iType).CompareTo(m_mcType.c_str(),TString::kIgnoreCase))
        {
            foundMCtype = true;
            m_mcType = allowedMCtypes.at(iType); // To ensure right capitalization
            break;
        }
    if (!foundMCtype)
    {
        ATH_MSG_ERROR("Unsupported MC type: " << m_mcType);
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(Form("  MC type: %s",m_mcType.c_str()));

    
    // Get the file to read uncertainties in from
    TString histFileName = settings.GetValue("UncertaintyRootFile","");
    if (histFileName == "")
    {
        ATH_MSG_ERROR("Cannot find uncertainty histogram file");
        return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(Form("  UncertaintyFile: %s",histFileName.Data()));

    // Get the analysis ROOT file for later use (only if it wasn't specified by user config)
    if (m_analysisFile == "")
    {
        m_analysisFile = settings.GetValue("AnalysisRootFile","");
        if (m_analysisFile == "")
        {
            ATH_MSG_ERROR("Cannot find analysis root file");
            return StatusCode::FAILURE;
        }
    }
    ATH_MSG_INFO(Form("  AnalysisFile: %s",m_analysisFile.c_str()));

    
    // Now open the histogram file
    TFile* histFile = utils::readRootFile(histFileName,m_path.c_str());
    if (!histFile || histFile->IsZombie())
    {
        ATH_MSG_ERROR("Cannot open uncertainty histogram file: " << histFileName.Data());
        return StatusCode::FAILURE;
    }
    
    // Get the NPV/mu reference values
    // These may not be set - only needed if a pileup component is requested
    TString refNPV = settings.GetValue("Pileup.NPVRef","");
    TString refMu  = settings.GetValue("Pileup.MuRef","");
    if ( (refNPV != "" && refMu == "") || (refNPV == "" && refMu != "") )
    {
        ATH_MSG_ERROR(Form("Only one of the pileup references was specified: (NPV,mu) = (%.1f,%.1f)",m_refNPV,m_refMu));
        return StatusCode::FAILURE;
    }
    else if ( refNPV != "" && refMu != "")
    {
        // Check if these are floating point values for the pileup references
        // If so, then fill the float, otherwise retrieve the histogram
        if (utils::isTypeObjFromString<float>(refNPV))
            m_refNPV = utils::getTypeObjFromString<float>(refNPV);
        else
        {
            m_refNPVHist = new UncertaintyHistogram(refNPV+"_"+m_jetDef,"",false);
            if (m_refNPVHist->initialize(histFile).isFailure())
                return StatusCode::FAILURE;
        }

        if (utils::isTypeObjFromString<float>(refMu))
            m_refMu = utils::getTypeObjFromString<float>(refMu);
        else
        {
            m_refMuHist = new UncertaintyHistogram(refMu+"_"+m_jetDef,"",false);
            if (m_refMuHist->initialize(histFile).isFailure())
                return StatusCode::FAILURE;
        }
    }
    
    // Loop over uncertainty components in the config
    ATH_MSG_INFO("");
    ATH_MSG_INFO(Form("%6s %-35s : %s","","JES uncert. comp.","Description"));
    ATH_MSG_INFO(Form("%6s %-35s  -%s","","-----------------","-----------"));
    for (size_t iComp = 0; iComp < 999; ++iComp)
    {
        // Format the style
        const TString prefix = Form("JESComponent.%zu.",iComp);

        // Read in information on the uncertainty component
        const ComponentHelper component(settings,prefix,m_mcType.c_str(),m_energyScale);

        // Ignore component if it is not defined
        if (component.name == "")
            continue;
        
        // Call the uncertainty component helper method to add a new component
        if(addUncertaintyComponent(histFile,component).isFailure())
            return StatusCode::FAILURE;
    }
    // Initialize the groups to mark them complete
    for (size_t iGroup = 0; iGroup < m_groups.size(); ++iGroup)
        if (m_groups.at(iGroup)->initialize().isFailure())
            return StatusCode::FAILURE;
    
    
    // Determine the number of input parameters (complicated when groups are in use)
    size_t numCompInGroups = 0;
    for (size_t iGroup = 0; iGroup < m_groups.size(); ++iGroup)
        numCompInGroups += m_groups.at(iGroup)->getNumComponents();

    // Summary message
    ATH_MSG_INFO(Form("   Found and read in %zu components%s",m_components.size(),m_groups.size()?Form(" (%zu inputs in %zu groups, %zu independent input%s):",numCompInGroups,m_groups.size(),m_components.size()-m_groups.size(),m_components.size()-m_groups.size()!=1?"s":""):""));
    if (m_groups.size())
        for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
            ATH_MSG_INFO(Form("%5zu. %-35s : %s",iComp+1,m_components.at(iComp)->getName().Data(),m_components.at(iComp)->getDesc().Data()));
    ATH_MSG_INFO(Form("================================================"));

    // Close the histogram file
    histFile->Close();
    // Go back to initial directory
    gDirectory = currentDir;

    // Finally done!
    m_isInit = true;
    return asg::AsgTool::initialize();
}

//////////////////////////////////////////////////
//                                              //
//  Initialization helper methods               //
//                                              //
//////////////////////////////////////////////////

StatusCode JetUncertaintiesTool::addUncertaintyComponent(TFile* histFile, const ComponentHelper& component)
{
    // Sanity checks
    if (component.category == CompCategory::UNKNOWN)
    {
        ATH_MSG_ERROR("Unexpected category " << component.cat.Data() << " for component " << component.name.Data());
        return StatusCode::FAILURE;
    }
    if (component.parametrization == CompParametrization::UNKNOWN)
    {
        ATH_MSG_ERROR("Unexpected parametrization " << component.param.Data() << " for component " << component.name.Data());
        return StatusCode::FAILURE;
    }
    
    // Ensure we asked for a reasonable split
    //       0. Default, no split, no weight required
    //       1. Split with weight number 1 (linear increase vs log pT)
    //      -1. Split with weight number 2 (quadratic complement to 1)
    //      Otherwise, this is currently unsupported
    if (component.splitNum != 0 && abs(component.splitNum) > 4)
    {
        ATH_MSG_ERROR("Only split numbers of 0, +/-1, +/-2, +/-3, and +/-4 are currently supported, not " << component.splitNum << ", for component " << component.name.Data());
        return StatusCode::FAILURE;
    }
    
    // Add the split number to the name and description if necessary
    TString localCompName = component.splitNum != 0 ? Form("%s_T%d%s",component.name.Data(),abs(component.splitNum),component.splitNum > 0 ? "F" : "C") : component.name.Data();
    TString localCompDesc = component.splitNum != 0 ? Form("%s [Type %d, %s part]",component.desc.Data(),abs(component.splitNum),component.splitNum > 0 ? "functional" : "complementary") : component.desc.Data();

    // Prepend the jet systematic prefix if necessary
    if (m_namePrefix != "")
    {
        // Doesn't have the prefix
        if (!localCompName.BeginsWith(m_namePrefix.c_str(),TString::kIgnoreCase))
            localCompName = Form("%s%s",m_namePrefix.c_str(),localCompName.Data());
        // Has the right prefix, but not the right case (enforce identical prefix)
        if (!localCompName.BeginsWith(m_namePrefix.c_str()))
            localCompName.Replace(0,m_namePrefix.size(),m_namePrefix.c_str());
    }

    // Set the local component name
    jet::ComponentHelper localComponent(component);
    localComponent.name = localCompName;
    localComponent.desc = localCompDesc;


    // Special cases first
    if (component.isSpecial)
    {
        // First check pileup components
        if (component.pileupType != PileupComp::UNKNOWN)
        {
            // Ensure that the reference values were specified
            if (m_refNPV < 0 && !m_refNPVHist)
            {
                ATH_MSG_ERROR("Attempted to create pileup component without NPV reference value: " << localCompName.Data());
                return StatusCode::FAILURE;
            }
            if (m_refMu < 0 && !m_refMuHist)
            {
                ATH_MSG_ERROR("Attempted to create pileup component without mu reference value: " << localCompName.Data());
                return StatusCode::FAILURE;
            }

            if (component.parametrization == CompParametrization::PtEta || component.parametrization == CompParametrization::PtAbsEta)
            {
                if (m_refNPVHist && m_refMuHist)
                    m_components.push_back(new PileupUncertaintyComponent(localComponent,m_refNPVHist,m_refMuHist));
                else if (!m_refNPVHist && !m_refMuHist)
                    m_components.push_back(new PileupUncertaintyComponent(localComponent,m_refNPV,m_refMu));
                else if (m_refNPVHist && !m_refMuHist)
                    m_components.push_back(new PileupUncertaintyComponent(localComponent,m_refNPVHist,m_refMu));
                else if (!m_refNPVHist && m_refMuHist)
                    m_components.push_back(new PileupUncertaintyComponent(localComponent,m_refNPV,m_refMuHist));
            }
            else
            {
                ATH_MSG_ERROR(Form("Unexpected parametrization of %s for component %s",CompParametrization::enumToString(component.parametrization).Data(),localCompName.Data()));
                return StatusCode::FAILURE;
            }
        }
        // Next check flavour components
        else if (component.flavourType != FlavourComp::UNKNOWN)
        {
            if (component.parametrization == CompParametrization::PtEta || component.parametrization == CompParametrization::PtAbsEta)
                m_components.push_back(new FlavourUncertaintyComponent(localComponent,m_jetDef,m_analysisFile,m_path.c_str()));
            else
            {
                ATH_MSG_ERROR(Form("Unexpected parametrization of %s for component %s",CompParametrization::enumToString(component.parametrization).Data(),localCompName.Data()));
                return StatusCode::FAILURE;
            }
        }
        // Next check punchthrough
        else if (localCompName.Contains("PunchThrough",TString::kIgnoreCase))
        {
            if (component.parametrization == CompParametrization::PtEta || component.parametrization == CompParametrization::PtAbsEta)
                m_components.push_back(new PunchthroughUncertaintyComponent(localComponent));
            else
            {
                ATH_MSG_ERROR(Form("Unexpected parametrization of %s for component %s",CompParametrization::enumToString(component.parametrization).Data(),localCompName.Data()));
                return StatusCode::FAILURE;
            }
        }
        // Next check closeby
        else if (localCompName.Contains("Closeby",TString::kIgnoreCase))
        {
            if (component.parametrization == CompParametrization::Pt)
                m_components.push_back(new ClosebyUncertaintyComponent(localComponent));
            else
            {
                ATH_MSG_ERROR(Form("Unexpected parametrization of %s for component %s",CompParametrization::enumToString(component.parametrization).Data(),localCompName.Data()));
            }
        }
        else
        {
            ATH_MSG_ERROR("Unexpected special component: " << localCompName.Data());
            return StatusCode::FAILURE;
        }
        
    }
    // Standard components
    else
    {
        switch(component.parametrization)
        {
            case CompParametrization::Pt:
                m_components.push_back(new PtUncertaintyComponent(localComponent));
                break;
            case CompParametrization::PtEta:
            case CompParametrization::PtAbsEta:
                m_components.push_back(new PtEtaUncertaintyComponent(localComponent));
                break;
            case CompParametrization::PtMassEta:
            case CompParametrization::PtMassAbsEta:
                m_components.push_back(new PtMassEtaUncertaintyComponent(localComponent));
                break;
            default:
                ATH_MSG_ERROR("Encountered unexpected parameter type: " << component.param.Data());
                return StatusCode::FAILURE;
        }
    }
    
    // Add histogram(s) to components and initialize
    // No specified histograms == use the name of the component for the histogram name
    // Note: use the raw name without the split number, not the local name
    std::vector<TString> histNames;
    if (component.hists == "")
        histNames.push_back(component.name);
    else
    {
        const bool success = utils::vectorize<TString>(component.hists,", ",histNames);
        if (!success)
        {
            ATH_MSG_ERROR("Failed to convert histNames into a vector: " << component.hists.Data());
            return StatusCode::FAILURE;
        }
    }
    // Add the jet string to the end of the name
    for (size_t iHisto = 0; iHisto < histNames.size(); ++iHisto)
        histNames[iHisto] = histNames[iHisto]+"_"+m_jetDef;

    
    // Check if validity histograms were specified
    std::vector<TString> validHistNames;
    if (component.validHists != "")
    {
        const bool success = utils::vectorize<TString>(component.validHists,", ",validHistNames);
        if (!success)
        {
            ATH_MSG_ERROR("Failed to convert validHists into a vector: " << component.validHists.Data());
            return StatusCode::FAILURE;
        }
    }

    // Now initialize
    if (!validHistNames.size())
    {
        if (m_components.back()->initialize(histNames,histFile).isFailure())
            return StatusCode::FAILURE;
    }
    else
    {
        if (m_components.back()->initialize(histNames,validHistNames,histFile).isFailure())
            return StatusCode::FAILURE;
    }

    // Check if the component is part of a group
    if (component.group > 0)
    {
        // This component belongs to a group
        // We need to remove the component from the global list and give it to the group
        // If the group does not exist, we need to make a new one
        bool foundGroup = false;
        for (size_t iGroup = 0; iGroup < m_groups.size(); ++iGroup)
            if (m_groups.at(iGroup)->getGroupNum() == component.group)
            {
                foundGroup = true;
                // Transfer component ownership to the group
                if (m_groups.at(iGroup)->addComponent(m_components.back()).isFailure())
                    return StatusCode::FAILURE;
                m_components.pop_back();
            }

        if (!foundGroup)
        {
            const TString groupName = Form("%sGroupedNP_%zu",m_namePrefix.c_str(),m_groups.size()+1);
            // Make a new group and use that
            ComponentHelper groupHelper(groupName,m_energyScale);
            groupHelper.desc = Form("Component group %zu",m_groups.size()+1);
            groupHelper.group = component.group;
            groupHelper.category = CompCategory::Other;
            groupHelper.correlation = CompCorrelation::Uncorrelated;
            groupHelper.scaleVar = component.scaleVar;
            m_groups.push_back(new UncertaintyComponentGroup(groupHelper));
            if (m_groups.back()->addComponent(m_components.back()).isFailure())
                return StatusCode::FAILURE;
            m_components.pop_back();
            m_components.push_back(m_groups.back());

            // Add the new group to the registry of what can be applied
            CP::SystematicVariation systVar(groupName.Data(),CP::SystematicVariation::CONTINUOUS);
            if (addAffectingSystematic(systVar,true) != CP::SystematicCode::Ok)
                return StatusCode::FAILURE;
        }
    }
    else
    {
        // Not part of a group, add the component to the registry of what can be applied
        // Note: check if this is a MC non-closure term, which is zero for the nominal MC type
        // In this case, we do not want to set this as a recommended systematic (it has no effect)
        const bool isRecommended = component.name.BeginsWith("RelativeNonClosure_") && m_components.back()->isAlwaysZero() ? false : true;
        CP::SystematicVariation systVar(localCompName.Data(),CP::SystematicVariation::CONTINUOUS);
        if (addAffectingSystematic(systVar,isRecommended) != CP::SystematicCode::Ok)
            return StatusCode::FAILURE;
    }
    
    // Determine the number of input components (not straightforward when groups are used)
    size_t compNum = m_components.size() - m_groups.size();
    for (size_t iGroup = 0; iGroup < m_groups.size(); ++iGroup)
        compNum += m_groups.at(iGroup)->getNumComponents();

    // Print the info
    ATH_MSG_INFO(Form("%5zu. %-40s : %s",
                    compNum,
                    localComponent.name.Data(),
                    localComponent.desc.Data()
                    ));

    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Methods to implement from ISystematicsTool  //
//                                              //
//////////////////////////////////////////////////

bool JetUncertaintiesTool::isAffectedBySystematic(const CP::SystematicVariation& systematic) const
{
    // Compare using basenames to avoid continious vs fixed value comparisons
    const std::set<std::string> baseNames = m_recognizedSystematics.getBaseNames();
    return baseNames.find(systematic.basename()) != baseNames.end();
    //return m_recognizedSystematics.find(systematic) != m_recognizedSystematics.end();
}

CP::SystematicSet JetUncertaintiesTool::affectingSystematics() const
{
    return m_recognizedSystematics;
}

CP::SystematicSet JetUncertaintiesTool::recommendedSystematics() const
{
    return m_recommendedSystematics;
}

CP::SystematicSet JetUncertaintiesTool::appliedSystematics() const
{
    return m_currentSystSet;
}

CP::SystematicCode JetUncertaintiesTool::addAffectingSystematic(const CP::SystematicVariation& systematic, bool recommended)
{
    CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    registry.registerSystematic(systematic);
    m_recognizedSystematics.insert(systematic);
    if (recommended)
    {
        m_recommendedSystematics.insert(systematic);
        if (registry.addSystematicToRecommended(systematic) != CP::SystematicCode::Ok)
        {
            ATH_MSG_ERROR("Failed to add systematic to list of recommended systematics: " << systematic.name());
            return CP::SystematicCode::Unsupported;
        }
    }
    return CP::SystematicCode::Ok;
}

CP::SystematicCode JetUncertaintiesTool::applySystematicVariation(const CP::SystematicSet& systConfig)
{
    //if (!m_isInit)
    //{
    //    ATH_MSG_FATAL("Tool must be initialized before calling applySystematicVariation");
    //    return CP::SystematicCode::Unsupported;
    //}

    // Filter the full set of systematics to the set we care about
    CP::SystematicSet filteredSet;
    if (getFilteredSystematicSet(systConfig,filteredSet) != CP::SystematicCode::Ok)
        return CP::SystematicCode::Unsupported;

    // Get the uncertainty set associated to the filtered systematics set
    jet::UncertaintySet* uncSet = NULL;
    if (getUncertaintySet(filteredSet,uncSet) != CP::SystematicCode::Ok)
        return CP::SystematicCode::Unsupported;

    // Change the current state
    m_currentSystSet.swap(filteredSet);
    m_currentUncSet = uncSet;
    return CP::SystematicCode::Ok;
}

CP::SystematicCode JetUncertaintiesTool::getFilteredSystematicSet(const CP::SystematicSet& systConfig, CP::SystematicSet& filteredSet)
{
    // Check if we have already encountered this set
    std::unordered_map<CP::SystematicSet,CP::SystematicSet>::iterator iter = m_systFilterMap.find(systConfig);
    if (iter != m_systFilterMap.end())
        filteredSet = iter->second;
    // Make the filtered set and store it
    else
    {
        if (CP::SystematicSet::filterForAffectingSystematics(systConfig,m_recognizedSystematics,filteredSet) != CP::SystematicCode::Ok)
            return CP::SystematicCode::Unsupported;
        m_systFilterMap.insert(std::make_pair(systConfig,filteredSet));
    }

    return CP::SystematicCode::Ok;
}

CP::SystematicCode JetUncertaintiesTool::getUncertaintySet(const CP::SystematicSet& filteredSet, jet::UncertaintySet*& uncSet)
{
    // Check if we have already encountered this set
    std::unordered_map<CP::SystematicSet,UncertaintySet*>::iterator iter = m_systSetMap.find(filteredSet);

    // If we have dealt with this set previously, we're done
    if (iter != m_systSetMap.end())
    {
        uncSet = iter->second;
    }
    // Make the new set and store it
    else
    {
        uncSet = new UncertaintySet(filteredSet.name());
        if (uncSet == NULL || uncSet->initialize(filteredSet,m_components).isFailure())
        {
            ATH_MSG_ERROR("Failed to create UncertaintySet for filtered CP::SystematicSet: " << filteredSet.name());
            JESUNC_SAFE_DELETE(uncSet);
            return CP::SystematicCode::Unsupported;
        }
        m_systSetMap.insert(std::make_pair(filteredSet,uncSet));
    }
    
    return CP::SystematicCode::Ok;
}


//////////////////////////////////////////////////
//                                              //
//  Information retrieval methods               //
//                                              //
//////////////////////////////////////////////////

float JetUncertaintiesTool::getSqrtS() const
{
    float sqrtS = -1;
    const TString release = getRelease().c_str();
    if (release.BeginsWith("2011_"))
        sqrtS = 7000.*m_energyScale;
    else if (release.BeginsWith("2012_"))
        sqrtS = 8000.*m_energyScale;
    else if (release.BeginsWith("2015_"))
        sqrtS = 13000.*m_energyScale;
    return sqrtS;
}


float JetUncertaintiesTool::getRefMu() const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getRefMu");
        return JESUNC_ERROR_CODE;
    }
    if (m_refMuHist)
    {
        ATH_MSG_FATAL("Tool contains a histogram for refMu, cannot return float");
        return JESUNC_ERROR_CODE;
    }
    return m_refMu;
}

float JetUncertaintiesTool::getRefNPV() const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getRefNPV");
        return JESUNC_ERROR_CODE;
    }
    if (m_refNPVHist)
    {
        ATH_MSG_FATAL("Tool contains a histogram for refNPV, cannot return float");
        return JESUNC_ERROR_CODE;
    }
    return m_refNPV;
}

float JetUncertaintiesTool::getRefMu(const xAOD::Jet& jet) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getRefMu");
        return JESUNC_ERROR_CODE;
    }
    return m_refMuHist ? m_refMuHist->getUncertainty(fabs(jet.eta())) : m_refMu;
}

float JetUncertaintiesTool::getRefNPV(const xAOD::Jet& jet) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getRefNPV");
        return JESUNC_ERROR_CODE;
    }
    return m_refNPVHist ? m_refNPVHist->getUncertainty(fabs(jet.eta())) : m_refNPV;
}


size_t JetUncertaintiesTool::getNumComponents() const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getNumComponents");
        return 0;
    }

    return m_components.size();
}

size_t JetUncertaintiesTool::getComponentIndex(const std::string& name) const
{
    return getComponentIndex(TString(name.c_str()));
}

size_t JetUncertaintiesTool::getComponentIndex(const TString& name) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentIndex");
        return m_components.size();
    }

    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        if (m_components.at(iComp)->getName().CompareTo(name,TString::kIgnoreCase) == 0)
            return iComp;
    
    ATH_MSG_ERROR("Failed to find index for requested component: " << name.Data());
    return m_components.size();
}

std::string JetUncertaintiesTool::getComponentName(const size_t index) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentName");
        return "";
    }

    if (index < m_components.size())
        return m_components.at(index)->getName().Data();

    ATH_MSG_ERROR("Index out of bounds for component name: " << index);
    return "";
}

std::string JetUncertaintiesTool::getComponentDesc(const size_t index) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentDesc");
        return "";
    }

    if (index < m_components.size())
        return m_components.at(index)->getDesc().Data();

    ATH_MSG_ERROR("Index out of bounds for component desc: " << index);
    return "";
}

std::string JetUncertaintiesTool::getComponentCategory(const size_t index) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentCategory");
        return "";
    }

    if (index < m_components.size())
        return CompCategory::enumToString(m_components.at(index)->getCategory()).Data();

    ATH_MSG_ERROR("Index out of bounds for component category: " << index);
    return "";
}

bool JetUncertaintiesTool::getComponentIsReducible(const size_t index) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentIsReducible");
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getIsReducible();

    ATH_MSG_ERROR("Index out of bounds for component category:  " << index);
    return false;
}

bool JetUncertaintiesTool::getComponentScalesFourVec(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::FourVec;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::enumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::enumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesPt(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::Pt;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::enumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::enumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesMass(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::Mass;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::enumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::enumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesD12(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::D12;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::enumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::enumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesD23(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::D23;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::enumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::enumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesTau21(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::Tau21;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::enumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::enumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesTau32(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::Tau32;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::enumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::enumToString(scaleVar).Data() << ", index is " << index);
    return false;
}


bool JetUncertaintiesTool::getValidity(size_t index, const xAOD::Jet& jet) const
{
    const xAOD::EventInfo* eInfo = getDefaultEventInfo();
    if (!eInfo) return false;
    return getValidity(index,jet,*eInfo);
}
bool JetUncertaintiesTool::getValidity(size_t index, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getValidity");
        return false;
    }

    if (index < m_components.size())
        return m_components.at(index)->getValidity(jet,eInfo);

    ATH_MSG_ERROR("Index out of bounds for validity: " << index);
    return false;
}

double JetUncertaintiesTool::getUncertainty(size_t index, const xAOD::Jet& jet) const
{
    const xAOD::EventInfo* eInfo = getDefaultEventInfo();
    if (!eInfo) return JESUNC_ERROR_CODE;
    return getUncertainty(index,jet,*eInfo);
}
double JetUncertaintiesTool::getUncertainty(size_t index, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getUncertainty");
        return JESUNC_ERROR_CODE;
    }

    if (index < m_components.size())
        return m_components.at(index)->getUncertainty(jet,eInfo);
    
    ATH_MSG_ERROR("Index out of bounds for uncertainty: " << index);
    return JESUNC_ERROR_CODE;
}

bool JetUncertaintiesTool::getValidUncertainty(size_t index, double& unc, const xAOD::Jet& jet) const
{
    const xAOD::EventInfo* eInfo = getDefaultEventInfo();
    if (!eInfo) return false;
    return getValidUncertainty(index,unc,jet,*eInfo);
}
bool JetUncertaintiesTool::getValidUncertainty(size_t index, double& unc, const xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getValidUncertainty");
        return false;
    }

    if (index < m_components.size())
        return m_components.at(index)->getValidUncertainty(unc,jet,eInfo);

    ATH_MSG_ERROR("Index out of bounds for valid uncertainty: " << index);
    return false;
}


//////////////////////////////////////////////////
//                                              //
//  Mulit-component retrieval methods           //
//                                              //
//////////////////////////////////////////////////

std::vector<std::string> JetUncertaintiesTool::getComponentCategories() const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentCategories");
        return std::vector<std::string>();
    }

    // Internally use a set for speed
    // Use std::string rather than CompCategory::TypeEnum because std::string has a hash
    // Hashed access should mean there is no speed difference between using the two types
    std::unordered_set<std::string> categories;
    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        categories.insert(CompCategory::enumToString(m_components.at(iComp)->getCategory()).Data());
    
    // Convert the set to a vector
    std::vector<std::string> categoryStrings;
    for (std::unordered_set<std::string>::const_iterator iter = categories.begin() ; iter != categories.end(); ++iter)
        categoryStrings.push_back(*iter);

    return categoryStrings;
}

std::vector<size_t> JetUncertaintiesTool::getComponentsInCategory(const std::string& category) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentsInCategory");
        return std::vector<size_t>();
    }

    // Internally conver to an enum for both checking and speed of comparison
    const CompCategory::TypeEnum categoryEnum = CompCategory::stringToEnum(category.c_str());
    if (categoryEnum == CompCategory::UNKNOWN)
    {
        ATH_MSG_WARNING("Unrecognized category: " << category);
        return std::vector<size_t>();
    }

    // Now find the components
    std::vector<size_t> components;
    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        if (m_components.at(iComp)->getCategory() == categoryEnum)
            components.push_back(iComp);

    return components;
}

std::vector<std::string> JetUncertaintiesTool::getComponentNamesInCategory(const std::string& category) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentNamesInCategory");
        return std::vector<std::string>();
    }

    std::vector<size_t> components = getComponentsInCategory(category);
    std::vector<std::string> names;
    for (size_t iComp = 0; iComp < components.size(); ++iComp)
        names.push_back(getComponentName(components.at(iComp)));

    return names;
}


//////////////////////////////////////////////////
//                                              //
//  Methods to build the correlation matrix     //
//                                              //
//////////////////////////////////////////////////

TH2D* JetUncertaintiesTool::getPtCorrelationMatrix(const int numBins, const double minPt, const double maxPt, const double valEta)
{
    return getPtCorrelationMatrix(numBins,minPt,maxPt,valEta,valEta);
}

TH2D* JetUncertaintiesTool::getPtCorrelationMatrix(const int numBins, const double minPt, const double maxPt, const double valEta1, const double valEta2)
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getCorrelationMatrix");
        return NULL;
    }

    CorrelationMatrix corrMat(Form("%s_varpt_eta%.2f_eta%.2f",m_name.c_str(),valEta1,valEta2),numBins,minPt*m_energyScale,maxPt*m_energyScale,valEta1,valEta2);
    if (corrMat.initializeForPt(*this).isFailure())
        return NULL;
    return new TH2D(*corrMat.getMatrix());
}

TH2D* JetUncertaintiesTool::getEtaCorrelationMatrix(const int numBins, const double minEta, const double maxEta, const double valPt)
{
    return getEtaCorrelationMatrix(numBins,minEta,maxEta,valPt,valPt);
}

TH2D* JetUncertaintiesTool::getEtaCorrelationMatrix(const int numBins, const double minEta, const double maxEta, const double valPt1, const double valPt2)
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getCorrelationMatrix");
        return NULL;
    }

    CorrelationMatrix corrMat(Form("%s_vareta_pt%.1f_pt%.1f",m_name.c_str(),valPt1/1.e3,valPt2/1.e3),numBins,minEta,maxEta,valPt1*m_energyScale,valPt2*m_energyScale);
    if (corrMat.initializeForEta(*this).isFailure())
        return NULL;
    return new TH2D(*corrMat.getMatrix());
}


//////////////////////////////////////////////////
//                                              //
//  Methods to apply variations or get a copy   //
//                                              //
//////////////////////////////////////////////////

CP::CorrectionCode JetUncertaintiesTool::applyCorrection(xAOD::Jet& jet) const
{
    const xAOD::EventInfo* eInfo = getDefaultEventInfo();
    if (!eInfo) return CP::CorrectionCode::Error;
    return applyCorrection(jet,*eInfo);
}

CP::CorrectionCode JetUncertaintiesTool::applyCorrection(xAOD::Jet& jet, const xAOD::EventInfo& eInfo) const
{
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling applyCorrection");
        return CP::CorrectionCode::Error;
    }

    // Scale the jet and/or its moments by the uncertainty/uncertainties
    // Note that uncertainties may be either positive or negative
    // Make sure to check the validity at the same time
    std::vector< std::pair<CompScaleVar::TypeEnum,double> > uncSet;
    const std::vector< std::pair<CompScaleVar::TypeEnum,bool> > validitySet = m_currentUncSet->getValidUncertaintySet(uncSet,jet,eInfo);

    // Ensure every case was successful
    bool allValid = true;
    for (size_t iVar = 0; iVar < validitySet.size(); ++iVar)
    {
        const CompScaleVar::TypeEnum scaleVar = validitySet.at(iVar).first;
        const bool validity = validitySet.at(iVar).second;

        if (!validity)
        {
            allValid = false;
            ATH_MSG_ERROR("Uncertainty configuration is not valid for the specified jet when attempting to scale " << CompScaleVar::enumToString(scaleVar).Data() << ".  Set: " << m_currentUncSet->getName());
        }
    }
    if (!allValid)
        return CP::CorrectionCode::Error;
    
    // Handle each case as needed
    for (size_t iVar = 0; iVar < uncSet.size(); ++iVar)
    {
        const CompScaleVar::TypeEnum scaleVar = uncSet.at(iVar).first;
        //const double unc = uncSet.at(iVar).second;
        const double shift = 1 + uncSet.at(iVar).second;
        
        // Careful of const vs non-const objects with accessors
        // Can unintentionally create something new which didn't exist, as jet is non-const
        const xAOD::Jet* constJet = &jet;
        xAOD::JetFourMom_t shifted;
        float origValue;
        switch (scaleVar)
        {
            case CompScaleVar::FourVec:
                shifted = xAOD::JetFourMom_t(shift*jet.pt(),jet.eta(),jet.phi(),shift*jet.m());
                jet.setJetP4(shifted);
                break;
            case CompScaleVar::Pt:
                shifted = xAOD::JetFourMom_t(shift*jet.pt(),jet.eta(),jet.phi(),jet.m());
                jet.setJetP4(shifted);
                break;
            case CompScaleVar::Mass:
                shifted = xAOD::JetFourMom_t(jet.pt(),jet.eta(),jet.phi(),shift*jet.m());
                jet.setJetP4(shifted);
                break;
            case CompScaleVar::D12:
                origValue = m_D12Accessor(*constJet);
                m_D12Accessor(jet) = shift*origValue;
                break;
            case CompScaleVar::D23:
                origValue = m_D23Accessor(*constJet);
                m_D23Accessor(jet) = shift*origValue;
                break;
            case CompScaleVar::Tau21:
                origValue = m_Tau21Accessor.isAvailable(*constJet) ? m_Tau21Accessor(*constJet) : m_Tau2Accessor(*constJet)/m_Tau1Accessor(*constJet);
                m_Tau21Accessor(jet) = shift*origValue;
                break;
            case CompScaleVar::Tau32:
                origValue = m_Tau32Accessor.isAvailable(*constJet) ? m_Tau32Accessor(*constJet) : m_Tau3Accessor(*constJet)/m_Tau2Accessor(*constJet);
                m_Tau32Accessor(jet) = shift*origValue;
                break;
            default:
                ATH_MSG_ERROR("Asked to scale an UNKNOWN variable for set: " << m_currentUncSet->getName());
                return CP::CorrectionCode::Error;
        }
    }

    return CP::CorrectionCode::Ok;
}

CP::CorrectionCode JetUncertaintiesTool::correctedCopy(const xAOD::Jet& input, xAOD::Jet*& output) const
{
    const xAOD::EventInfo* eInfo = getDefaultEventInfo();
    if (!eInfo) return CP::CorrectionCode::Error;
    return correctedCopy(input,output,*eInfo);
}

CP::CorrectionCode JetUncertaintiesTool::correctedCopy(const xAOD::Jet& input, xAOD::Jet*& output, const xAOD::EventInfo& eInfo) const
{
    xAOD::Jet* copy = new xAOD::Jet(input);
    
    // Call the implemented function
    if (applyCorrection(*copy,eInfo) != CP::CorrectionCode::Ok)
    {
        delete copy;
        return CP::CorrectionCode::Error;
    }
    output = copy;
    return CP::CorrectionCode::Ok;
}

CP::CorrectionCode JetUncertaintiesTool::applyContainerCorrection(xAOD::JetContainer& inputs) const
{
    const xAOD::EventInfo* eInfo = getDefaultEventInfo();
    if (!eInfo) return CP::CorrectionCode::Error;
    return applyContainerCorrection(inputs,*eInfo);
}

CP::CorrectionCode JetUncertaintiesTool::applyContainerCorrection(xAOD::JetContainer& inputs, const xAOD::EventInfo& eInfo) const
{
    CP::CorrectionCode result = CP::CorrectionCode::Ok;

    // Loop over the container
    for (size_t iJet = 0; iJet < inputs.size(); ++iJet)
    {
        result = applyCorrection(*inputs.at(iJet),eInfo);
        if (result == CP::CorrectionCode::Error)
            break;
    }
    return result;
}

const xAOD::EventInfo* JetUncertaintiesTool::getDefaultEventInfo() const
{
    // Define static EventInfo objects
    // Unfortunately this is messy, but needed as we are caching across tool calls
    // Using voltatile class variables doesn't work well as we need to return a const object
    // Interesting enough, the shallow copy link is updated when evtStore()->retrieve() is called
    // As such, just retrieving the new EventInfo object updates this copy
    // We therefore need to also store our own local copy of the eventNumber
    static xAOD::EventInfo*           eInfoObj = NULL;
    static xAOD::ShallowAuxContainer* eInfoAux = NULL;
    static unsigned long long         eventNum = 0;

    // Retrieve the EventInfo object
    const xAOD::EventInfo* eInfoConst = NULL;
    if (evtStore()->retrieve(eInfoConst,"EventInfo").isFailure())
    {
        ATH_MSG_ERROR("Failed to retrieve default EventInfo object");
        return NULL;
    }
    
    // Check if this is a new event or if we can re-use the existing EventInfo object
    if (eInfoObj && eventNum == eInfoConst->eventNumber())
        return eInfoObj;
    eventNum = eInfoConst->eventNumber();

    // It's a new event, get rid of the old object and build a new one
    JESUNC_SAFE_DELETE(eInfoObj);
    JESUNC_SAFE_DELETE(eInfoAux);

    // Make a shallow copy
    std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> eInfoPair = xAOD::shallowCopyObject(*eInfoConst);
    eInfoObj = eInfoPair.first;
    eInfoAux = eInfoPair.second;

    // Check if NPV already exists on const EventInfo object, return if so
    if (m_NPVAccessor.isAvailable(*eInfoConst))
        return eInfoObj;

    // NPV doesn't already exist, so calculate it
    const xAOD::VertexContainer* vertices = NULL;
    if (evtStore()->retrieve(vertices,"PrimaryVertices").isFailure())
    {
        ATH_MSG_ERROR("Failed to retrieve default NPV value from PrimaryVertices");
        JESUNC_SAFE_DELETE(eInfoObj);
        JESUNC_SAFE_DELETE(eInfoAux);
        return NULL;
    }

    unsigned NPV = 0;
    xAOD::VertexContainer::const_iterator itr;
    for (itr = vertices->begin(); itr != vertices->end(); ++itr)
        if ( (*itr)->nTrackParticles() > 1)
            NPV++;

    // Add NPV to the shallow copy EventInfo object
    m_NPVAccessor(*eInfoObj) = NPV;

    // Done, return EventInfo decorated with NPV
    return eInfoObj;
}



