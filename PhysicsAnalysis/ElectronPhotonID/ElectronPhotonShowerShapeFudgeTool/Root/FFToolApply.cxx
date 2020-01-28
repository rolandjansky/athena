/**
   @class FFToolApply
   @brief Tool to correct photon and electron MC shower shapes.

   @author Nils Gillwald (DESY) nils.gillwald@desy.de
   @date   September 2019
*/

#include "AsgElectronPhotonCorrectionConfigHelper.h"
#include "ElectronPhotonShowerShapeFudgeTool/FFToolApply.h"
#include "xAODEventShape/EventShape.h"

// EDM includes
#include "xAODEgamma/EgammaEnums.h"
#include "TEnv.h"

//Root includes
#include "TF1.h"
#include "TGraph.h"
#include "TFile.h"

//allow advanced math for the TF1
#include "TMath.h"

#include "PathResolver/PathResolver.h"

// ===========================================================================
// Standard Constructor
// ===========================================================================
FFToolApply::FFToolApply(const std::string& myname) :
    AsgTool(myname)
{
    //declare the needed properties
    declareProperty("ConfigFile",m_configFile="","The configuration file to use");
}

// ===========================================================================
// Standard Destructor
// ===========================================================================
FFToolApply::~FFToolApply()
{
    //check status code of finalize
    if(finalize().isFailure())
    {
        ATH_MSG_ERROR( "Failure in FFToolApply finalize()");
    }
}

// ===========================================================================
// Initialize and Finalize
// ===========================================================================
StatusCode FFToolApply::finalize()
{
    return StatusCode::SUCCESS;
}

StatusCode FFToolApply::initialize()
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

    //retreive variable to correct
    if (env.Lookup("Variable"))
    {
        m_correctionVariable = env.GetValue("Variable","");
    }
    else
    {
        ATH_MSG_ERROR("Correction variable is empty or not in configuration file.");
        return StatusCode::FAILURE;
    }

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

    // Save the type of all parameters in the correction function (assuming m_numberOfFunctionParameters parameters)
    for (int parameter_itr = 0; parameter_itr < m_numberOfFunctionParameters; parameter_itr++)
    {
        // if the parameter #parameter_itr is in the conf file, save its type
        TString parameterType = TString::Format("Parameter%dType",parameter_itr);
        if (env.Lookup(parameterType))
        {
            // convert string to ParameterType, fail if non-existing type
            FFToolApply::ParameterType type = StringToParameterType(env.GetValue(parameterType.Data(),""));
            if( type == FFToolApply::ParameterType::Failure )
            {
                ATH_MSG_ERROR("Parameter " << parameter_itr << " read-in failed, not an allowed parameter type.");
                return StatusCode::FAILURE;
            }
            // save type, get according type information and save it
            m_ParameterTypeVector.at(parameter_itr) = type;
            ATH_CHECK(GetParameterInformationFromConf(env, parameter_itr, type));
        }
        // else fail
        else
        {
            ATH_MSG_ERROR("Did not find Parameter" << parameter_itr << ", although you specified there were " << m_numberOfFunctionParameters << " parameters for the correction function.");
            return StatusCode::FAILURE;
        }
    } // end loop over all function parameters

    // ceck if this conf is only for correcting (un)converted photons
    std::string convertedPhotonFlag = env.GetValue("ConvertedPhotonsOnly","NO");
    std::string unconvertedPhotonFlag = env.GetValue("UnconvertedPhotonsOnly","NO");
    if (convertedPhotonFlag.compare("YES") == 0)
    {
        //will only deal with converted photons
        m_convertedPhotonsOnly = true;
    }
    if (unconvertedPhotonFlag.compare("YES") == 0)
    {
        //will only deal with unconverted photons
        m_unconvertedPhotonsOnly = true;
    }
    if (m_convertedPhotonsOnly && m_unconvertedPhotonsOnly)
    {
        ATH_MSG_ERROR("You stated this conf file is only for unconverted and only for converted photons, which is contradictory. If you want to use it for both, don't specify any of these flags.");
        return StatusCode::FAILURE;
    }

    //everything worked out, so
    return StatusCode::SUCCESS;
}
 
