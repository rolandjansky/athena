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
// Class ElectronPhotonVariableCorrectionTool
// ===========================================================================

class ElectronPhotonVariableCorrectionToolWrapper : public asg::AsgTool
{

public:
    ElectronPhotonVariableCorrectionToolWrapper( const std::string& myname);
    ~ElectronPhotonVariableCorrectionToolWrapper();

    StatusCode initialize();
    StatusCode finalize();

    const StatusCode applyCorrection( xAOD::Photon& photon );
    const StatusCode applyCorrection( xAOD::Electron& electron );
    const StatusCode correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon );
    const StatusCode correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron);

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

}; //end 