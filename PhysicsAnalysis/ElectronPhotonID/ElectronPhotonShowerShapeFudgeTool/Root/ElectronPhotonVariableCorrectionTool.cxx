/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class ElectronPhotonVariableCorrectionTool
   @brief Tool to correct electron and photon MC variables.

   @author Nils Gillwald (DESY) nils.gillwald@desy.de
   @date   February 2020
**/

#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionTool.h"
#include "EgammaAnalysisHelpers/AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "PathResolver/PathResolver.h"
#include "TEnv.h"

// ===========================================================================
// Standard Constructor
// ===========================================================================
ElectronPhotonVariableCorrectionTool::ElectronPhotonVariableCorrectionTool(const std::string& myname) :
    AsgTool(myname)
{
    //declare the needed properties
    declareProperty("ConfigFile",m_configFile="", "The configuration file to use");
}

StatusCode ElectronPhotonVariableCorrectionTool::initialize()
{
    // Locate configuration file, abort if not found
    std::string configFile;
    if (!m_configFile.empty())
    {
        configFile = PathResolverFindCalibFile(m_configFile);
        if (configFile == "")
        {
            ATH_MSG_ERROR("Could not locate configuration file " << m_configFile);
            return StatusCode::FAILURE;
        }
        ATH_MSG_DEBUG("Use configuration file " << m_configFile);
    }
    else
    {
        ATH_MSG_ERROR("Config file string is empty. Please provide a config file to the tool.");
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
        ANA_MSG_ERROR("You did not provide any config files for the ElectronPhotonVariableCorrectionBase to the ElectronPhotonVariableCorrectionTool.");
        return StatusCode::FAILURE;
    }

    // initialize the ElectronPhotonVariableCorrectionTools
    ANA_CHECK(initializeCorrectionTools());

    ANA_MSG_INFO("Initialized tool " << name());
    //everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionTool::initializeCorrectionTools()
{
    //find all the config files using path resolver
    ANA_CHECK(findAllConfigFiles(m_convertedPhotonConfFiles));
    ANA_CHECK(findAllConfigFiles(m_unconvertedPhotonConfFiles));
    ANA_CHECK(findAllConfigFiles(m_electronConfFiles));
    //initialize all tools
    ANA_CHECK(initializeTools("convertedPhotons", m_convertedPhotonConfFiles, m_convertedPhotonTools));
    ANA_CHECK(initializeTools("unconvertedPhotons", m_unconvertedPhotonConfFiles, m_unconvertedPhotonTools));
    ANA_CHECK(initializeTools("electrons", m_electronConfFiles, m_electronTools));
    // check if ApplyTo Flag matches with the tool holder
    ANA_CHECK(applyToFlagMatchesToolHolder(m_convertedPhotonConfFiles, m_convertedPhotonTools, ElectronPhotonVariableCorrectionBase::EGammaObjects::convertedPhotons));
    ANA_CHECK(applyToFlagMatchesToolHolder(m_unconvertedPhotonConfFiles, m_unconvertedPhotonTools, ElectronPhotonVariableCorrectionBase::EGammaObjects::unconvertedPhotons));
    ANA_CHECK(applyToFlagMatchesToolHolder(m_electronConfFiles, m_electronTools, ElectronPhotonVariableCorrectionBase::EGammaObjects::allElectrons));

    //everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionTool::findAllConfigFiles( std::vector<std::string>& confFiles )
{
    //loop over conf file vector, find conf file using path resolver
    for( unsigned int confFile_itr = 0; confFile_itr < confFiles.size(); confFile_itr++ )
    {
        std::string tmp_confFile = confFiles.at(confFile_itr);
        confFiles.at(confFile_itr) = PathResolverFindCalibFile(confFiles.at(confFile_itr));
        if (confFiles.at(confFile_itr) == "")
        {
            ATH_MSG_ERROR("Could not locate configuration file " << tmp_confFile);
            return StatusCode::FAILURE;
        }
    }
    //everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionTool::initializeTools( const std::string& name, const std::vector<std::string>& confFiles, std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>>& toolHolder )
{
    // adapt size of toolHolder
    toolHolder.resize(confFiles.size());
    // for each conf file, initialize one tool
    for( unsigned int confFile_itr = 0; confFile_itr < confFiles.size(); confFile_itr++ )
    {
        // name: supertool name + type name + variable name
        std::string variable = ""; //get the name of the variable to be corrected
        ANA_CHECK(getCorrectionVariableName(variable, confFiles.at(confFile_itr)));
        TString toolname = TString::Format("%s_%s_%s", this->name().c_str(), name.c_str(), variable.c_str());
        ANA_MSG_DEBUG("Subtool name: " << toolname.Data());
        toolHolder.at(confFile_itr) = std::make_unique<ElectronPhotonVariableCorrectionBase>(toolname.Data());
        ANA_CHECK(toolHolder.at(confFile_itr)->setProperty("ConfigFile", confFiles.at(confFile_itr)));
        ANA_CHECK(toolHolder.at(confFile_itr)->initialize());
    }
    //everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionTool::applyToFlagMatchesToolHolder( const std::vector<std::string>& confFiles, const std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>>& toolHolder, ElectronPhotonVariableCorrectionBase::EGammaObjects toolHolderType )
{
    // loop over conf file holder
    for (unsigned int tool_itr = 0; tool_itr < toolHolder.size(); tool_itr++)
    {
        // get ApplyTo flag
        ElectronPhotonVariableCorrectionBase::EGammaObjects confFileType = toolHolder.at(tool_itr)->isAppliedTo();
        // skip all further tests if should be applied to all objects
        if (confFileType == ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects) continue;
        // continue if ApplyTo flag matches toolholder
        if (toolHolderType == ElectronPhotonVariableCorrectionBase::EGammaObjects::convertedPhotons && toolHolder.at(tool_itr)->applyToConvertedPhotons()) continue;
        if (toolHolderType == ElectronPhotonVariableCorrectionBase::EGammaObjects::unconvertedPhotons && toolHolder.at(tool_itr)->applyToUnconvertedPhotons()) continue;
        if (toolHolderType == ElectronPhotonVariableCorrectionBase::EGammaObjects::allElectrons && toolHolder.at(tool_itr)->applyToElectrons()) continue;
        // if this point is reached, something is wrong, so
        ATH_MSG_ERROR("In conf " << confFiles.at(tool_itr) << ": The ApplyTo flag does not match with the container type from the conf file.");
        return StatusCode::FAILURE;
    }
    //everything worked out, so
    return StatusCode::SUCCESS;
}

/* ========================================
 * Apply correction
 * ======================================== */

const CP::CorrectionCode ElectronPhotonVariableCorrectionTool::applyCorrection( xAOD::Photon& photon ) const
{
    bool isConvertedPhoton = xAOD::EgammaHelpers::isConvertedPhoton(&photon);

    // check if need to run over converted or unconverted photons
    if (isConvertedPhoton)
    {
        // correct variables on the converted photon
        for (unsigned int convertedPhotonTool_itr = 0; convertedPhotonTool_itr < m_convertedPhotonTools.size(); convertedPhotonTool_itr++)
        {
            if ((m_convertedPhotonTools.at(convertedPhotonTool_itr)->applyCorrection(photon)) != CP::CorrectionCode::Ok)
            {
                ATH_MSG_ERROR("Could not apply correction to converted photon object.");
                return CP::CorrectionCode::Error;
            }
        }
    }
    else
    {
        // correct variables on the converted photon
        for (unsigned int unconvertedPhotonTool_itr = 0; unconvertedPhotonTool_itr < m_unconvertedPhotonTools.size(); unconvertedPhotonTool_itr++)
        {
            if ((m_unconvertedPhotonTools.at(unconvertedPhotonTool_itr)->applyCorrection(photon)) != CP::CorrectionCode::Ok)
            {
                ATH_MSG_ERROR("Could not apply correction to unconverted photon object.");
                return CP::CorrectionCode::Error;
            }
        }
    }
    
    // everything worked out, so
    return CP::CorrectionCode::Ok;
}
const CP::CorrectionCode ElectronPhotonVariableCorrectionTool::applyCorrection( xAOD::Electron& electron ) const
{
    // correct variables on the electron
    for (unsigned int electronTool_itr = 0; electronTool_itr < m_electronTools.size(); electronTool_itr++)
    {
        if ((m_electronTools.at(electronTool_itr)->applyCorrection(electron)) != CP::CorrectionCode::Ok)
        {
            ATH_MSG_ERROR("Could not apply correction to electron object.");
            return CP::CorrectionCode::Error;
        }
    }
    // everything worked out, so
    return CP::CorrectionCode::Ok;
}

/* ========================================
 * Corrected Copy
 * ======================================== */

const CP::CorrectionCode ElectronPhotonVariableCorrectionTool::correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon ) const
{
    out_photon = new xAOD::Photon(in_photon);
    return applyCorrection(*out_photon);
}

const CP::CorrectionCode ElectronPhotonVariableCorrectionTool::correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron) const
{
    out_electron = new xAOD::Electron(in_electron);
    return applyCorrection(*out_electron);
}

/* ========================================
 * Helper functions
 * ======================================== */

const StatusCode ElectronPhotonVariableCorrectionTool::getCorrectionVariableName( std::string &variableName, const std::string& confFile ) const
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
        ATH_MSG_ERROR("In conf file " << confFile << ": Correction variable is empty or not in configuration file.");
        return StatusCode::FAILURE;
    }
    //everything worked out, so
    return StatusCode::SUCCESS;
}