// ===========================================================================
// Application of correction
// ===========================================================================
const StatusCode FFToolApply::applyCorrection(xAOD::Photon& photon )
{
    // check if we should only deal with converted / unconverted photons
    ATH_CHECK(PassedCorrectPhotonType(photon));
    
    // create aux element accessor for correction
    SG::AuxElement::Accessor<float> VariableToCorrect(m_correctionVariable);
    // From the object, get the variable value according to the variable from the conf file
    // if variable not found, fail
    float original_variable = 0.;
    if( VariableToCorrect.isAvailable(photon) )
    {
        original_variable = VariableToCorrect(photon);
        //Save the original value to the photon under different name
        SG::AuxElement::Accessor<float> OriginalVariable(m_correctionVariable + "_original");
        OriginalVariable(photon) = original_variable;
    }
    else
    {
        ATH_MSG_ERROR("The correction variable \"" << m_correctionVariable << "\" provided in the conf file is not available.");
        return StatusCode::FAILURE;
    }
    
    //declare objects needed to retreive photon properties
    std::vector<float> properties; //safe value of function parameter i at place i
    properties.resize(m_numberOfFunctionParameters);
    float absEta; //safe absolute value of eta of event
    float pt; //safe pt of event

    //Get all the properties from the photon and fill them to properties, eta, pt
    ATH_CHECK(GetKinematicProperties(photon, pt, absEta));
    ATH_CHECK(GetCorrectionParameters(properties, pt, absEta));
    
    // Apply the correction, write to the corrected AuxElement
    ATH_CHECK(Correct(VariableToCorrect(photon),original_variable, properties));

    // everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode FFToolApply::applyCorrection(xAOD::Electron& electron )
{   
    if (m_convertedPhotonsOnly || m_unconvertedPhotonsOnly)
    {
        ATH_MSG_ERROR("You want to correct electrons, but set either ConvertedPhotonsOnly or UnconvertedPhotonsOnly to \"YES\". Are you using the correct conf file?");
        return StatusCode::FAILURE;
    }

    // create aux element accessors for correction
    SG::AuxElement::Accessor<float> VariableToCorrect(m_correctionVariable);
    // From the object, get the variable value according to the variable from the conf file
    // if variable not found, fail
    float original_variable = 0.;
    if (VariableToCorrect.isAvailable(electron))
    {
        original_variable = VariableToCorrect(electron);
        //Save the original value to the photon under different name
        SG::AuxElement::Accessor<float> OriginalVariable(m_correctionVariable + "_original");
        OriginalVariable(electron) = original_variable;
    }
    else
    {
        ATH_MSG_ERROR("The correction variable \"" << m_correctionVariable << "\" provided in the conf file is not available.");
        return StatusCode::FAILURE;
    }
    
    //declare objects needed to retreive electron properties
    std::vector<float> properties; //safe value of function parameter i at place i
    properties.resize(m_numberOfFunctionParameters);
    float absEta; //safe absolute value of eta of event
    float pt; //safe pt of event

    //Get all the properties from the electron and fill them to properties, eta, pt
    ATH_CHECK(GetKinematicProperties(electron, pt, absEta));
    ATH_CHECK(GetCorrectionParameters(properties, pt, absEta));
    
    // Apply the correction, write to the corrected AuxElement
    ATH_CHECK(Correct(VariableToCorrect(electron),original_variable, properties));

    // everything worked out, so
    return StatusCode::SUCCESS;
}

const StatusCode FFToolApply::Correct(float& return_corrected_variable, const float &original_variable, std::vector<float>& properties) const
{   
    // set the parameters of the correction function
    for (unsigned int parameter_itr = 0; parameter_itr < properties.size(); parameter_itr++)
    {
        m_correctionFunctionTF1->SetParameter(parameter_itr,properties.at(parameter_itr));
    }

    // Calculate corrected value
    return_corrected_variable = m_correctionFunctionTF1->Eval(original_variable);

    // everything worked out, so
    return StatusCode::SUCCESS;
}

// ===========================================================================
// Corrected Copies
// ===========================================================================
const StatusCode FFToolApply::correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon )
{
    out_photon = new xAOD::Photon(in_photon);
    return applyCorrection(*out_photon);
}

const StatusCode FFToolApply::correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron)
{
    out_electron = new xAOD::Electron(in_electron);
    return applyCorrection(*out_electron);
}

// ===========================================================================
// Helper Functions
// ===========================================================================

