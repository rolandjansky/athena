/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class ElectronPhotonVariableCorrectionToolWrapper
   @brief Wrapper tool to correct electron and photon MC variables using the ElectronPhotonVariableCorrectionToolWrapper.

   @author Nils Gillwald (DESY) nils.gillwald@desy.de
   @date   February 2020
*/

#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionTool.h"
#include "TFile.h"
#include "PathResolver/PathResolver.h"
#include "TEnv.h"
#include "AsgTools/AsgTool.h"
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

// forward declarations
class TF1;
class TFile;
class TEnv;

// ===========================================================================
// Class ElectronPhotonVariableCorrectionToolWrapper
// ===========================================================================

class ElectronPhotonVariableCorrectionToolWrapper : public asg::AsgTool, virtual public IElectronPhotonVariableCorrectionTool
{

/// Declare the interface that the class provides
ASG_TOOL_CLASS(ElectronPhotonVariableCorrectionToolWrapper, IElectronPhotonVariableCorrectionTool)

public:
    ElectronPhotonVariableCorrectionToolWrapper( const std::string& myname);
    ~ElectronPhotonVariableCorrectionToolWrapper();

    StatusCode initialize() override;
    StatusCode finalize() override;

    const StatusCode applyCorrection( xAOD::Photon& photon ) override;
    const StatusCode applyCorrection( xAOD::Electron& electron ) override;
    const StatusCode correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon ) override;
    const StatusCode correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron) override;

private:
    std::string m_configFile;
    std::vector<std::string> m_convertedPhotonConfFiles;
    std::vector<std::string> m_unconvertedPhotonConfFiles;
    std::vector<std::string> m_electronConfFiles;
    std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionTool>> m_convertedPhotonTools;
    std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionTool>> m_unconvertedPhotonTools;
    std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionTool>> m_electronTools;
    const StatusCode InitializeCorrectionTools();
    const StatusCode InitializeTools( const std::string& name, const std::vector<std::string>& confFiles, std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionTool>>& toolHolder );
    const StatusCode GetCorrectionVariableName( std::string &variableName, const std::string& confFile ) const;
    const StatusCode FindAllConfigFiles( std::vector<std::string>& confFiles );
    const StatusCode ApplyToFlagMatchesToolHolder( const std::vector<std::string>& confFiles, const std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionTool>>& toolHolder, ElectronPhotonVariableCorrectionTool::EGammaObjects toolHolderType );

}; //end class ElectronPhotonVariableCorrectionToolWrapper