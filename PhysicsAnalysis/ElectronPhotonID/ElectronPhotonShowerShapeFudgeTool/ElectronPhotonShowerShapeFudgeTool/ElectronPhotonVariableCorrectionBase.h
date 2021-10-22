/*
    Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ElectronPhotonVariableCorrectionBase_H
#define ElectronPhotonVariableCorrectionBase_H

/** @class ElectronPhotonVariableCorrectionBase
 * @brief Class to correct electron and photon MC variables.
 * @details For a detailed documentation, please refer to the [gitlab readme](https://gitlab.cern.ch/atlas/athena/-/blob/21.2/PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool/README.md)
 * 
 * @author Nils Gillwald (DESY) nils.gillwald@desy.de
 * @date   February 2020
*/

//ATLAS includes
#include "AsgTools/AsgTool.h"
#include "PATInterfaces/CorrectionCode.h"

//EDM includes
#include "xAODEgamma/Electron.h"
#include "xAODEgamma/Photon.h"

//Root includes
#include "TF1.h"

// forward declarations
class TObject;
class TGraph;
class TH2;
class TEnv;

// ===========================================================================
// Class ElectronPhotonVariableCorrectionBase
// ===========================================================================

class ElectronPhotonVariableCorrectionBase : public asg::AsgTool
{

public:
    /** @brief Standard constructor
     * @param myname Internal name of the class instance, so they can be distinguished
     */ 
    ElectronPhotonVariableCorrectionBase(const std::string& myname);
    //! @brief Standard destructor
    ~ElectronPhotonVariableCorrectionBase() {};

    /** @brief Initialize the class instance
     * @details Reads out the configuration file set via the setProperty(ConfigFile, "/path/to/conf/file.conf") function
     * and sets up the class instance accordingly
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
    const CP::CorrectionCode applyCorrection( xAOD::Photon& photon ) const;

    /** @brief Apply the correction given in the conf file to the passed electron
     * @param electron The electron which should be corrected
     * @details Pass an electron which should be corrected by the tool to this function. The tool gets the original electron variable value,
     * corrects it according to the configuration file given, and overwrites the original value of the variable. The original variable
     * is saved as <variable_name>_original. Note that the variable to be corrected should be an auxiliary variable, and that the electron
     * or electron container, respectively, must not be const if you want to use this function. If you are running on const objects, please
     * make a deep / shallow copy of the container or use the correctedCopy function of this class.
     */
    const CP::CorrectionCode applyCorrection( xAOD::Electron& electron ) const;

    /** @brief Make a corrected copy of the passed photon according to the given conf file
     * @param in_photon The original photon of which a corrected copy should be made
     * @param out_photon Empty new photon where the corrected copy will be stored in
     * @details Makes a corrected copy of the passed photon object according to the correction provided in the configuration file. The in_photon
     * is copied to the out_photon, which is then passed to the applyCorrection function.
     */
    const CP::CorrectionCode correctedCopy( const xAOD::Photon& in_photon, xAOD::Photon*& out_photon ) const;

    /** @brief Make a corrected copy of the passed electron according to the given conf file
     * @param in_electron The original electron of which a corrected copy should be made
     * @param out_electron Empty new electron where the corrected copy will be stored in
     * @details Makes a corrected copy of the passed electron object according to the correction provided in the configuration file. The in_electron
     * is copied to the out_electron, which is then passed to the applyCorrection function.
     */
    const CP::CorrectionCode correctedCopy( const xAOD::Electron& in_electron, xAOD::Electron*& out_electron) const;

    //! @brief Returns the variable which should be corrected according to the passed configuration file
    const std::string getCorrectionVariable() { return m_correctionVariable; };

    /** @brief Define the categories of EGamma objects tool can be applied to
     * @details The tool can be applied to electrons and photons, but the latter could also have different corrections according to whether they are
     * converted or unconverted photons. The categories are designed in such a way, that the analyzer can freely choose whether he_she_* wants all objects
     * (i.e. e and y) to be corrected in the same way, or wheter he_she_* wants to apply different corrections to different types of objects.
     */
    enum class EGammaObjects{
        Failure = 0,
        unconvertedPhotons = 1,
        convertedPhotons,
        allPhotons,
        allElectrons,
        allEGammaObjects
    }; //end enum EGammaObjects

