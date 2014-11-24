/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// General package includes
#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetUncertainties/Helpers.h"
#include "JetUncertainties/UncertaintyEnum.h"
#include "JetUncertainties/ComponentHelper.h"

// UncertaintyComponent types
#include "JetUncertainties/UncertaintyComponent.h"
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
    , m_jetDef("")
    , m_mcType("")
    , m_configFile("")
    , m_path("")
    , m_analysisFile("")
    , m_refNPV(-1)
    , m_refMu(-1)
    , m_components()
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
    , m_jetDef(toCopy.m_jetDef)
    , m_mcType(toCopy.m_mcType)
    , m_configFile(toCopy.m_configFile)
    , m_path(toCopy.m_path)
    , m_analysisFile(toCopy.m_analysisFile)
    , m_refNPV(toCopy.m_refNPV)
    , m_refMu(toCopy.m_refMu)
    , m_components()
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

    if (applySystematicVariation(m_currentSystSet) != CP::SystematicCode::Ok)
        ATH_MSG_ERROR(Form("Failed to re-set applySystematicVariation in new tool copy"));
}

JetUncertaintiesTool::~JetUncertaintiesTool()
{
    ATH_MSG_DEBUG(Form("Deleting JetUncertaintiesTool named %s",m_name.c_str()));
    
    for (size_t iComp = 0; iComp < m_components.size(); ++iComp)
        JESUNC_SAFE_DELETE(m_components.at(iComp));
    m_components.clear();

    m_currentUncSet  = NULL;
    
    m_systFilterMap.clear();

    boost::unordered_map<CP::SystematicSet,UncertaintySet*>::iterator iter;
    for (iter = m_systSetMap.begin(); iter != m_systSetMap.end(); ++iter)
        JESUNC_SAFE_DELETE(iter->second);
    m_systSetMap.clear();
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
    const TString configFilePath = jet::utils::FindFilePath(m_configFile.c_str(),m_path.c_str());
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
    
    
    // Check the jet definition
    TString allowedJetDefStr = settings.GetValue("SupportedJetDefs","");
    if (allowedJetDefStr == "")
    {
        ATH_MSG_ERROR("Cannot find supported jet definitions in config");
        return StatusCode::FAILURE;
    }
    std::vector<TString> allowedJetDefs = jet::utils::Vectorize<TString>(allowedJetDefStr," ,");
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
    std::vector<TString> allowedMCtypes = jet::utils::Vectorize<TString>(allowedMCtypeStr," ,");
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
    
    // Get the NPV/mu reference values
    // These may not be set - only needed if a pileup component is requested
    m_refNPV = settings.GetValue("Pileup.NPVRef",-1.0);
    m_refMu  = settings.GetValue("Pileup.MuRef",-1.0);
    if (m_refMu > 0 && m_refNPV > 0)
        ATH_MSG_INFO(Form("  Pileup reference: (NPV,mu) = (%.1f,%.1f)",m_refNPV,m_refMu));
    else if ( (m_refMu > 0 && m_refNPV < 0) || (m_refMu < 0 && m_refNPV > 0) )
    {
        ATH_MSG_ERROR(Form("Only one of the pileup references was specified: (NPV,mu) = (%.1f,%.1f)",m_refNPV,m_refMu));
        return StatusCode::FAILURE;
    }
    

    
    // Now open the histogram file
    TFile* histFile = utils::ReadRootFile(histFileName,m_path.c_str());
    if (!histFile || histFile->IsZombie())
    {
        ATH_MSG_ERROR("Cannot open uncertainty histogram file: " << histFileName.Data());
        return StatusCode::FAILURE;
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
        const ComponentHelper component(settings,prefix,m_mcType.c_str());

        // Ignore component if it is not defined
        if (component.name == "")
            continue;
        
        // Call the uncertainty component helper method to add a new component
        if(addUncertaintyComponent(histFile,component).isFailure())
            return StatusCode::FAILURE;
    }
    ATH_MSG_INFO(Form("   Found and read in %zu parameters",m_components.size()));
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
    //      1. Default, no split
    //      2. Split into two components
    //          - linear increase vs log(pT)
    //          - quadratic complement
    //      Otherwise, this is currently unsupported
    if (component.numSplit != 1 and component.numSplit != 2)
    {
        ATH_MSG_ERROR("Only splits of 1 and 2 are currently supported, not " << component.numSplit << ", for component " << component.name.Data());
        return StatusCode::FAILURE;
    }

    // Now loop to produce the required number of components
    for (int iSplit = 1; iSplit <= component.numSplit; ++iSplit)
    {
        // Add the split number to the name if necessary
        TString localCompName = component.numSplit > 1 ? Form("%s%d",component.name.Data(),iSplit) : component.name.Data();
        TString localCompDesc = component.numSplit > 1 ? Form("%s (part %d of %d)",component.desc.Data(),iSplit,component.numSplit) : component.desc.Data();

        // Prepend the jet prefix if necessary
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
                if (m_refNPV < 0)
                {
                    ATH_MSG_ERROR("Attempted to create pileup component without NPV reference value: " << localCompName.Data());
                    return StatusCode::FAILURE;
                }
                if (m_refMu < 0)
                {
                    ATH_MSG_ERROR("Attempted to create pileup component without mu reference value: " << localCompName.Data());
                    return StatusCode::FAILURE;
                }

                if (component.parametrization == CompParametrization::PtEta || component.parametrization == CompParametrization::PtAbsEta)
                    m_components.push_back(new PileupUncertaintyComponent(localComponent,m_refNPV,m_refMu));
                else
                {
                    ATH_MSG_ERROR(Form("Unexpected parametrization of %s for component %s",CompParametrization::EnumToString(component.parametrization).Data(),localCompName.Data()));
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
                    ATH_MSG_ERROR(Form("Unexpected parametrization of %s for component %s",CompParametrization::EnumToString(component.parametrization).Data(),localCompName.Data()));
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
                    ATH_MSG_ERROR(Form("Unexpected parametrization of %s for component %s",CompParametrization::EnumToString(component.parametrization).Data(),localCompName.Data()));
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
                    ATH_MSG_ERROR(Form("Unexpected parametrization of %s for component %s",CompParametrization::EnumToString(component.parametrization).Data(),localCompName.Data()));
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
        
        // Set the split factor if applicable
        // This must be done before initialization
        if (component.numSplit > 1 && m_components.back()->SetSplitFactor(iSplit).isFailure())
            return StatusCode::FAILURE;
        
        // Add histogram(s) to components and initialize
        // No specified histograms == use the name of the component for the histogram name
        // Note: use the raw name without the split number, not the local name
        std::vector<TString> histNames;
        if (component.hists == "")
            histNames.push_back(component.name);
        else
        {
            const bool success = utils::Vectorize<TString>(component.hists,", ",histNames);
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
            const bool success = utils::Vectorize<TString>(component.validHists,", ",validHistNames);
            if (!success)
            {
                ATH_MSG_ERROR("Failed to convert validHists into a vector: " << component.validHists.Data());
                return StatusCode::FAILURE;
            }
        }

        // Now initialize
        if (!validHistNames.size())
        {
            if (m_components.back()->Initialize(histNames,histFile).isFailure())
                return StatusCode::FAILURE;
        }
        else
        {
            if (m_components.back()->Initialize(histNames,validHistNames,histFile).isFailure())
                return StatusCode::FAILURE;
        }

        // Add the component to the register of what can be applied
        CP::SystematicVariation systVar(localCompName.Data(),CP::SystematicVariation::CONTINUOUS);
        if (addAffectingSystematic(systVar,true) != CP::SystematicCode::Ok)
            return StatusCode::FAILURE;
    
    
        // Print the info
        ATH_MSG_INFO(Form("%5zu. %-35s : %s",
                        m_components.size()-component.numSplit+iSplit,
                        localComponent.name.Data(),
                        localComponent.desc.Data()
                        ));
    }

    return StatusCode::SUCCESS;
}


//////////////////////////////////////////////////
//                                              //
//  Methods to implement from ISystematicsTool  //
//                                              //
//////////////////////////////////////////////////

bool JetUncertaintiesTool::isAffectedBySystematic(const CP::SystematicVariation& systematic) const
{
    return m_recognizedSystematics.find(systematic) != m_recognizedSystematics.end();
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
    boost::unordered_map<CP::SystematicSet,CP::SystematicSet>::iterator iter = m_systFilterMap.find(systConfig);
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
    boost::unordered_map<CP::SystematicSet,UncertaintySet*>::iterator iter = m_systSetMap.find(filteredSet);

    // If we have dealt with this set previously, we're done
    if (iter != m_systSetMap.end())
    {
        uncSet = iter->second;
    }
    // Make the new set and store it
    else
    {
        uncSet = new UncertaintySet(filteredSet.name());
        if (uncSet == NULL || uncSet->Initialize(filteredSet,m_components).isFailure())
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
//  Methods to implement from this interface    //
//                                              //
//////////////////////////////////////////////////

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

bool JetUncertaintiesTool::getComponentScalesFourVec(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::FourVec;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::EnumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::EnumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesPt(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::Pt;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::EnumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::EnumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesMass(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::Mass;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::EnumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::EnumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesD12(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::D12;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::EnumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::EnumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesD23(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::D23;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::EnumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::EnumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesTau21(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::Tau21;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::EnumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::EnumToString(scaleVar).Data() << ", index is " << index);
    return false;
}
bool JetUncertaintiesTool::getComponentScalesTau32(const size_t index) const
{
    const CompScaleVar::TypeEnum scaleVar = CompScaleVar::Tau32;
    if (!m_isInit)
    {
        ATH_MSG_FATAL("Tool must be initialized before calling getComponentScales"<<CompScaleVar::EnumToString(scaleVar).Data());
        return false;
    }
    
    if (index < m_components.size())
        return m_components.at(index)->getScaleVar() == scaleVar;

    ATH_MSG_ERROR("Index out of bounds for component scales " << CompScaleVar::EnumToString(scaleVar).Data() << ", index is " << index);
    return false;
}


bool JetUncertaintiesTool::getValidity(size_t index, const xAOD::Jet& jet) const
{
    const std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> eInfoPair = getDefaultEventInfo();
    if (!eInfoPair.first) return false;
    const bool validity = getValidity(index,jet,*(eInfoPair.first));
    delete eInfoPair.first;
    delete eInfoPair.second;
    return validity;
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
    const std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> eInfoPair = getDefaultEventInfo();
    if (!eInfoPair.first) return JESUNC_ERROR_CODE;
    const double unc = getUncertainty(index,jet,*(eInfoPair.first));
    delete eInfoPair.first;
    delete eInfoPair.second;
    return unc;
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
    const std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> eInfoPair = getDefaultEventInfo();
    if (!eInfoPair.first) return false;
    const bool validity = getValidUncertainty(index,unc,jet,*(eInfoPair.first));
    delete eInfoPair.first;
    delete eInfoPair.second;
    return validity;
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
//  Methods to apply variations or get a copy   //
//                                              //
//////////////////////////////////////////////////

CP::CorrectionCode JetUncertaintiesTool::applyCorrection(xAOD::Jet& jet) const
{
    const std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> eInfoPair = getDefaultEventInfo();
    if (!eInfoPair.first) return CP::CorrectionCode::Error;
    const CP::CorrectionCode code = applyCorrection(jet,*(eInfoPair.first));
    delete eInfoPair.first;
    delete eInfoPair.second;
    return code;
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
            ATH_MSG_ERROR("Uncertainty configuration is not valid for the specified jet when attempting to scale " << CompScaleVar::EnumToString(scaleVar).Data() << ".  Set: " << m_currentUncSet->getName());
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
    const std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> eInfoPair = getDefaultEventInfo();
    if (!eInfoPair.first) return CP::CorrectionCode::Error;
    const CP::CorrectionCode code = correctedCopy(input,output,*(eInfoPair.first));
    delete eInfoPair.first;
    delete eInfoPair.second;
    return code;
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
    const std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> eInfoPair = getDefaultEventInfo();
    if (!eInfoPair.first) return CP::CorrectionCode::Error;
    const CP::CorrectionCode code = applyContainerCorrection(inputs,*(eInfoPair.first));
    delete eInfoPair.first;
    delete eInfoPair.second;
    return code;
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

std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> JetUncertaintiesTool::getDefaultEventInfo() const
{
    const xAOD::EventInfo* eInfoConst = NULL;
    if (evtStore()->retrieve(eInfoConst,"EventInfo").isFailure())
    {
        ATH_MSG_ERROR("Failed to retrieve default EventInfo object");
        return std::make_pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*>(NULL,NULL);
    }

    // Make a shallow copy
    std::pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*> eInfoPair = xAOD::shallowCopyObject(*eInfoConst);
    xAOD::EventInfo* eInfo = eInfoPair.first;

    // Calculate NPV
    const xAOD::VertexContainer* vertices = NULL;
    if (evtStore()->retrieve(vertices,"PrimaryVertices").isFailure())
    {
        ATH_MSG_ERROR("Failed to retrieve default NPV value from PrimaryVertices");
        delete eInfoPair.first;
        delete eInfoPair.second;
        return std::make_pair<xAOD::EventInfo*,xAOD::ShallowAuxContainer*>(NULL,NULL);
    }

    unsigned NPV = 0;
    xAOD::VertexContainer::const_iterator itr;
    for (itr = vertices->begin(); itr != vertices->end(); ++itr)
        if ( (*itr)->nTrackParticles() > 1)
            NPV++;

    // Add NPV to the shallow copy EventInfo object
    m_NPVAccessor(*eInfo) = NPV;

    // Done, return EventInfo decorated with NPV
    return eInfoPair;
}



