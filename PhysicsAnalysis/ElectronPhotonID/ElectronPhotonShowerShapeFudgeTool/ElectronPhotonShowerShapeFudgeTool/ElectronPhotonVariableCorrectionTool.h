/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ElectronPhotonVariableCorrectionTool_H
#define ElectronPhotonVariableCorrectionTool_H

/**
   @class ElectronPhotonVariableCorrectionTool
   @brief Tool to correct electron and photon MC variables using the ElectronPhotonVariableCorrectionBase class.

   @author Nils Gillwald (DESY) nils.gillwald@desy.de
   @date   February 2020
*/

//ATLAS includes
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"
#include "ElectronPhotonShowerShapeFudgeTool/ElectronPhotonVariableCorrectionBase.h"

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
    ElectronPhotonVariableCorrectionTool( const std::string& myname);
    ~ElectronPhotonVariableCorrectionTool() {};

    virtual StatusCode initialize() override;

    virtual const CP::CorrectionCode applyCorrection( xAOD::Photon& photon ) const override;
    virtual const CP::CorrectionCode applyCorrection( xAOD::Electron& electron ) const override;
    virtual const CP::CorrectionCode correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon ) const override;
    virtual const CP::CorrectionCode correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron) const override;

private:
    std::string m_configFile;
    std::vector<std::string> m_convertedPhotonConfFiles;
    std::vector<std::string> m_unconvertedPhotonConfFiles;
    std::vector<std::string> m_electronConfFiles;
    std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>> m_convertedPhotonTools;
    std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>> m_unconvertedPhotonTools;
    std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>> m_electronTools;
    const StatusCode initializeCorrectionTools();
    const StatusCode initializeTools( const std::string& name, const std::vector<std::string>& confFiles, std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>>& toolHolder );
    const StatusCode getCorrectionVariableName( std::string &variableName, const std::string& confFile ) const;
    const StatusCode findAllConfigFiles( std::vector<std::string>& confFiles );
    const StatusCode applyToFlagMatchesToolHolder( const std::vector<std::string>& confFiles, const std::vector<std::unique_ptr<ElectronPhotonVariableCorrectionBase>>& toolHolder, ElectronPhotonVariableCorrectionBase::EGammaObjects toolHolderType );

}; //end class ElectronPhotonVariableCorrectionTool

#endif