    //! @brief Returns the type of EGamma object to which the correction should be applied.
    ElectronPhotonVariableCorrectionBase::EGammaObjects isAppliedTo() { return m_applyToObjects; };

    /** @brief Check if the ApplyTo flag passed in the conf file is compatible with converted photons
     * @return True if the ApplyTo flag is compatible, False if not
     */
    bool applyToConvertedPhotons() const;

    /** @brief Check if the ApplyTo flag passed in the conf file is compatible with uconverted photons
     * @return True if the ApplyTo flag is compatible, False if not
     */
    bool applyToUnconvertedPhotons() const;

    /** @brief Check if the ApplyTo flag passed in the conf file is compatible with electrons
     * @return True if the ApplyTo flag is compatible, False if not
     */
    bool applyToElectrons() const; //check if passed flag is compatible with electron

private:
    //! @brief Use enum and not string for type of function parameter in order to do faster comparisons
    enum class parameterType{
        Failure = 0,
        EtaDependentTGraph = 1,
        PtDependentTGraph,
        EtaBinned,
        PtBinned,
        EtaTimesPtBinned,
        EtaTimesPhiTH2,
        EventDensity
    }; // end enum ParameterType
    //! @brief The name of the configuration file
    std::string m_configFile;
    //! @brief The name of the variable to correct
    std::string m_correctionVariable;
    //! @brief Whether to apply normal correction or smearing correction
    bool m_doGaussianSmearing = false;
    //! @brief Values of discontinuities in the variable which should not be corrected
    std::vector<float> m_uncorrectedDiscontinuities;
    //! @brief Function to use for the variable correction, TF1 style
    std::string m_correctionFunctionString;
    //! @brief The actual TF1 correction function
    std::unique_ptr<TF1> m_correctionFunctionTF1;
    //! @brief Number of parameters of the variable correction function
    int m_numberOfFunctionParameters;
    //! @brief Map of the correction function parameter number to the parameter type
    std::vector<parameterType> m_ParameterTypeVector;
    //! @brief Copy of the TGraph from the root file, stored if needed by the respective correction function parameter
    std::vector<TGraph*> m_graphCopies;
    //! @brief Copy of the TH2 from the root file, stored if needed by the respective correction function parameter
    std::vector<TH2*> m_TH2Copies;
    //! @brief Store the lowest eta bin boundary: used for checking if the respective TH2 needs the eta or abs(eta) value for evaluation
    std::vector<float> m_useAbsEtaTH2;
    //! @brief List of eta/pt dependent values, stored if needed by the respective correction function parameter
    std::vector<std::vector<float>> m_binValues;
    //! @brief List of bin boundaries in eta, stored if needed by any correction function parameter
    std::vector<float> m_etaBins;
    //! @brief Store if the eta binned parameters need the eta or abs(eta) value for evaluation
    bool m_useAbsEtaBinned;
    //! @brief List of bin boundaries in pT, stored if needed by any correction function parameter
    std::vector<float> m_ptBins;
    //! @brief List of bools whether a parameter should use linear interpolation in pT if it's some kind of pT binned parameter
    std::vector<bool> m_interpolatePtFlags;
    //! @brief The type of objects to which the specific conf file settings are allowed to be applied to
    ElectronPhotonVariableCorrectionBase::EGammaObjects m_applyToObjects;
    //! @brief Store if already retrieved eta binning
    bool m_retrievedEtaBinning = false;
    //! @brief Store if already retrieved pt binning
    bool m_retrievedPtBinning = false;
    //! @brief  Accessor for the variable to be corrected
    std::unique_ptr<SG::AuxElement::Accessor<float>> m_variableToCorrect;
    //! @brief  Accessor to store the original value of the corrected variable
    std::unique_ptr<SG::AuxElement::Accessor<float>> m_originalVariable;

    /** @brief Convert input string to a parameter function type
     * @param input The string to convert to ElectronPhotonVariableCorrectionBase::parameterType
     * @return ElectronPhotonVariableCorrectionBase::parameterType, returns ElectronPhotonVariableCorrectionBase::parameterType::Failure if no match found
     */
    ElectronPhotonVariableCorrectionBase::parameterType stringToParameterType( const std::string& input ) const;

    /** @brief Convert input string to egamma object type
     * @param input The string to convert to ElectronPhotonVariableCorrectionBase::EGammaObjects
     * @return ElectronPhotonVariableCorrectionBase::EGammaObjects, returns ElectronPhotonVariableCorrectionBase::EGammaObjects::Failure if no match found
     */
    ElectronPhotonVariableCorrectionBase::EGammaObjects stringToEGammaObject( const std::string& input ) const;

