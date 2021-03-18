/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

/**
    @class ElectronPhotonVariableCorrectionBase
    @brief Tool to correct electron and photon MC variables.

    @author Nils Gillwald (DESY) nils.gillwald@desy.de
    @date   February 2020
**/

#include "EGammaVariableCorrection/ElectronPhotonVariableCorrectionBase.h"
#include "EgammaAnalysisHelpers/AsgEGammaConfigHelper.h"
#include "xAODEventShape/EventShape.h"

// EDM includes
#include "xAODEgamma/EgammaEnums.h"
#include "TEnv.h"

//Root includes
#include "TObject.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH2.h"
#include "TFile.h"

//allow advanced math for the TF1
#include "TMath.h"

#include "PathResolver/PathResolver.h"

// ===========================================================================
// Standard Constructor
// ===========================================================================
ElectronPhotonVariableCorrectionBase::ElectronPhotonVariableCorrectionBase(const std::string& myname) :
    AsgTool(myname)
{
    //declare the needed properties
    declareProperty("ConfigFile",m_configFile="","The configuration file to use");
}

// ===========================================================================
// Initialize
// ===========================================================================
StatusCode ElectronPhotonVariableCorrectionBase::initialize()
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
        ATH_MSG_DEBUG("Use configuration file " << m_configFile << " " << configFile);
    }
    else
    {
        ATH_MSG_ERROR("Config file string is empty. Please provide a config file to the tool.");
        return StatusCode::FAILURE;
    }

    // retrieve properties from configuration file, using TEnv class
    TEnv env(configFile.c_str());
    // Send warning if duplicates found in conf file
    env.IgnoreDuplicates(false);

    //retrieve variable to correct
    if (env.Lookup("Variable"))
    {
        m_correctionVariable = env.GetValue("Variable","");
    }
    else
    {
        ATH_MSG_ERROR("Correction variable is empty or not in configuration file.");
        return StatusCode::FAILURE;
    }

    // initialize the variable aux element accessors
    // variable to be corrected
    m_variableToCorrect = std::make_unique<SG::AuxElement::Accessor<float>>(m_correctionVariable);
    // save original value under different name
    m_originalVariable = std::make_unique<SG::AuxElement::Accessor<float>>(m_correctionVariable + "_original");

    // Get the function used to correct the variable
    if (env.Lookup("Function"))
    {
        m_correctionFunctionString = env.GetValue("Function","failure");
        // initialize the actual correction function TF1
        m_correctionFunctionTF1 = std::make_unique<TF1>(TF1("m_correctionFunctionTF1",m_correctionFunctionString.c_str()));
    }
    else
    {
        ATH_MSG_ERROR("Correction function is empty or not in configuration file.");
        return StatusCode::FAILURE;
    }

    // Get the number of parameters used in the correction function
    if (env.Lookup("nFunctionParameters"))
    {
        m_numberOfFunctionParameters = env.GetValue("nFunctionParameters",-1);
    }
    else
    {
        ATH_MSG_ERROR("You did not specify the number of parameters in the correction function.");
        return StatusCode::FAILURE;
    }

    // resize all vectors used for getting parameters to m_numberOfFunctionParameters
    m_ParameterTypeVector.resize(m_numberOfFunctionParameters);
    m_binValues.resize(m_numberOfFunctionParameters);
    m_graphCopies.resize(m_numberOfFunctionParameters);
    m_TH2Copies.resize(m_numberOfFunctionParameters);
    m_useAbsEtaTH2.resize(m_numberOfFunctionParameters);
    m_interpolatePtFlags.resize(m_numberOfFunctionParameters);

    // Save the type of all parameters in the correction function (assuming m_numberOfFunctionParameters parameters)
    for (int parameter_itr = 0; parameter_itr < m_numberOfFunctionParameters; parameter_itr++)
    {
        // if the parameter #parameter_itr is in the conf file, save its type
        TString parameterType = TString::Format("Parameter%dType",parameter_itr);
        if (env.Lookup(parameterType))
        {
            // convert string to ParameterType, fail if non-existing type
            ElectronPhotonVariableCorrectionBase::parameterType type = stringToParameterType(env.GetValue(parameterType.Data(),""));
            if( type == ElectronPhotonVariableCorrectionBase::parameterType::Failure )
            {
                ATH_MSG_ERROR("Parameter " << parameter_itr << " read-in failed, not an allowed parameter type.");
                return StatusCode::FAILURE;
            }
            // save type, get according type information and save it
            m_ParameterTypeVector.at(parameter_itr) = type;
            ATH_CHECK(getParameterInformationFromConf(env, parameter_itr, type));
        }
        // else fail
        else
        {
            ATH_MSG_ERROR("Did not find Parameter" << parameter_itr << ", although you specified there were " << m_numberOfFunctionParameters << " parameters for the correction function.");
            return StatusCode::FAILURE;
        }
    } // end loop over all function parameters

    // check to which EGamma object the conf file should be applied to, if flag is set
    if (env.Lookup("ApplyTo"))
    {
        std::string applyToObjectsFlag = env.GetValue("ApplyTo","Failure");
        m_applyToObjects = stringToEGammaObject(applyToObjectsFlag);
        // fail if not passed a proper type
        if (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::Failure)
        {
            ATH_MSG_ERROR("You did not correctly specify the object type in the ApplyTo flag.");
            return StatusCode::FAILURE;
        }
    }
    // else fail
    else
    {
        ATH_MSG_ERROR("You did not specify to which objects this conf file should be applied to (ApplyTo).");
        return StatusCode::FAILURE;
    }

    // check if there are any (discrete) values which should be left uncorrected
    if (env.Lookup("UncorrectedDiscontinuities"))
    {
        m_uncorrectedDiscontinuities = AsgConfigHelper::HelperFloat("UncorrectedDiscontinuities", env);
        // if flag is given, but no values, fail
        if (m_uncorrectedDiscontinuities.size() < 1)
        {
            ATH_MSG_ERROR("Did not find any discontinuities to not correct, despite finding the flag UncorrectedDiscontinuities.");
            return StatusCode::FAILURE;
        }
    }

    //everything worked out, so
    return StatusCode::SUCCESS;
}
 
