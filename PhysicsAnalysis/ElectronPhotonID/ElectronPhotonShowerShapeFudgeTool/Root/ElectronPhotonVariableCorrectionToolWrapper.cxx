/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionToolWrapper.h"
#include "AsgElectronPhotonCorrectionConfigHelper.h"

// ===========================================================================
// Standard Constructor
// ===========================================================================
ElectronPhotonVariableCorrectionToolWrapper::ElectronPhotonVariableCorrectionToolWrapper(const std::string& myname) :
    AsgTool(myname)
{
    //declare the needed properties
    declareProperty("ConfigFile",m_configFile="", "The configuration file to use");
}

// ===========================================================================
// Standard Destructor
// ===========================================================================
ElectronPhotonVariableCorrectionToolWrapper::~ElectronPhotonVariableCorrectionToolWrapper()
{
    //check status code of finalize
    if(finalize().isFailure())
    {
        ATH_MSG_ERROR("In " << name() << ": Failure in ElectronPhotonVariableCorrectionTool finalize()");
    }
}

// ===========================================================================
// Initialize and Finalize
// ===========================================================================
StatusCode ElectronPhotonVariableCorrectionToolWrapper::finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode ElectronPhotonVariableCorrectionToolWrapper::initialize()
{
    // Locate configuration file, abort if not found
    std::string configFile;
    if (!m_configFile.empty())
    {
        configFile = PathResolverFindCalibFile(m_configFile);
        if (configFile == "")
        {
            ATH_MSG_ERROR("In " << name() << ": Could not locate configuration file " << m_configFile);
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("Use configuration file " << m_configFile);
    }
    else
    {
        ATH_MSG_ERROR("In " << name() << ": Config file string is empty. Please provide a config file to the tool.");
        return StatusCode::FAILURE;
    }

    // Retreive properties from configuration file, using TEnv class
    TEnv env(configFile.c_str());
    // Send warning if duplicates found in conf file
    env.IgnoreDuplicates(false);

    // retrieve different object type conf files
    if (env.Lookup("ElectronConfigs"))
    {
        m_electronConfFiles = AsgConfigHelper::HelperString("ElectronConfigs",env);
    }
    if (env.Lookup("ConvertedPhotonConfigs"))
    {
        m_convertedPhotonConfFiles = AsgConfigHelper::HelperString("ConvertedPhotonConfigs",env);
    }
    if (env.Lookup("UnconvertedPhotonConfigs"))
    {
        m_unconvertedPhotonConfFiles = AsgConfigHelper::HelperString("UnconvertedPhotonConfigs",env);
    }

    // check if any conf files were received
    if (m_electronConfFiles.size() + m_convertedPhotonConfFiles.size() + m_unconvertedPhotonConfFiles.size() < 1)
    {
        ANA_MSG_ERROR("You did not provide any config files for the ElectronPhotonVariableCorrectionTool to the ElectronPhotonVariableCorrectionToolWrapper.");
        return StatusCode::FAILURE;
    }

    // initialize the ElectronPhotonVariableCorrectionTools
    ANA_CHECK(InitializeCorrectionTools());

    ANA_MSG_INFO("Initialized tool " << name());
    //everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionToolWrapper::InitializeCorrectionTools()
{
    //find all the config files using path resolver
    ANA_CHECK(FindAllConfigFiles(m_convertedPhotonConfFiles));
    ANA_CHECK(FindAllConfigFiles(m_unconvertedPhotonConfFiles));
    ANA_CHECK(FindAllConfigFiles(m_electronConfFiles));
    //initialize all tools
    ANA_CHECK(InitializeTools("unconvertedPhotons", m_convertedPhotonConfFiles, m_convertedPhotonTools));
    ANA_CHECK(InitializeTools("convertedPhotons", m_unconvertedPhotonConfFiles, m_unconvertedPhotonTools));
    ANA_CHECK(InitializeTools("electrons", m_electronConfFiles, m_electronTools));
    // check if ApplyTo Flag matches with the tool holder
    ANA_CHECK(ApplyToFlagMatchesToolHolder(m_convertedPhotonConfFiles, m_convertedPhotonTools, ElectronPhotonVariableCorrectionTool::EGammaObjects::convertedPhotons));
    ANA_CHECK(ApplyToFlagMatchesToolHolder(m_unconvertedPhotonConfFiles, m_unconvertedPhotonTools, ElectronPhotonVariableCorrectionTool::EGammaObjects::unconvertedPhotons));
    ANA_CHECK(ApplyToFlagMatchesToolHolder(m_electronConfFiles, m_electronTools, ElectronPhotonVariableCorrectionTool::EGammaObjects::allElectrons));

    //everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionToolWrapper::FindAllConfigFiles( std::vector<std::string>& confFiles )
{
    //loop over conf file vector, find conf file using path resolver
    for( unsigned int confFile_itr = 0; confFile_itr < confFiles.size(); confFile_itr++ )
    {
        std::string tmp_confFile = confFiles.at(confFile_itr);
        confFiles.at(confFile_itr) = PathResolverFindCalibFile(confFiles.at(confFile_itr));
        if (confFiles.at(confFile_itr) == "")
        {
            ATH_MSG_ERROR("In " << name()  << ": Could not locate configuration file " << tmp_confFile);
            return StatusCode::FAILURE;
        }
    }
    //everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionToolWrapper::InitializeTools( const std::string& name, const std::vector<std::string>& confFiles, std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionTool>>& toolHolder )
{
    // adapt size of toolHolder
    toolHolder.resize(confFiles.size());
    // for each conf file, initialize one tool
    for( unsigned int confFile_itr = 0; confFile_itr < confFiles.size(); confFile_itr++ )
    {
        // name: supertool name + type name + variable name
        std::string variable = ""; //get the name of the variable to be corrected
        ANA_CHECK(GetCorrectionVariableName(variable, confFiles.at(confFile_itr)));
        TString toolname = TString::Format("%s_%s_%s", this->name().c_str(), name.c_str(), variable.c_str());
        ANA_MSG_DEBUG("Subtool name: " << toolname.Data());
        toolHolder.at(confFile_itr) = std::make_unique<ElectronPhotonVariableCorrectionTool>(toolname.Data());
        ANA_CHECK(toolHolder.at(confFile_itr)->setProperty("ConfigFile", confFiles.at(confFile_itr)));
        ANA_CHECK(toolHolder.at(confFile_itr)->initialize());
    }
    //everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionToolWrapper::ApplyToFlagMatchesToolHolder( const std::vector<std::string>& confFiles, const std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionTool>>& toolHolder, ElectronPhotonVariableCorrectionTool::EGammaObjects toolHolderType )
{
    // loop over conf file holder
    for (unsigned int tool_itr = 0; tool_itr < toolHolder.size(); tool_itr++)
    {
        // get ApplyTo flag
        ElectronPhotonVariableCorrectionTool::EGammaObjects confFileType = toolHolder.at(tool_itr)->IsAppliedTo();
        // skip all further tests if should be applied to all objects
        if (!(confFileType == ElectronPhotonVariableCorrectionTool::EGammaObjects::allEGammaObjects))
        {
            // if tool for converted photons, but not the conf file, fail
            if (toolHolderType == ElectronPhotonVariableCorrectionTool::EGammaObjects::convertedPhotons)
            {
                if (!(confFileType == ElectronPhotonVariableCorrectionTool::EGammaObjects::convertedPhotons || confFileType == ElectronPhotonVariableCorrectionTool::EGammaObjects::allPhotons))
                {
                    ATH_MSG_ERROR("In " << name() << ": In conf " << confFiles.at(tool_itr) << ": You specified tool should be applied to converted photons in the wrapper, but the ApplyTo flag doesn't match.");
                    return StatusCode::FAILURE;
                }
            }
            // if tool for unconverted photons, but not the conf file, fail
            if (toolHolderType == ElectronPhotonVariableCorrectionTool::EGammaObjects::unconvertedPhotons)
            {
                if (!(confFileType == ElectronPhotonVariableCorrectionTool::EGammaObjects::unconvertedPhotons || confFileType == ElectronPhotonVariableCorrectionTool::EGammaObjects::allPhotons))
                {
                    ATH_MSG_ERROR("In " << name() << ": In conf " << confFiles.at(tool_itr) << ": You specified tool should be applied to unconverted photons in the wrapper, but the ApplyTo flag doesn't match.");
                    return StatusCode::FAILURE;
                }
            }
            // if tool for electrons, but not the conf file, fail
            if (toolHolderType == ElectronPhotonVariableCorrectionTool::EGammaObjects::allElectrons)
            {
                if (!(confFileType == ElectronPhotonVariableCorrectionTool::EGammaObjects::allElectrons))
                {
                    ATH_MSG_ERROR("In " << name() << ": In conf " << confFiles.at(tool_itr) << ": You specified tool should be applied to electrons in the wrapper, but the ApplyTo flag doesn't match.");
                    return StatusCode::FAILURE;
                }
            }
        }
    }
    //everything worked out, so
    return StatusCode::SUCCESS;
}

/* ========================================
 * Apply correction
 * ======================================== */

const StatusCode ElectronPhotonVariableCorrectionToolWrapper::applyCorrection( xAOD::Photon& photon )
{
    bool isConvertedPhoton = xAOD::EgammaHelpers::isConvertedPhoton(&photon);

    // check if need to run over converted or unconverted photons
    if (isConvertedPhoton)
    {
        // correct variables on the converted photon
        for (unsigned int convertedPhotonTool_itr = 0; convertedPhotonTool_itr < m_convertedPhotonTools.size(); convertedPhotonTool_itr++)
        {
            ANA_CHECK(m_convertedPhotonTools.at(convertedPhotonTool_itr)->applyCorrection(photon));
        }
    }
    else
    {
        // correct variables on the converted photon
        for (unsigned int unconvertedPhotonTool_itr = 0; unconvertedPhotonTool_itr < m_unconvertedPhotonTools.size(); unconvertedPhotonTool_itr++)
        {
            ANA_CHECK(m_unconvertedPhotonTools.at(unconvertedPhotonTool_itr)->applyCorrection(photon));
        }
    }
    
    // everything worked out, so
    return StatusCode::SUCCESS;
}
const StatusCode ElectronPhotonVariableCorrectionToolWrapper::applyCorrection( xAOD::Electron& electron )
{
    // correct variables on the electron
    for (unsigned int electronTool_itr = 0; electronTool_itr < m_electronTools.size(); electronTool_itr++)
    {
        ANA_CHECK(m_electronTools.at(electronTool_itr)->applyCorrection(electron));
    }
    // everything worked out, so
    return StatusCode::SUCCESS;
}

/* ========================================
 * Corrected Copy
 * ======================================== */

const StatusCode ElectronPhotonVariableCorrectionToolWrapper::correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon )
{
    out_photon = new xAOD::Photon(in_photon);
    return applyCorrection(*out_photon);
}

const StatusCode ElectronPhotonVariableCorrectionToolWrapper::correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron)
{
    out_electron = new xAOD::Electron(in_electron);
    return applyCorrection(*out_electron);
}

/* ========================================
 * Helper functions
 * ======================================== */

const StatusCode ElectronPhotonVariableCorrectionToolWrapper::GetCorrectionVariableName( std::string &variableName, const std::string& confFile ) const
{
    // Retreive properties from configuration file, using TEnv class
    TEnv env(confFile.c_str());
    // Send warning if duplicates found in conf file
    env.IgnoreDuplicates(false);

    // retrieve variable name
    if (env.Lookup("Variable"))
    {
        variableName = env.GetValue("Variable", "");
    }
    else
    {
        ATH_MSG_ERROR("In " << name() << ": In conf file " << confFile << ": Correction variable is empty or not in configuration file.");
        return StatusCode::FAILURE;
    }
    //everything worked out, so
    return StatusCode::SUCCESS;
}