    /** @brief Check if the photon which was passed to the tool has the correct type, if only (un)converted photons were requested to be corrected
     * @param photon The photon which was passed to the tool to be corrected
     */
    bool passedCorrectPhotonType(const xAOD::Photon& photon) const;

    /** @brief Check if the value passed is equal to one of the values passed via the UncorrectedDiscontinuites flag and should thus not be corrected
     * @param value The value which should be checked
     */
    bool isEqualToUncorrectedDiscontinuity(const float value) const;

    /** @brief Get the relevant information for a correction function parameter from the given conf file
     * @param env The given TEnv, which is used to read out the current conf file
     * @param parameter_number The parameter number with respect to the m_correctionFunctionTF1
     * @param type The type of the respective parameter with this parameter_number
     * @details The relevant information for the parameter with the given parameter number and given parameter type is retrieved from the current configuration
     * file, and saved in the according member variables of the class, i.e. m_graphCopies, m_binValues, m_etaBins, m_ptBins
     */
    const StatusCode getParameterInformationFromConf(TEnv& env, const int parameter_number, const ElectronPhotonVariableCorrectionBase::parameterType type);

    /** @brief Get the eta and pt binning as well as the respective correction values from the given conf file
     * @param getEtaBins if to get the eta binning and bin values
     * @param getPtBins if to get the pt binning and bin values
     * @param binValues the conf file key to retrieve the bin values
     * @param interpolate the conf file key on wheather to interpolate the pt values
     * @param env The given TEnv,, which is used to read out the current conf file
     * @param parameter_number The parameter number with respect to the m_correctionFunctionTF1
     */
    const StatusCode getEtaPtBinningsFromConf(const bool getEtaBins, const bool getPtBins, const TString& binValues, const TString& interpolate, TEnv& env, const int parameter_number);

    /** @brief Get a TObject storing corrections (i.e. TGraph or TH2) from a root file
     * @param env The configuration file to search for the relevant information (file path, object name)
     * @param parameter_number The number of the respective parameter, used to display error messages correctly
     * @param filePathKey The key for finding the file path in env
     * @param nameKey The key for finding the object name in env
     * @param return_object The retrieved TObject will be saved in this variable
     * @details The TObject with the name matching the key nameKey stored in the file with path matching filePathKey in the configuration file env will be retrieved
     * and saved in return_object.
     */
    const StatusCode getObjectFromRootFile(TEnv& env, const int parameter_number, const TString& filePathKey, const TString& nameKey, std::unique_ptr<TObject>& return_object);

    /** @brief Get the actual parameters entering the correction TF1 for the current e/y object to be corrected
     * @param properties The vector where the values of the correction TF1 parameters will be saved in
     * @param pt The pT of the current e/y object to be corrected
     * @param eta The eta of the current e/y object to be corrected
     * @details As every electron/photon has different values of pT/eta, the correction function must be adapted accordingly for every e/y. The according values of
     * each of the correction function parameters are updated with this function.
     */
    const StatusCode getCorrectionParameters(std::vector<float>& properties, const float pt, const float eta, const float phi) const;

    /** @brief Get the correction function parameter value if its type is eta- or pT-binned
     * @param return_parameter_value The respective correction function parameter value is saved in this parameter
     * @param evalPoint pT or eta evaluation point - i.e., the eta or pT value of the current e/y object to be corrected. Used to find the correct eta/pT bin to use for the correction
     * @param binning The eta or pT binning
     * @param parameter_number The number of the parameter with respect to the correction TF1. Needed in order to retrieve the correct values matching this parameter.
     */
    const StatusCode get1DBinnedParameter(float& return_parameter_value, const float evalPoint, const std::vector<float>& binning, const int parameter_number) const;

    /** @brief Get the correction function parameter value if its type is eta- and pT-binned
     * @param return_parameter_value The respective correction function parameter value is saved in this parameter
     * @param etaEvalPoint eta evaluation point - i.e., the eta value of the current e/y object to be corrected. Used to find the correct eta bin to use for the correction
     * @param ptEvalPoint pT evaluation point - i.e., the pT value of the current e/y object to be corrected. Used to find the correct pT bin to use for the correction
     * @param parameter_number The number of the parameter with respect to the correction TF1. Needed in order to retrieve the correct values matching this parameter.
     */
    const StatusCode get2DBinnedParameter(float& return_parameter_value, const float etaEvalPoint, const float ptEvalPoint, const int parameter_number) const;

