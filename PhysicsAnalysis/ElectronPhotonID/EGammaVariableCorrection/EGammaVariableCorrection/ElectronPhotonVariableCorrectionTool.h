/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ElectronPhotonVariableCorrectionTool_H
#define ElectronPhotonVariableCorrectionTool_H

/** @class ElectronPhotonVariableCorrectionTool
 * @brief Tool to correct electron and photon MC variables using the ElectronPhotonVariableCorrectionBase class.
 * @details For a detailed documentation, please refer to the [gitlab readme](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool/README.md)
 * 
 * @author Nils Gillwald (DESY) nils.gillwald@desy.de
 * @date   February 2020
*/

//ATLAS includes
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
#include "EGammaVariableCorrection/ElectronPhotonVariableCorrectionBase.h"

//EDM includes
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

// ===========================================================================
// Class ElectronPhotonVariableCorrectionTool
// ===========================================================================

class ElectronPhotonVariableCorrectionTool : public asg::AsgTool, virtual public IElectronPhotonShowerShapeFudgeTool
{

/// Declare the interface that the class provides
ASG_TOOL_CLASS(ElectronPhotonVariableCorrectionTool, IElectronPhotonShowerShapeFudgeTool)

public:
    /** @brief Standard constructor
     * @param myname Internal name of the tool instance, so they can be distinguished
     */ 
    ElectronPhotonVariableCorrectionTool(const std::string& myname);
    //! @brief Standard destructor
    ~ElectronPhotonVariableCorrectionTool() {};

    /** @brief Initialize the tool instance
     * @details Reads out the configuration file set via the setProperty(ConfigFile, "/path/to/conf/file.conf") function
     * and sets up the tool instance accordingly
     */
    virtual StatusCode initialize() override;

    /** @brief Apply the correction given in the conf file to the passed photon
     * @param photon The photon which should be corrected
     * @details Pass a photon which should be corrected by the tool to this function. The tool gets the original photon variable value,
     * corrects it according to the configuration file given, and overwrites the original value of the variable. The original variable
     * is saved as <variable_name>_original. Note that the variable to be corrected should be an auxiliary variable, and that the photon
     * or photon container, respectively, must not be const if you want to use this function. If you are running on const objects, please
     * make a deep / shallow copy of the container or use the correctedCopy function of this class.
     */
    virtual const CP::CorrectionCode applyCorrection( xAOD::Photon& photon ) const override;

    /** @brief Apply the correction given in the conf file to the passed electron
     * @param electron The electron which should be corrected
     * @details Pass an electron which should be corrected by the tool to this function. The tool gets the original electron variable value,
     * corrects it according to the configuration file given, and overwrites the original value of the variable. The original variable
     * is saved as <variable_name>_original. Note that the variable to be corrected should be an auxiliary variable, and that the electron
     * or electron container, respectively, must not be const if you want to use this function. If you are running on const objects, please
     * make a deep / shallow copy of the container or use the correctedCopy function of this class.
     */
    virtual const CP::CorrectionCode applyCorrection( xAOD::Electron& electron ) const override;

    /** @brief Make a corrected copy of the passed photon according to the given conf file
     * @param in_photon The original photon of which a corrected copy should be made
     * @param out_photon Empty new photon where the corrected copy will be stored in
     * @details Makes a corrected copy of the passed photon object according to the correction provided in the configuration file. The in_photon
     * is copied to the out_photon, which is then passed to the applyCorrection function.
     */
    virtual const CP::CorrectionCode correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon ) const override;
    
    /** @brief Make a corrected copy of the passed electron according to the given conf file
     * @param in_electron The original electron of which a corrected copy should be made
     * @param out_electron Empty new electron where the corrected copy will be stored in
     * @details Makes a corrected copy of the passed electron object according to the correction provided in the configuration file. The in_electron
     * is copied to the out_electron, which is then passed to the applyCorrection function.
     */
    virtual const CP::CorrectionCode correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron) const override;

private:
    //! @brief The configuration file for the tool
    std::string m_configFile;
    //! @brief The configuration files for the base class - converted photons
    std::vector<std::string> m_convertedPhotonConfFiles;
    //! @brief The configuration files for the base class - unconverted photons
    std::vector<std::string> m_unconvertedPhotonConfFiles;
    //! @brief The configuration files for the base class - electrons
    std::vector<std::string> m_electronConfFiles;
    //! @brief The base class instances for single variable correction - converted photons
    std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>> m_convertedPhotonTools;
    //! @brief The base class instances for single variable correction - unconverted photons
    std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>> m_unconvertedPhotonTools;
    //! @brief The base class instances for single variable correction - electrons
    std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>> m_electronTools;

    //! @brief Configure, initialize, and check the base class instances saved in the m_*Tools members
    const StatusCode initializeCorrectionTools();

    /** @brief Initialize the base clase instances saved in the m_*Tools members
     * @param name The name of the tool holder which should be included in the base class instances names
     * @param confFiles The conf files for the base class instances which should be created and initialized
     * @param toolHolder The tool holder where the base class instances are created
     */
    const StatusCode initializeTools( const std::string& name, const std::vector<std::string>& confFiles, std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>>& toolHolder );

    /** @brief Get current name of the variable to be corrected by the current base class instance, in order to coherently name the base class instances
     * @param variableName The current correction variable name is saved in this parameter
     * @param confFile The respective conf file of the current base class instance to be read-out
     */
    const StatusCode getCorrectionVariableName( std::string &variableName, const std::string& confFile ) const;

    /** @brief Locate all config files for the base class instances passed to the tool in m_configFile in the ATLAS file system
     * @param confFiles The conf files which shoud be used for the setup and initialization of the base class instances
     */
    const StatusCode findAllConfigFiles( std::vector<std::string>& confFiles );

    /** @brief Check if the ApplyTo flag from the base class conf file does match with the container type spcified in the tool conf file
     * @param confFiles The conf files used to initialze the base class instances now stored in the respective toolHolder
     * @param toolHolder The base class instances which were initialized using the respective confFiles
     * @param toolHolderType The type of base class instances which should be stored in the respective toolHolder with respect to the tespectif confFiles
     */
    const StatusCode applyToFlagMatchesToolHolder( const std::vector<std::string>& confFiles, const std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>>& toolHolder, ElectronPhotonVariableCorrectionBase::EGammaObjects toolHolderType );

}; //end class ElectronPhotonVariableCorrectionTool

#endif