// ===========================================================================
// Application of correction
// ===========================================================================
const CP::CorrectionCode ElectronPhotonVariableCorrectionBase::applyCorrection(xAOD::Photon& photon) const
{
    // check if we should only deal with converted / unconverted photons
    if (!passedCorrectPhotonType(photon))
    {
        ATH_MSG_ERROR("You specified in the conf file that the tool should only be used for (un-)converted photons, but passed the other conversion type.");
        return CP::CorrectionCode::Error;
    }
    
    // From the object, get the variable value according to the variable from the conf file
    // if variable not found, fail
    float original_variable = 0.;
    if( m_variableToCorrect->isAvailable(photon) )
    {
        original_variable = (*m_variableToCorrect)(photon);
        //Save the original value to the photon under different name
        (*m_originalVariable)(photon) = original_variable;
        // check if tool should skip correcting this variable, as it's from some discontinuity
        if (isEqualToUncorrectedDiscontinuity(original_variable))
        {
            return CP::CorrectionCode::Ok;
        }
    }
    else
    {
        ATH_MSG_ERROR("The correction variable \"" << m_correctionVariable << "\" provided in the conf file is not available.");
        return CP::CorrectionCode::Error;
    }
    
    //declare objects needed to retrieve photon properties
    std::vector<float> properties; //safe value of function parameter i at place i
    properties.resize(m_numberOfFunctionParameters);
    float eta; //safe value of eta of event
    float pt; //safe pt of event
    float phi; //safe phi of event

    //Get all the properties from the photon and fill them to properties, eta, pt, phi
    if (getKinematicProperties(photon, pt, eta, phi) != StatusCode::SUCCESS)
    {
        ATH_MSG_ERROR("Could not retrieve kinematic properties of this photon object.");
        return CP::CorrectionCode::Error;
    }
    ATH_MSG_VERBOSE("Got the photon kinematics , pT = " << pt << " eta = " << eta << "phi = " << phi);
    if (getCorrectionParameters(properties, pt, eta, phi) != StatusCode::SUCCESS)
    {
        ATH_MSG_ERROR("Could not get the correction parameters for this photon object.");
        return CP::CorrectionCode::Error;
    }
    for (auto p : properties)
        ATH_MSG_VERBOSE("prop " << p);

    // Apply the correction, write to the corrected AuxElement
    correct((*m_variableToCorrect)(photon),original_variable, properties).ignore(); // ignore as will always return SUCCESS

    // everything worked out, so
    return CP::CorrectionCode::Ok;
}

const CP::CorrectionCode ElectronPhotonVariableCorrectionBase::applyCorrection(xAOD::Electron& electron) const
{   
    if (!applyToElectrons())
    {
        ATH_MSG_ERROR("You want to correct electrons, but passed a conf file with ApplyTo flag not set for electrons. Are you using the correct conf file?");
        return CP::CorrectionCode::Error;
    }

    // From the object, get the variable value according to the variable from the conf file
    // if variable not found, fail
    float original_variable = 0.;
    if (m_variableToCorrect->isAvailable(electron))
    {
        original_variable = (*m_variableToCorrect)(electron);
        //Save the original value to the photon under different name
        (*m_originalVariable)(electron) = original_variable;
        // check if tool should skip correcting this variable, as it's from some discontinuity
        if (isEqualToUncorrectedDiscontinuity(original_variable))
        {
            return CP::CorrectionCode::Ok;
        }
    }
    else
    {
        ATH_MSG_ERROR("The correction variable \"" << m_correctionVariable << "\" provided in the conf file is not available.");
        return CP::CorrectionCode::Error;
    }

    //declare objects needed to retrieve electron properties
    std::vector<float> properties; //safe value of function parameter i at place i
    properties.resize(m_numberOfFunctionParameters);
    float eta; //safe value of eta of event
    float pt; //safe pt of event
    float phi; //safe phi of event

    //Get all the properties from the electron and fill them to properties, eta, pt, phi
    if (getKinematicProperties(electron, pt, eta, phi) != StatusCode::SUCCESS)
    {
        ATH_MSG_ERROR("Could not retrieve kinematic properties of this electron object.");
        return CP::CorrectionCode::Error;
    }
    ATH_MSG_VERBOSE("Got the electron kinematics , pT = " << pt << " eta = " << eta << "phi = " << phi);
    if (getCorrectionParameters(properties, pt, eta, phi) != StatusCode::SUCCESS)
    {
        ATH_MSG_ERROR("Could not get the correction parameters for this electron object.");
        return CP::CorrectionCode::Error;
    }

    for (auto p : properties)
        ATH_MSG_VERBOSE("prop " << p);

    // Apply the correction, write to the corrected AuxElement
    correct((*m_variableToCorrect)(electron),original_variable, properties).ignore(); // ignore as will always return SUCCESS

    // everything worked out, so
    return CP::CorrectionCode::Ok;
}