    /** @brief Get the correction function parameter value if its type is eta- and pT-binned
     * @param return_parameter_value The respective correction function parameter value is saved in this parameter
     * @param etaEvalPoint eta evaluation point - i.e., the eta value of the current e/y object to be corrected. Used to find the correct eta bin to use for the correction
     * @param phiEvalPoint phi evaluation point - i.e., the phi value of the current e/y object to be corrected. Used to find the correct phi bin to use for the correction
     * @param parameter_number The number of the parameter with respect to the correction TF1. Needed in order to retrieve the correct values matching this parameter.
     */
    const StatusCode get2DHistParameter(float& return_parameter_value, const float etaEvalPoint, const float phiEvalPoint, const int parameter_number) const;

    /** @brief Find the bin number in which the evalPoint is located in the binning binning.
     * @param return_bin The respective bin number is saved in this parameter
     * @param evalPoint The evaluation point for which the bin number should be found
     * @param binning The binning which should be evaluated
     */
    const StatusCode findBin(int& return_bin, const float evalPoint, const std::vector<float>& binning) const;

    /** @brief Return the interpolation flag of parameter parameter_number as a boolean.
     * @param parameter_number Number of the parameter for which the interpolation flag should be checked
     * @return The interpolation flag of parameter parameter_number (boolean)
     */
    bool getInterpolationFlag(const int parameter_number) const;

    /** @brief Given a point x, approximates the value via linear interpolation based on the two nearest bin centers. Re-implementation of Double_t TH1::Interpolate( Double_t x) const.
     * @param return_parameter_value The interpolated parameter value is saved in this parameter
     * @param evalPoint The point for which the interpolation should be done
     * @param bin The bin in which the evalPoint is located with respect to binning
     * @param binning The binning based on which the interpolation should be done
     * @param binValues The bin values according to the binning given in binning
     */
    const StatusCode interpolate(float& return_parameter_value, const float evalPoint, const int bin, const std::vector<float>& binning, const std::vector<float>& binValues) const;

    /** @brief Get the bin center of a bin bin_int using the binning binning
     * @param return_bin_center The bin center is saved in this parameter
     * @param binning The binning which should be used to find the bin centers
     * @param bin_int The bin for which the bin center should be found
     */
    const StatusCode getBinCenter(float& return_bin_center, const std::vector<float>& binning, const int bin_int) const;

    /** @brief Returns the linearly intrpolated value of value given the bin centers and bin values
     * @param value The x-value at which the interpolation should be done
     * @param left_bin_center The x-value of the left bin center used for the interpolation
     * @param left_bin_value The y-value of the left bin at the left bin center
     * @param right_bin_center The x-value of the right bin center used for the interpolation
     * @param right_bin_value The y-value of the right bin at the right bin center
     */
    float interpolate_function(const float value, const float left_bin_center, const float left_bin_value, const float right_bin_center, const float right_bin_value) const;

    /** @brief Get the events energy density from the eventShape
     * @param value The respective correction function parameter value is saved in this parameter
     * @param eventShapeContainer The name of the respective event shape container to use
     */
    const StatusCode getDensity(float& value, const std::string& eventShapeContainer) const;

    /** @brief Get the e/y kinematic properties
     * @param egamma_object The e/y object to get the kinemativ properties of
     * @param pt The pT value is saved in this parameter
     * @param eta The eta value is saved in this parameter
     * @details As every electron/photon has different values of pT/eta, the correction function must be adapted accordingly for every e/y. The according values of
     * eta and pt are updated with this function.
     */
    const StatusCode getKinematicProperties(const xAOD::Egamma& egamma_object, float& pt, float& eta, float& phi) const;

    /** @brief Actual application of the correction to the variable
     * @param return_corrected_variable The corrected variable value is saved in this parameter
     * @param original_variable The original value of the corrected variable
     * @param properties The vector containing the correction TF1 parameters so the correction TF1 can be set for the respective e/y object
     */
    const StatusCode correct(float& return_corrected_variable, const float original_variable, std::vector<float>& properties) const;

}; //end class ElectronPhotonVariableCorrectionBase

#endif