const StatusCode FFToolApply::GetKinematicProperties(const xAOD::Egamma& egamma_object, float& pt, float& absEta) const
{
    // just reteriving eta and pt is probably less expensive then checking if I need it and
    // then retreive it only if I actually need it

    // protection against bad clusters
    const xAOD::CaloCluster* cluster  = egamma_object.caloCluster();
    if ( cluster == 0 )
    {
        ATH_MSG_ERROR("EGamma object calorimeter cluster is NULL: Cluster " << cluster);
        return StatusCode::FAILURE;
    }

    // Fill variables
    // eta position in second sampling
    absEta   = fabsf(cluster->etaBE(2));
    // transverse energy in calorimeter (using eta position in second sampling)
    const double energy =  cluster->e();
    double et = 0.;
    if (absEta<999.) {
      const double cosheta = cosh(absEta);
      et = (cosheta != 0.) ? energy/cosheta : 0.;
    }
    pt = et;

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode FFToolApply::GetParameterInformationFromConf(TEnv& env, const int& parameter_number, const FFToolApply::ParameterType& type)
{
    // don't want to write the same code multiple times, so set flags when to retrieve eta/pt bins
    bool getEtaBins = false;
    bool getPtBins = false;
    // form strings according to which parameter to retreive
    TString filePathKey = TString::Format("Parameter%dFile",parameter_number);
    TString graphNameKey = TString::Format("Parameter%dGraphName",parameter_number);
    TString binValues = TString::Format("Parameter%dValues",parameter_number);
    // helpers
    TString filePath = "";
    TString graphName = "";

    // according to the parameter type, retreive the information from conf
    if (type == FFToolApply::ParameterType::EtaDependentTGraph || type == FFToolApply::ParameterType::PtDependentTGraph)
    {
        // check if necessary information is in conf, else fail
        if (env.Lookup(filePathKey))
        {
            //get the path to the root file where the graph is saved
            filePath = PathResolverFindCalibFile(env.GetValue(filePathKey.Data(),""));
        }
        else
        {
            ATH_MSG_ERROR("Could not retreive parameter %d file path.");
            return StatusCode::FAILURE;
        }
        // check if necessary information is in conf, else fail
        if (env.Lookup(graphNameKey))
        {
            //get the name of the TGraph
            graphName = env.GetValue(graphNameKey.Data(),"");
        }
        else
        {
            ATH_MSG_ERROR("Could not retreive parameter %d graph name.");
            return StatusCode::FAILURE;
        }
        // open file, get graph, store a copy
        //TODO: Check if opening file works & if graph is found, else warning + fail
        std::unique_ptr<TFile> file (new TFile(filePath.Data(),"READ"));
        std::unique_ptr<TGraph> graph ((TGraph*)file->Get(graphName.Data()));
        m_graphCopies.at(parameter_number) = (TGraph*)graph->Clone(); //Or use copy constructor?
        file->Close();
    }
    else if (type == FFToolApply::ParameterType::EtaBinned )
    {
        //get eta binning later
        getEtaBins = true;
    }
    else if (type == FFToolApply::ParameterType::PtBinned )
    {
        //get pt binning later
        getPtBins = true;
    }
    else if (type == FFToolApply::ParameterType::EtaTimesPtBinned )
    {
        //get eta and pt binning later
        getEtaBins = true;
        getPtBins = true;
    }
    else if (type == FFToolApply::ParameterType::EventDensity )
    {
        // nothing has to be retrieved, no additional parameters for EventDensity currently
        return StatusCode::SUCCESS;
    }

    // if needed and not already retreived, get eta binning
    if (getEtaBins && !m_retreivedEtaBinning)
    {
        // check if necessary information is in conf, else fail
        if (env.Lookup("EtaBins"))
        {
            //get the eta binning (global!)
            m_etaBins = AsgConfigHelper::HelperFloat("EtaBins",env);
            //force that the low bin edges are given by the conf file, starting with 0
            if (m_etaBins.at(0) != 0.)
            {
                ATH_MSG_ERROR("Lowest bin edge given for parameter " << parameter_number << " is not 0. Please provide the lower bin edges of your correction binning in the conf file, starting with 0.");
                return StatusCode::FAILURE;
            }
            // don't want to retreive the same thing twice from conf
            m_retreivedEtaBinning = true;
        }
        else
        {
            ATH_MSG_ERROR("Could not retreive eta binning.");
            return StatusCode::FAILURE;
        }
    }
    // if needed and not already retreived, get pt binning
    if (getPtBins && !m_retreivedPtBinning)
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
            // don't want to retreive the same thing twice from conf
            m_retreivedPtBinning = true;
        }
        else
        {
            ATH_MSG_ERROR("Could not retreive pt binning.");
            return StatusCode::FAILURE;
        }
    }
    // if needed, get binned correction values
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
            ATH_MSG_ERROR("Could not retreive binned values.");
            return StatusCode::FAILURE;
        }
    }

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode FFToolApply::GetCorrectionParameters(std::vector<float>& properties, const float& pt, const float& absEta) const
{
    // according to the parameter type, get the actual parameter going to the correction function
    // for this, loop over the parameter type vector
    for (unsigned int parameter_itr = 0; parameter_itr < m_ParameterTypeVector.size(); parameter_itr++)
    {
        FFToolApply::ParameterType type = m_ParameterTypeVector.at(parameter_itr);
        if (type == FFToolApply::ParameterType::EtaDependentTGraph)
        {
            // evaluate TGraph at abs(eta)
            properties.at(parameter_itr) = m_graphCopies.at(parameter_itr)->Eval(absEta);
        }
        else if (type == FFToolApply::ParameterType::PtDependentTGraph)
        {
            // evaluate TGraph at pt
            properties.at(parameter_itr) = m_graphCopies.at(parameter_itr)->Eval(pt);
        }
        else if (type == FFToolApply::ParameterType::EtaBinned)
        {
            // get value of correct eta bin
            ATH_CHECK(Get1DBinnedParameter(properties.at(parameter_itr),absEta,m_etaBins,parameter_itr));
        }
        else if (type == FFToolApply::ParameterType::PtBinned)
        {
            // get value of correct pt bin
            ATH_CHECK(Get1DBinnedParameter(properties.at(parameter_itr),pt,m_ptBins,parameter_itr));
        }
        else if (type == FFToolApply::ParameterType::EtaTimesPtBinned)
        {
            // get value of correct eta x pt bin
            ATH_CHECK(Get2DBinnedParameter(properties.at(parameter_itr),absEta,pt,parameter_itr));
        }
        else if (type == FFToolApply::ParameterType::EventDensity)
        {
            // get event density
            ATH_CHECK(GetDensity(properties.at(parameter_itr), "TopoClusterIsoCentralEventShape"));
        }
    }

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode FFToolApply::Get1DBinnedParameter(float& return_parameter_value, const float& evalPoint, const std::vector<float>& binning, const int& parameter_number) const
{
    ANA_MSG_VERBOSE("Get 1DBinnedParameters...");
    ANA_MSG_VERBOSE("EvalPoint: " << evalPoint);
    // check in which bin the evalPoint is
    // if the evalPoint is < 0, something is very wrong
    if (evalPoint < binning.at(0))
    {
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
            return_parameter_value = m_binValues.at(parameter_number).at(bin_itr);
            return StatusCode::SUCCESS;
        }
    }
    //if this point is ever reached, the evaluation point is larger then the largest lowest bin edge
    //if that's the case, return the value for the last bin
    //the -1 is because the parameter numbering in a vector starts at 0
    return_parameter_value = m_binValues.at(parameter_number).at(m_binValues.size()-1);

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode FFToolApply::Get2DBinnedParameter(float& return_parameter_value, const float& etaEvalPoint, const float& ptEvalPoint, const int& parameter_number) const
{
    //need to find eta bin, and need to find pt bin
    //from this, calculate which parameter of the list is needed to be returned.
    int etaBin = -1;
    int ptBin = -1;

    // get eta bin
    //run only up to binning.size()-1, as running to binning.size() will get a seg fault for the boundary check
    for (unsigned int etaBin_itr = 0; etaBin_itr < m_etaBins.size()-1; etaBin_itr++)
    {
        // if the evaluation point is within the checked bins boundaries, this is the value we want
        if (etaEvalPoint > m_etaBins.at(etaBin_itr) && etaEvalPoint < m_etaBins.at(etaBin_itr))
        {
            // we found the according bin, so set to the according value
            etaBin = etaBin_itr;
        }
    }
    //if etaBin was not set yet, it's larger then the largest lower bin edge
    //thus, need to set it to the last bin
    if (etaBin == -1)
    {
        //need to correct for vector numbering starting at 0
        etaBin = m_etaBins.size()-1;
    }

    // get pt bin
    //run only up to binning.size()-1, as running to binning.size() will get a seg fault for the boundary check
    for (unsigned int ptBin_itr = 0; ptBin_itr < m_ptBins.size()-1; ptBin_itr++)
    {
        // if the evaluation point is within the checked bins boundaries, this is the value we want
        if (ptEvalPoint > m_ptBins.at(ptBin_itr) && ptEvalPoint < m_ptBins.at(ptBin_itr))
        {
            // we found the according bin, so set to the according value
            ptBin = ptBin_itr;
        }
    }
    //if ptBin was not set yet, it's larger then the largest lower bin edge
    //thus, need to set it to the last bin
    if (ptBin == -1)
    {
        //need to correct for vector numbering starting at 0
        ptBin = m_ptBins.size()-1;
    }

    // return the value corresponding to the pt x eta bin found
    /* Note: Assuming that the values are binned in pt x eta in the conf file:
     *           eta bin 0 | eta bin 1 | eta bin 2 | eta bin 3 | eta bin 4 | etc.
     * pt bin 0      0           1           2           3           4
     * pt bin 1      5           6           7           8           9
     * pt bin 2     10          11          12          13          14
     * pt bin 3     15          16          17          18          19
     * pt bin 4     20          21          22          23          24
     * etc.
     * the correct parameter is saved in the vector at 4*ptBinNumber + etaBinNumber
     * */
    return_parameter_value = m_binValues.at(parameter_number).at(4 * ptBin + etaBin);

    // everything went fine, so
    return StatusCode::SUCCESS;
}

const StatusCode FFToolApply::GetDensity(float& value, const std::string& eventShapeContainer) const
{
    // retreive the event shape container
    const xAOD::EventShape* evtShape;
    if(evtStore()->retrieve(evtShape, eventShapeContainer).isFailure()){
        ATH_MSG_ERROR("Cannot retrieve density container " << eventShapeContainer);
        return StatusCode::FAILURE;
    }
    // get the density from the container
    value = evtShape->getDensity(xAOD::EventShape::Density);
    return StatusCode::SUCCESS;
}

FFToolApply::ParameterType FFToolApply::StringToParameterType( const std::string& input ) const
{
    // return parameter type according to string given in conf file
    if( input == "EtaDependentTGraph") return FFToolApply::ParameterType::EtaDependentTGraph;
    else if( input == "PtDependentTGraph") return FFToolApply::ParameterType::PtDependentTGraph;
    else if( input == "EtaBinned") return FFToolApply::ParameterType::EtaBinned;
    else if( input == "PtBinned") return FFToolApply::ParameterType::PtBinned;
    else if( input == "EtaTimesPtBinned") return FFToolApply::ParameterType::EtaTimesPtBinned;
    else if( input == "EventDensity") return FFToolApply::ParameterType::EventDensity;
    else
    {
        // if not a proper type, return failure type - check and fail on this!
        ATH_MSG_ERROR(input.c_str() << " is not an allowed parameter type.");
        return FFToolApply::ParameterType::Failure;
    }
}

const StatusCode FFToolApply::PassedCorrectPhotonType(const xAOD::Photon& photon) const
{
    //check if the passed photon type is the same as the requested photon type, if only one photon type was requested
    if (m_convertedPhotonsOnly != m_unconvertedPhotonsOnly)
    {
        bool isConvertedPhoton = xAOD::EgammaHelpers::isConvertedPhoton(&photon);
        bool isUnconvertedPhoton = !isConvertedPhoton;
        //check if the correct photon was passed to the tool (unconverted/converted)
        //if the photon is not ((converted and converted requested) or (unconverted and unconverted requested)) fail
        if (!((isConvertedPhoton && m_convertedPhotonsOnly) || (isUnconvertedPhoton && m_unconvertedPhotonsOnly)))
        {
            if (m_convertedPhotonsOnly)
            {
                ATH_MSG_ERROR("You requested to only correct converted photons, but passed an unconverted one.");
            }
            else
            {
                ATH_MSG_ERROR("You requested to only correct unconverted photons, but passed a converted one.");
            }
            return StatusCode::FAILURE;
        }
    }
    return StatusCode::SUCCESS;
}