const StatusCode ElectronPhotonVariableCorrectionBase::correct(float& return_corrected_variable, const float original_variable, std::vector<float>& properties) const
{   
    // set the parameters of the correction function
    for (unsigned int parameter_itr = 0; parameter_itr < properties.size(); parameter_itr++)
    {
        m_correctionFunctionTF1->SetParameter(parameter_itr,properties.at(parameter_itr));
    }

    ATH_MSG_VERBOSE("original var " << original_variable);

    // Calculate corrected value
    return_corrected_variable = m_correctionFunctionTF1->Eval(original_variable);

    ATH_MSG_VERBOSE("corrected var " << return_corrected_variable);

    // everything worked out, so
    return StatusCode::SUCCESS;
}

// ===========================================================================
// Corrected Copies
// ===========================================================================
const CP::CorrectionCode ElectronPhotonVariableCorrectionBase::correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon ) const
{
    out_photon = new xAOD::Photon(in_photon);
    return applyCorrection(*out_photon);
}

const CP::CorrectionCode ElectronPhotonVariableCorrectionBase::correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron) const
{
    out_electron = new xAOD::Electron(in_electron);
    return applyCorrection(*out_electron);
}

// ===========================================================================
// Helper Functions
// ===========================================================================

bool ElectronPhotonVariableCorrectionBase::isEqualToUncorrectedDiscontinuity(const float value) const
{
    // if no values set, return false as there is nothing to check
    if (m_uncorrectedDiscontinuities.size() < 1)
    {
        return false;
    }

    // check all discontinuities which where passed
    for (unsigned int value_itr = 0; value_itr < m_uncorrectedDiscontinuities.size(); value_itr++)
    {
        if (value == m_uncorrectedDiscontinuities.at(value_itr))
        {
            // if the value is equal to one of the discontinuities, no need to check further
            return true;
        }
    }
    // if we eer get here, the value was never equal to a discontinuity
    return false;
}

