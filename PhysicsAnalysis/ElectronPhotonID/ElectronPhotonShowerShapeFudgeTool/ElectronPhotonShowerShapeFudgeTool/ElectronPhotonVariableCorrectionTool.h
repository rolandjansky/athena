/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ElectronPhotonVariableCorrectionTool_H
#define ElectronPhotonVariableCorrectionTool_H

/**
   @class ElectronPhotonVariableCorrectionTool
   @brief Tool to correct electron and photon MC variables.

   @author Nils Gillwald (DESY) nils.gillwald@desy.de
   @date   February 2020
*/

//ATLAS includes
#include "AsgTools/AsgTool.h"
#include "EgammaAnalysisInterfaces/IElectronPhotonShowerShapeFudgeTool.h"

//EDM includes
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

//Root includes
#include "TF1.h"

// forward declarations
class TGraph;
class TFile;
class TEnv;

// ===========================================================================
// Class ElectronPhotonVariableCorrectionTool
// ===========================================================================

class ElectronPhotonVariableCorrectionTool : public asg::AsgTool, virtual public IElectronPhotonShowerShapeFudgeTool
{
    /// Declare the interface that the class provides
    ASG_TOOL_CLASS(ElectronPhotonVariableCorrectionTool, IElectronPhotonShowerShapeFudgeTool)

public:
    ElectronPhotonVariableCorrectionTool(const std::string& myname);
    ~ElectronPhotonVariableCorrectionTool() {};

    /** Gaudi Service Interface method implementations */
    StatusCode initialize() override;

    const CP::CorrectionCode applyCorrection( xAOD::Photon& photon ) const override;
    const CP::CorrectionCode applyCorrection( xAOD::Electron& electron ) const override;
    const CP::CorrectionCode correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon ) const override;
    const CP::CorrectionCode correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron) const override;
    const std::string GetCorrectionVariable() { return m_correctionVariable; };

private:
    // In order to do faster comparisons, use enum and not string for type of function parameter
    enum ParameterType{
        Failure = 0,
        EtaDependentTGraph = 1,
        PtDependentTGraph,
        EtaBinned,
        PtBinned,
        EtaTimesPtBinned,
        EventDensity
    }; // end enum ParameterType
    std::string m_configFile; //store the name of the conf file
    std::string m_correctionVariable; //store the name of the variable to correct
    std::string m_correctionFunctionString; //function to use for the variable correction
    std::unique_ptr<TF1> m_correctionFunctionTF1; //the actual TF1 correction function
    int m_numberOfFunctionParameters; //number of parameters of the variable correction function
    std::vector<ParameterType> m_ParameterTypeVector; //map object properties to correction function parameter number
    std::vector<TGraph*> m_graphCopies; //if needed, safe a copy of the TGraph from the root file
    std::vector<std::vector<float>> m_binValues; //if needed, store a list of eta/pt dependent values
    std::vector<float> m_etaBins; //if needed, store a list of bin boundaries in eta
    std::vector<float> m_ptBins; //if needed, store a list of bin boundaries in pt
    bool m_convertedPhotonsOnly = false; //set to true if correction is only for converted photons
    bool m_unconvertedPhotonsOnly = false; //set to true if correction is only for unconverted photons
    bool m_retreivedEtaBinning = false; //check if already retreived eta binning
    bool m_retreivedPtBinning = false; //check if already retreived pt binning
    std::unique_ptr<SG::AuxElement::Accessor<float>> m_variableToCorrect; // accessor for the variable to be corrected
    std::unique_ptr<SG::AuxElement::Accessor<float>> m_originalVariable; // accessor to store the original value of the corrected variable
    ElectronPhotonVariableCorrectionTool::ParameterType StringToParameterType( const std::string& input ) const; //convert input string to a parameter function type
    const StatusCode PassedCorrectPhotonType(const xAOD::Photon& photon) const; //check if the correct type of photon was passed to the tool, if only (un)converted photons requested
    const StatusCode GetParameterInformationFromConf(TEnv& env, const int& parameter_number, const ElectronPhotonVariableCorrectionTool::ParameterType& type); //depending on parameter type, get the relevant information from conf file
    const StatusCode GetCorrectionParameters(std::vector<float>& properties, const float& pt, const float& absEta) const; //get the actual parameters entering the correction TF1
    const StatusCode Get1DBinnedParameter(float& return_parameter_value, const float& evalPoint, const std::vector<float>& binning, const int& parameter_number) const; //get the parameter if saved binned (eta or pt) in conf
    const StatusCode Get2DBinnedParameter(float& return_parameter_value, const float& etaEvalPoint, const float& ptEvalPoint, const int& parameter_number) const; //get the parameter if saved binned (eta and pt) in conf
    const StatusCode GetDensity(float& value, const std::string& eventShapeContainer) const; //get the density from the eventShape
    const StatusCode GetKinematicProperties(const xAOD::Egamma& egamma_object, float& pt, float& absEta) const; //get photon kinematic properties which change on single event basis
    const StatusCode Correct(float& return_corrected_variable, const float &original_variable, std::vector<float>& properties) const; //actual function applying the correction to the variable

}; //end class ElectronPhotonVariableCorrectionTool

#endif