const StatusCode ElectronPhotonVariableCorrectionBase::getKinematicProperties(const xAOD::Egamma& egamma_object, float& pt, float& eta, float& phi) const
{
    // just reteriving eta and pt is probably less expensive then checking if I need it and
    // then retrieve it only if I actually need it

    // protection against bad clusters
    const xAOD::CaloCluster* cluster  = egamma_object.caloCluster();
    if ( cluster == nullptr )
    {
        ATH_MSG_ERROR("EGamma object calorimeter cluster is NULL: Cluster " << cluster);
        return StatusCode::FAILURE;
    }

    // Fill variables
    // eta position in second sampling
    eta   = cluster->etaBE(2);
    // transverse energy in calorimeter (using eta position in second sampling)
    const double energy =  cluster->e();
    double et = 0.;
    if (eta<999.) {
        const double cosheta = cosh(eta);
        et = (cosheta != 0.) ? energy/cosheta : 0.;
    }
    pt = et;
    // phi
    phi = cluster->phiBE(2);

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::getParameterInformationFromConf(TEnv& env, const int parameter_number, const ElectronPhotonVariableCorrectionBase::parameterType type)
{
    // don't want to write the same code multiple times, so set flags when to retrieve eta/pt bins
    bool getEtaBins = false;
    bool getPtBins = false;
    // form strings according to which parameter to retrieve
    TString filePathKey = TString::Format("Parameter%dFile",parameter_number);
    TString graphNameKey = TString::Format("Parameter%dGraphName",parameter_number);
    TString histNameKey = TString::Format("Parameter%dTH2Name",parameter_number);
    TString binValues = TString::Format("Parameter%dValues",parameter_number);
    TString interpolate = TString::Format("Parameter%dInterpolate",parameter_number);

    // according to the parameter type, retrieve the information from conf
    switch (type)
    {
        case ElectronPhotonVariableCorrectionBase::parameterType::EtaDependentTGraph:
            // this fallthrough is intentional!
        case ElectronPhotonVariableCorrectionBase::parameterType::PtDependentTGraph:
        { // need to mark scope, since variables are initialized in this case
            std::unique_ptr<TObject> graph;
            ATH_CHECK(getObjectFromRootFile(env, parameter_number, filePathKey, graphNameKey, graph));
            m_graphCopies.at(parameter_number) = (TGraph*)graph.get();
        }
            break;
        case ElectronPhotonVariableCorrectionBase::parameterType::EtaBinned:
            //get eta binning later
            getEtaBins = true;
            break;
        case ElectronPhotonVariableCorrectionBase::parameterType::PtBinned:
            //get pt binning later
            getPtBins = true;
            break;
        case ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPtBinned:
            //get eta and pt binning later
            getEtaBins = true;
            getPtBins = true;
            break;
        case ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPhiTH2:
        { // need to mark scope, since variables are initialized in this case
            // Retreive TH2F steering eta x phi corrections
            std::unique_ptr<TObject> th2;
            ATH_CHECK(getObjectFromRootFile(env, parameter_number, filePathKey, histNameKey, th2));
            m_TH2Copies.at(parameter_number) = (TH2*)th2.get();
            // check and store if this TH2 needs eta or abs(eta) for evaluation
            // for this, check if lowest bin boundary < 0
            // bin 0 is the undeflow bin, so use bin 1
            float lowest_bin_boundary = ((TH2*)th2.get())->GetXaxis()->GetBinLowEdge(1);
            // the lowest boundary should never be greater than 0! Fail if it is
            if (lowest_bin_boundary > 0)
            {
                ATH_MSG_ERROR("Lowest bin edge in TH2 for parameter " << parameter_number << " is > 0. Please provide the TH2 including corrections either for the positive eta range (starting at 0), or the whole eta range (starting with a negative dummy value which is treated as -infinity.");
                return StatusCode::FAILURE;
            }
            else
            {
                // use eta for evaluation of this TH2 if corrections for eta < 0 are in the TH2
                // store the actual value so it can be used in the TH2 parameter check
                // (need to make sure the object eta is not smaller than the smallest TH2 bin boundary)
                m_useAbsEtaTH2.at(parameter_number) = lowest_bin_boundary;
            }
        }
            break;
        case ElectronPhotonVariableCorrectionBase::parameterType::EventDensity:
            // nothing has to be retrieved, no additional parameters for EventDensity currently
            return StatusCode::SUCCESS;
        default:
        {}//only adding default to omit compile time warnings for not including parameterType::Failure
            // this case will never occur, since returning this case fails earlier
    }

    //get the pt and eta binning from the conf file, if necessary
    if (getEtaBins || getPtBins)
    { ATH_CHECK(getEtaPtBinningsFromConf(getEtaBins, getPtBins, binValues, interpolate, env, parameter_number)); }

    //everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::getEtaPtBinningsFromConf(const bool getEtaBins, const bool getPtBins, const TString& binValues, const TString& interpolate, TEnv& env, const int parameter_number)
{
    // if needed and not already retrieved, get eta binning
    if (getEtaBins && !m_retrievedEtaBinning)
    {
        // check if necessary information is in conf, else fail
        if (env.Lookup("EtaBins"))
        {
            //get the eta binning (global!)
            m_etaBins = AsgConfigHelper::HelperFloat("EtaBins",env);
            //force that the low bin edges are given by the conf file, starting with 0 or a negative value
            if (m_etaBins.at(0) > 0.)
            {
                ATH_MSG_ERROR("Lowest bin edge given for parameter " << parameter_number << " is > 0. Please provide the lower bin edges of your correction binning in the conf file, starting with either 0 (for corrections symmetric in eta) or a negative number (being treated as -infinity).");
                return StatusCode::FAILURE;
            }
            else
            {
                m_useAbsEtaBinned = m_etaBins.at(0) < 0 ? false : true;
            }
            // don't want to retrieve the same thing twice from conf
            m_retrievedEtaBinning = true;
        }
        else
        {
            ATH_MSG_ERROR("Could not retrieve eta binning.");
            return StatusCode::FAILURE;
        }
    }
    // if needed and not already retrieved, get pt binning
    if (getPtBins && !m_retrievedPtBinning)
    {
        // check if necessary information is in conf, else fail
        if (env.Lookup("PtBins"))
        {
            //get the pt binning (global!)
            m_ptBins = AsgConfigHelper::HelperFloat("PtBins",env);
            //force that the low bin edges are given by the conf file, starting with 0
            if (m_ptBins.at(0) != 0.)
            {
                ATH_MSG_ERROR("Lowest bin edge given for parameter " << parameter_number << " is not 0. Please provide the lower bin edges of your correction binning in the conf file, starting with 0.");
                return StatusCode::FAILURE;
            }
            // don't want to retrieve the same thing twice from conf
            m_retrievedPtBinning = true;
        }
        else
        {
            ATH_MSG_ERROR("Could not retrieve pt binning.");
            return StatusCode::FAILURE;
        }
    }
    //check if interpolation should be done
    if (getPtBins)
    {
        // default is false
        m_interpolatePtFlags.at(parameter_number) = env.GetValue(interpolate.Data(),false);
    }
    if ( getEtaBins || getPtBins)
    {
        // check if necessary information is in conf, else fail
        if (env.Lookup(binValues))
        {
            //get the binned values of the eta/pt dependent parameter
            m_binValues.at(parameter_number) = AsgConfigHelper::HelperFloat(binValues.Data(),env);
        }
        else
        {
            ATH_MSG_ERROR("Could not retrieve binned values.");
            return StatusCode::FAILURE;
        }
    }

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::getObjectFromRootFile(TEnv& env, const int parameter_number, const TString& filePathKey, const TString& nameKey, std::unique_ptr<TObject>& return_object)
{
    // helpers
    TString filePath = "";
    TString objectName = "";
    // check if necessary information is in conf, else fail
    if (env.Lookup(filePathKey))
    {
        //get the path to the root file where the graph is saved
        filePath = PathResolverFindCalibFile(env.GetValue(filePathKey.Data(),""));
        // fail if file not found
        if (filePath == "")
        {
            ATH_MSG_ERROR("Could not locate Parameter" << parameter_number << " TObject file.");
            return StatusCode::FAILURE;
        }
    }
    else
    {
        ATH_MSG_ERROR("Could not retrieve Parameter" << parameter_number << " file path.");
        return StatusCode::FAILURE;
    }
    // check if necessary information is in conf, else fail
    if (env.Lookup(nameKey))
    {
        //get the name of the TGraph
        objectName = env.GetValue(nameKey.Data(),"");
    }
    else
    {
        ATH_MSG_ERROR("Could not retrieve Parameter" << parameter_number << " object name.");
        return StatusCode::FAILURE;
    }
    // open file, if it works, try to find object, get object, store a copy, else warning + fail
    std::unique_ptr<TFile> file (new TFile(filePath.Data(),"READ"));
    // check if file is open - if open, get graph, if not, fail
    if (file->IsOpen())
    {
        // if object exists, get it, else fail
        if (file->Get(objectName))
        {
            return_object = std::unique_ptr<TObject> (file->Get(objectName.Data())->Clone());
            // need to un-associate THx type objects from file directory, so they remain accessible
            if (dynamic_cast<TH1*>(return_object.get()) != nullptr)
            {
                dynamic_cast<TH1*>(return_object.get())->SetDirectory(0);
            }
            file->Close();
        }
        else
        {
            ATH_MSG_ERROR("Could not find TObject " << objectName << " in file " << filePath);
            return StatusCode::FAILURE;
        }
    }
    else
    {
        ATH_MSG_ERROR("Could not open Parameter" << parameter_number << " TObject file " << filePath.Data());
        return StatusCode::FAILURE;
    }

    // everything went fine, so
    return StatusCode::SUCCESS;
}


const StatusCode ElectronPhotonVariableCorrectionBase::getCorrectionParameters(std::vector<float>& properties, const float pt, const float eta, const float phi) const
{
    // check if eta or abs(eta) is used for the binned variables
    float etaForBinned = eta;
    if (m_useAbsEtaBinned)
    { etaForBinned = std::abs(eta); }
    // if use eta, correct for cases where eta is smaller than lowest bin boundary
    // i.e. treat lowest bin boundary as -inf!
    else if (etaForBinned < m_etaBins.at(0))
    {
        // set eta to midpoint of lowest bin
        etaForBinned = 0.5*(m_etaBins.at(0) + m_etaBins.at(1));
    }
    
    // according to the parameter type, get the actual parameter going to the correction function
    // for this, loop over the parameter type vector
    for (unsigned int parameter_itr = 0; parameter_itr < m_ParameterTypeVector.size(); parameter_itr++)
    {
        switch (m_ParameterTypeVector.at(parameter_itr))
        {
            case ElectronPhotonVariableCorrectionBase::parameterType::EtaDependentTGraph:
                // evaluate TGraph at abs(eta)
                properties.at(parameter_itr) = m_graphCopies.at(parameter_itr)->Eval(etaForBinned);
                break;
            case ElectronPhotonVariableCorrectionBase::parameterType::PtDependentTGraph:
                // evaluate TGraph at pt
                properties.at(parameter_itr) = m_graphCopies.at(parameter_itr)->Eval(pt);
                break;
            case ElectronPhotonVariableCorrectionBase::parameterType::EtaBinned:
                // get value of correct eta bin
                ATH_CHECK(get1DBinnedParameter(properties.at(parameter_itr),etaForBinned,m_etaBins,parameter_itr));
                break;
            case ElectronPhotonVariableCorrectionBase::parameterType::PtBinned:
                // get value of correct pt bin
                ATH_CHECK(get1DBinnedParameter(properties.at(parameter_itr),pt,m_ptBins,parameter_itr));
                break;
            case ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPtBinned:
                // get value of correct eta x pt bin
                ATH_CHECK(get2DBinnedParameter(properties.at(parameter_itr),etaForBinned,pt,parameter_itr));
                break;
            case ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPhiTH2:
            { // need curly brackets here to mark scope - since etaForThisTH2 is initialized (and only needed) here
                // check if need to use eta or abseta for this parameter (depends on the respective TH2)
                float etaForThisTH2 = eta;
                if (m_useAbsEtaTH2.at(parameter_itr) == 0.)
                { etaForThisTH2 = std::abs(eta); }
                // get value of correct eta x phi bin
                ATH_CHECK(get2DHistParameter(properties.at(parameter_itr),etaForThisTH2,phi,parameter_itr));
            }
                break;
            case ElectronPhotonVariableCorrectionBase::parameterType::EventDensity:
                // get event density
                ATH_CHECK(getDensity(properties.at(parameter_itr), "TopoClusterIsoCentralEventShape"));
                break;
            default:
            {}//only adding default to omit compile time warnings for not including parameterType::Failure
                // this case will never occur, since returning this case fails earlier
        }
    }

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::get1DBinnedParameter(float& return_parameter_value, const float evalPoint, const std::vector<float>& binning, const int parameter_number) const
{
    ANA_MSG_VERBOSE("EvalPoint: " << evalPoint);
    // need to find the bin in which the evalPoint is
    int bin = -1;
    ATH_CHECK(findBin(bin, evalPoint, binning));

    // calculate return value
    // if interpolation flag is true, interpolate
    if (getInterpolationFlag(parameter_number))
    {
        ATH_CHECK(interpolate(return_parameter_value, evalPoint, bin, binning, m_binValues.at(parameter_number)));
    }
    else
    {
        return_parameter_value = m_binValues.at(parameter_number).at(bin);
    }

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::get2DBinnedParameter(float& return_parameter_value, const float etaEvalPoint, const float ptEvalPoint, const int parameter_number) const
{
    //need to find eta bin, and need to find pt bin
    //from this, calculate which parameter of the list is needed to be returned.
    int etaBin = -1;
    int ptBin = -1;

    ATH_MSG_VERBOSE("eta = " << etaEvalPoint << " pt = " << ptEvalPoint);

    ATH_CHECK(findBin(etaBin, etaEvalPoint, m_etaBins));
    ATH_CHECK(findBin(ptBin, ptEvalPoint, m_ptBins));

    // get the corresponding pt x eta bin found
    /* Note: Assuming that the values are binned in pt x eta in the conf file:
     *           eta bin 0 | eta bin 1 | eta bin 2 | eta bin 3 | eta bin 4 | etc.
     * pt bin 0      0           1           2           3           4
     * pt bin 1      5           6           7           8           9
     * pt bin 2     10          11          12          13          14
     * pt bin 3     15          16          17          18          19
     * pt bin 4     20          21          22          23          24
     * etc.
     * the correct parameter is saved in the vector at (number of eta bins) * ptBinNumber + etaBinNumber
     * */

    ATH_MSG_VERBOSE(" eta bin " << etaBin << " pT bin " << ptBin);

    int bin_number_in_bin_values = m_etaBins.size() * ptBin + etaBin;

    // calculate return value
    // if interpolation flag is true, interpolate
    if (getInterpolationFlag(parameter_number))
    {
        // create the vector of correction values binned in pT at the found eta bin
        std::vector<float> tmp_binValuesAtEtaSlice;
        // from the full binning vector, need to cut one of the columns
        for (unsigned int binValue_itr = etaBin; binValue_itr < m_binValues.at(parameter_number).size(); binValue_itr += m_etaBins.size())
        {
            tmp_binValuesAtEtaSlice.push_back(m_binValues.at(parameter_number).at(binValue_itr));
        }
        // interpolate using the above slice of the correction values
        ATH_CHECK(interpolate(return_parameter_value, ptEvalPoint, ptBin, m_ptBins, tmp_binValuesAtEtaSlice));
    }
    else
    {
        return_parameter_value = m_binValues.at(parameter_number).at(bin_number_in_bin_values);
    }

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::get2DHistParameter(float& return_parameter_value, const float etaEvalPoint, const float phiEvalPoint, const int parameter_number) const
{
    // might need to update eta evaluation point and can't update const float
    float this_etaEvalPoint = etaEvalPoint;
    ATH_MSG_VERBOSE("eta = " << this_etaEvalPoint << " phi = " << phiEvalPoint);
    // make sure the eta eval point is inside the bounds of the histogram (i.e. pretend the lowest bin boundary is -infinity)
    if (this_etaEvalPoint < m_useAbsEtaTH2.at(parameter_number))
    {
        this_etaEvalPoint = m_TH2Copies.at(parameter_number)->GetXaxis()->GetBinCenter(1);
        ATH_MSG_VERBOSE("eta was out of bounds of TH2, corrected to eta = " << this_etaEvalPoint);
    }
    // get the value of the TH2 at the corresponding eta x phi pair
    const int bin_number = m_TH2Copies.at(parameter_number)->FindBin(this_etaEvalPoint, phiEvalPoint);
    return_parameter_value = m_TH2Copies.at(parameter_number)->GetBinContent(bin_number);
    ATH_MSG_VERBOSE("Correction factor:" << return_parameter_value);

    // everything went fine, so
    return StatusCode::SUCCESS;
}


const StatusCode ElectronPhotonVariableCorrectionBase::findBin(int& return_bin, const float evalPoint, const std::vector<float>& binning) const
{
    // need to find the bin in which the evalPoint is
    return_bin = -1;
    // if the evalPoint is < 0, something is very wrong
    if (evalPoint < binning.at(0))
    {
        // I use it for eta and pT, so error message is tailored to that...
        ATH_MSG_ERROR("Abs(Eta) or pT of object is smaller than 0.");
        return StatusCode::FAILURE;
    }
    // loop over bin boundaries
    //run only up to binning.size()-1, as running to binning.size() will get a seg fault for the boundary check
    for (unsigned int bin_itr = 0; bin_itr < binning.size()-1; bin_itr++)
    {
        // if the evaluation point is within the checked bins boundaries, this is the value we want
        if (evalPoint > binning.at(bin_itr) && evalPoint < binning.at(bin_itr + 1))
        {
            // we found the according bin, so return the according value
            return_bin = bin_itr;
            // we can stop now
            break;
        }
    }
    //if this point is ever reached and bin == -1, the evaluation point is larger then the largest lowest bin edge
    //if that's the case, return the value for the last bin
    //the -1 is because the parameter numbering in a vector starts at 0
    if (return_bin == -1)
    {
        return_bin = m_binValues.size()-1;
    }

    // everythin went fine, so
    return StatusCode::SUCCESS;
}

bool ElectronPhotonVariableCorrectionBase::getInterpolationFlag(const int parameter_number) const
{
    bool do_interpolation = false;
    // get parameter number type
    ElectronPhotonVariableCorrectionBase::parameterType type = m_ParameterTypeVector.at(parameter_number);
    // check if parameter has a type for which we need to check if interpolation is wanted
    if (type == ElectronPhotonVariableCorrectionBase::parameterType::PtBinned || type == ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPtBinned)
    {
        // get the flag
        do_interpolation = m_interpolatePtFlags.at(parameter_number);
    }
    return do_interpolation;
}

const StatusCode ElectronPhotonVariableCorrectionBase::interpolate(float& return_parameter_value, const float evalPoint, const int bin, const std::vector<float>& binning, const std::vector<float>& binValues) const
{
    // check if passed binning is consistent
    if (binning.size() != binValues.size())
    {
        ATH_MSG_ERROR("Binning and bin values have different sizes.");
        return StatusCode::FAILURE;
    }

    // if evalPoint is left to the leftmost bin center, return the leftmost bin center without interpolation
    float leftmost_bin_center = 0;
    ANA_CHECK(getBinCenter(leftmost_bin_center, binning, 0));
    if (evalPoint <= leftmost_bin_center)
    {
        return_parameter_value = binValues.at(0);
        return StatusCode::SUCCESS;
    }

    // if evalPoint is right to the rightmost bin center, return the rightmost bin center without interpolation
    float rightmost_bin_center = 0;
    ANA_CHECK(getBinCenter(rightmost_bin_center, binning, binning.size()-1))
    if (evalPoint >= rightmost_bin_center)
    {
        return_parameter_value = binValues.at(binValues.size()-1);
        return StatusCode::SUCCESS;
    }

    float left_bin_center = 0;
    float right_bin_center = 0;
    float left_bin_value = 0;
    float right_bin_value = 0;
    float current_bin_center = 0;
    ANA_CHECK(getBinCenter(current_bin_center, binning, bin));

    // else interpolate using next left or next right bin
    if (evalPoint <= current_bin_center)
    {
        //interpolate left
        ANA_CHECK(getBinCenter(left_bin_center, binning, bin-1));
        ANA_CHECK(getBinCenter(right_bin_center, binning, bin));
        left_bin_value = binValues.at(bin-1);
        right_bin_value = binValues.at(bin);
    }
    else // evalPoint is right from bin center
    {
        //interpolate right
        ANA_CHECK(getBinCenter(left_bin_center, binning, bin));
        ANA_CHECK(getBinCenter(right_bin_center, binning, bin+1));
        left_bin_value = binValues.at(bin);
        right_bin_value = binValues.at(bin+1);
    }
    ATH_MSG_VERBOSE("bin centers : " << left_bin_center << " " << right_bin_center << " current : " << current_bin_center << " values : " << left_bin_value << " " << right_bin_value);

    // calculate return value
    return_parameter_value = interpolate_function(evalPoint, left_bin_center, left_bin_value, right_bin_center, right_bin_value);

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode ElectronPhotonVariableCorrectionBase::getBinCenter(float& return_bin_center, const std::vector<float>& binning, const int bin_int) const
{
    if (bin_int < 0)
    {
        ATH_MSG_ERROR("Bin number must be a non-negative integer.");
        return StatusCode::FAILURE;
    }

    // implicitly convert to loong unsigend int for comparisons, to get rid of compiler warnings resulting from comparisons of int and unsigned int
    long unsigned int bin = bin_int;

    if (bin >= binning.size())
    {
        ATH_MSG_ERROR("The requested bin is out of range of the passed binning.");
        return StatusCode::FAILURE;
    }

    // need special treatment for rightmost bin center:
    // it goes up to infinity...assume it's as broad as the
    // next to the rightmost bin for the interpolation
    if (bin == binning.size()-1)
    {
        //calculate the width of the next to rightmost bin
        float bin_width = binning.at(bin) - binning.at(bin-1);
        return_bin_center = binning.at(bin) + 0.5 * bin_width;
    }
    // normal calculation
    else
    {
        return_bin_center = 0.5 * (binning.at(bin) + binning.at(bin+1));
    }

    //everything went fine, so
    return StatusCode::SUCCESS;
}

float ElectronPhotonVariableCorrectionBase::interpolate_function(const float value, const float left_bin_center, const float left_bin_value, const float right_bin_center, const float right_bin_value) const
{
    return left_bin_value + (value - left_bin_center) * (right_bin_value - left_bin_value) / (right_bin_center - left_bin_center);
}

const StatusCode ElectronPhotonVariableCorrectionBase::getDensity(float& value, const std::string& eventShapeContainer) const
{
    // retrieve the event shape container
    const xAOD::EventShape* evtShape = nullptr;
    if(evtStore()->retrieve(evtShape, eventShapeContainer).isFailure()){
        ATH_MSG_ERROR("Cannot retrieve density container " << eventShapeContainer);
        return StatusCode::FAILURE;
    }
    // get the density from the container
    value = evtShape->getDensity(xAOD::EventShape::Density);
    return StatusCode::SUCCESS;
}

ElectronPhotonVariableCorrectionBase::parameterType ElectronPhotonVariableCorrectionBase::stringToParameterType( const std::string& input ) const
{
    // return parameter type according to string given in conf file
    if( input == "EtaDependentTGraph")
    { return ElectronPhotonVariableCorrectionBase::parameterType::EtaDependentTGraph; }
    else if( input == "PtDependentTGraph")
    { return ElectronPhotonVariableCorrectionBase::parameterType::PtDependentTGraph; }
    else if( input == "EtaBinned")
    { return ElectronPhotonVariableCorrectionBase::parameterType::EtaBinned; }
    else if( input == "PtBinned")
    { return ElectronPhotonVariableCorrectionBase::parameterType::PtBinned; }
    else if( input == "EtaTimesPtBinned")
    { return ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPtBinned; }
    else if( input == "EtaTimesPhiTH2")
    { return ElectronPhotonVariableCorrectionBase::parameterType::EtaTimesPhiTH2; }
    else if( input == "EventDensity")
    { return ElectronPhotonVariableCorrectionBase::parameterType::EventDensity; }
    else
    {
        // if not a proper type, return failure type - check and fail on this!
        ATH_MSG_ERROR(input.c_str() << " is not an allowed parameter type.");
        return ElectronPhotonVariableCorrectionBase::parameterType::Failure;
    }
}

ElectronPhotonVariableCorrectionBase::EGammaObjects ElectronPhotonVariableCorrectionBase::stringToEGammaObject( const std::string& input ) const
{
    // return object type which correction should be applied to
    if( input == "unconvertedPhotons" )
    { return ElectronPhotonVariableCorrectionBase::EGammaObjects::unconvertedPhotons; }
    else if( input == "convertedPhotons" )
    { return ElectronPhotonVariableCorrectionBase::EGammaObjects::convertedPhotons; }
    else if( input == "allPhotons" )
    { return ElectronPhotonVariableCorrectionBase::EGammaObjects::allPhotons; }
    else if( input == "allElectrons" )
    { return ElectronPhotonVariableCorrectionBase::EGammaObjects::allElectrons; }
    else if( input == "allEGammaObjects" )
    { return ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects; }
    else 
    {
        // if not a proper object type, return failure type - check and fail on this!
        ATH_MSG_ERROR(input.c_str() << " is not an allowed EGamma object type to apply corrections to.");
        return ElectronPhotonVariableCorrectionBase::EGammaObjects::Failure;
    }
}

bool ElectronPhotonVariableCorrectionBase::passedCorrectPhotonType(const xAOD::Photon& photon) const
{
    // retrieve if photon is converted or unconverted
    bool isConvertedPhoton = xAOD::EgammaHelpers::isConvertedPhoton(&photon);
    bool isUnconvertedPhoton = !isConvertedPhoton;

    // check if conf file ApplyTo flag matches photon conversion type
    return ((applyToConvertedPhotons() && isConvertedPhoton) || (applyToUnconvertedPhotons() && isUnconvertedPhoton));
}

bool ElectronPhotonVariableCorrectionBase::applyToConvertedPhotons() const
{
    bool applyToAllEGamma = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects);
    bool applyToAllPhotons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allPhotons);
    bool applyToConvertedPhotons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::convertedPhotons);
    return (applyToAllEGamma || applyToAllPhotons || applyToConvertedPhotons);
}

bool ElectronPhotonVariableCorrectionBase::applyToUnconvertedPhotons() const
{
    bool applyToAllEGamma = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects);
    bool applyToAllPhotons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allPhotons);
    bool applyToUnconvertedPhotons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::unconvertedPhotons);
    return (applyToAllEGamma || applyToAllPhotons || applyToUnconvertedPhotons);
}

bool ElectronPhotonVariableCorrectionBase::applyToElectrons() const
{
    bool applyToAllEGamma = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allEGammaObjects);
    bool applyToAllElectrons = (m_applyToObjects == ElectronPhotonVariableCorrectionBase::EGammaObjects::allElectrons);
    return (applyToAllEGamma || applyToAllElectrons);
}
