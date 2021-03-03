/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

/**
  @class AsgElectronSelectorTool
  @brief Electron selector tool to select signal electrons using the ElectronDNNCalculator
         retrieve a score based on a DNN. This includes preprocessing/transformations of the
         input variables and the actual calculation of the score using lwtnn.

  @author Lukas Ehrke, Daniel Nielsen, Troels Petersen
  @note   heavily adapted from AsgElectronLikelihoodTool
*/

// Include this class's header
#include "ElectronPhotonSelectorTools/AsgElectronSelectorTool.h"
#include "ElectronPhotonSelectorTools/ElectronSelectorHelpers.h"
#include "AsgElectronPhotonIsEMSelectorConfigHelper.h"
#include "EGSelectorConfigurationMapping.h"
#include "ElectronDNNCalculator.h"
// STL includes
#include <string>
#include <cstdint>
#include <cmath>

//EDM includes
#include "xAODEgamma/Electron.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "TEnv.h"

#include "AsgTools/CurrentContext.h"
#include "PathResolver/PathResolver.h"


//=============================================================================
// Standard constructor
//=============================================================================
AsgElectronSelectorTool::AsgElectronSelectorTool( std::string myname ) :
  AsgTool(myname),
  m_configFile{""},
  m_mvaTool(nullptr)
{

  // Declare the needed properties
  declareProperty("WorkingPoint", m_workingPoint="", "The Working Point");
  declareProperty("ConfigFile", m_configFile="", "The config file to use");

  // model file name. Managed in the ElectronDNNCalculator.
  declareProperty("inputModelFileName", m_modelFileName="", "The input file name that holds the model" );
  // QuantileTransformer file name ( required for preprocessing ). Managed in the ElectronDNNCalculator.
  declareProperty("quantileFileName", m_quantileFileName="", "The input file name that holds the QuantileTransformer");
  // Variable list
  declareProperty("Variables", m_variables, "Variables used in the MVA tool");
  // The mva cut values
  declareProperty("CutSelector", m_cutSelector, "Cut on MVA discriminant");
  // do the ambiguity cut
  declareProperty("CutAmbiguity" , m_cutAmbiguity, "Apply a cut on the ambiguity bit");
  // cut on b-layer
  declareProperty("CutBL", m_cutBL, "Apply a cut on b-layer");
  // cut on pixel hits
  declareProperty("CutPi", m_cutPi, "Apply a cut on pixel hits");
  // cut on precision hits
  declareProperty("CutSCT", m_cutSCT, "Apply a cut on SCT hits");
  // use smooth interpolation between discriminant bins
  declareProperty("doSmoothBinInterpolation", m_doSmoothBinInterpolation, "use smooth interpolation between discriminant bins");
}


//=============================================================================
// Standard destructor
//=============================================================================
AsgElectronSelectorTool::~AsgElectronSelectorTool()
{
}


//=============================================================================
// Asgena initialize method
//=============================================================================
StatusCode AsgElectronSelectorTool::initialize()
{
  if (!m_workingPoint.empty()){
    m_configFile = AsgConfigHelper::findConfigFile(m_workingPoint, EgammaSelectors::ElectronDNNPointToConfFile);
    ATH_MSG_INFO("operating point : " << this->getOperatingPointName());
  }

  if (!m_configFile.empty()){
    std::string configFile = PathResolverFindCalibFile(m_configFile);
    if (configFile==""){
      ATH_MSG_ERROR("Could not locate " << m_configFile);
      return StatusCode::FAILURE;
    }


    ATH_MSG_DEBUG("Configfile to use: " << m_configFile);
    TEnv env(configFile.c_str());


    std::string modelFilename("");
    std::string quantileFilename("");

    // Get the input model in the tool.
    ATH_MSG_DEBUG("Get the input model in the tool.");

    if (!m_modelFileName.empty()){  // If the property was set by the user, take that.
      ATH_MSG_INFO("Setting user specified Model file: " << m_modelFileName);
      modelFilename = m_modelFileName;
    }
    else {
      modelFilename = env.GetValue("inputModelFileName", "ElectronPhotonSelectorTools/offline/mc16_20210204/ElectronDNNNetwork.json");
      ATH_MSG_DEBUG("Getting the input Model from: " << modelFilename );
    }
    std::string filename = PathResolverFindCalibFile(modelFilename);
    if (filename.empty()){
      ATH_MSG_ERROR("Could not find model file " << modelFilename);
      return StatusCode::FAILURE;
    }

    // Get the input transformer in the tool.
    ATH_MSG_DEBUG("Get the input transformer in the tool.");

    if (!m_quantileFileName.empty()){  // If the property was set by the user, take that.
      ATH_MSG_INFO("Setting user specified QuantileTransformer file: " << m_quantileFileName);
      quantileFilename = m_quantileFileName;
    }
    else {
      quantileFilename = env.GetValue("inputQuantileFileName", "ElectronPhotonSelectorTools/offline/mc16_20210204/ElectronDNNQuantileTransformer.root");
      ATH_MSG_DEBUG("Getting the input QuantileTransformer from: " << quantileFilename);
    }
    std::string qfilename = PathResolverFindCalibFile(quantileFilename);
    if (qfilename.empty()){
        ATH_MSG_ERROR("Could not find QuantileTransformer file " << quantileFilename);
        return StatusCode::FAILURE;
    }

    // Variables used in the MVA tool as comma separated string;
    std::stringstream vars(env.GetValue("Variables", ""));
    // parse variables string into vector
    while(vars.good()){
      std::string substr;
      std::getline(vars, substr, ',');
      m_variables.push_back( substr );
    }

    // Create an instance of the class calculating the DNN score
    m_mvaTool = std::make_unique<ElectronDNNCalculator>(this, filename.c_str(), qfilename.c_str(), m_variables);

    // cut on MVA discriminant
    m_cutSelector = AsgConfigHelper::HelperDouble("CutSelector", env);
    // cut on ambiuity bit
    m_cutAmbiguity = AsgConfigHelper::HelperInt("CutAmbiguity", env);
    // cut on b-layer
    m_cutBL = AsgConfigHelper::HelperInt("CutBL", env);
    // cut on pixel hits
    m_cutPi = AsgConfigHelper::HelperInt("CutPi", env);
    // cut on precision hits
    m_cutSCT = AsgConfigHelper::HelperInt("CutSCT", env);
    // do smooth interpolation between bins
    m_doSmoothBinInterpolation = env.GetValue("doSmoothBinInterpolation", false);



    unsigned int numberOfExpectedBinCombinedMVA ;
    numberOfExpectedBinCombinedMVA = s_fnDiscEtBins * s_fnDiscEtaBins;
    unsigned int numberOfExpectedEtaBins = s_fnDiscEtBins;

    if (m_cutSelector.size() != numberOfExpectedBinCombinedMVA){
      ATH_MSG_ERROR("Configuration issue :  cutSelector expected size " << numberOfExpectedBinCombinedMVA <<
                    " input size " << m_cutSelector.size());
      return StatusCode::FAILURE;
    }

    if (m_cutSCT.size()){
      if (m_cutSCT.size() != numberOfExpectedEtaBins){
        ATH_MSG_ERROR("Configuration issue :  cutSCT expected size " << numberOfExpectedEtaBins <<
                      " input size " << m_cutSCT.size());
        return StatusCode::FAILURE;
      }
    }

    if (m_cutPi.size()){
      if (m_cutPi.size() != numberOfExpectedEtaBins){
        ATH_MSG_ERROR("Configuration issue :  cutPi expected size " << numberOfExpectedEtaBins <<
                      " input size " << m_cutPi.size());
        return StatusCode::FAILURE;
      }
    }

    if (m_cutBL.size()){
      if (m_cutBL.size() != numberOfExpectedEtaBins){
        ATH_MSG_ERROR("Configuration issue :  cutBL expected size " << numberOfExpectedEtaBins <<
                      " input size " << m_cutBL.size());
        return StatusCode::FAILURE;
      }
    }

    if (m_cutAmbiguity.size()){
      if (m_cutAmbiguity.size() != numberOfExpectedEtaBins){
        ATH_MSG_ERROR("Configuration issue :  cutAmbiguity expected size " << numberOfExpectedEtaBins <<
                      " input size " << m_cutAmbiguity.size());
        return StatusCode::FAILURE;
      }
    }

    // --------------------------------------------------------------------------
    // Register the cuts and check that the registration worked:
    // NOTE: THE ORDER IS IMPORTANT!!! Cut0 corresponds to bit 0, Cut1 to bit 1,...
    // use an int as a StatusCode
    int sc(1);

    // Cut position for the kineatic pre-selection
    m_cutPosition_kinematic = m_acceptMVA.addCut("kinematic", "pass kinematic");
    if (m_cutPosition_kinematic < 0) sc = 0;

    // NSilicon
    m_cutPosition_NSilicon = m_acceptMVA.addCut("NSCT", "pass NSCT");
    if (m_cutPosition_NSilicon < 0) sc = 0;

    // NPixel
    m_cutPosition_NPixel = m_acceptMVA.addCut("NPixel", "pass NPixel");
    if (m_cutPosition_NPixel < 0) sc = 0;

    // NBlayer
    m_cutPosition_NBlayer = m_acceptMVA.addCut("NBlayer", "pass NBlayer");
    if (m_cutPosition_NBlayer < 0) sc = 0;

    // Ambiguity
    m_cutPosition_ambiguity = m_acceptMVA.addCut("ambiguity", "pass ambiguity");
    if (m_cutPosition_ambiguity < 0) sc = 0;


    // Cut position for the likelihood selection - DO NOT CHANGE ORDER!
    m_cutPosition_MVA = m_acceptMVA.addCut("passMVA", "pass MVA");
    if (m_cutPosition_MVA < 0) sc = 0;

    // Check that we got everything OK
    if (sc == 0){
      ATH_MSG_ERROR("ERROR: Something went wrong with the setup of the decision objects...");
      return StatusCode::FAILURE;
    }

  }
  else {  //Error if it cant find the conf
    ATH_MSG_ERROR("Could not find configuration file " << m_configFile);
    return StatusCode::FAILURE;
  }
  ///-----------End of text config----------------------------

  return StatusCode::SUCCESS;
}


//=============================================================================
// return the accept info object
//=============================================================================

const asg::AcceptInfo& AsgElectronSelectorTool::getAcceptInfo() const
{
  return m_acceptMVA;
}

//=============================================================================
// The main accept method: the actual cuts are applied here
//=============================================================================
asg::AcceptData AsgElectronSelectorTool::accept( const EventContext& ctx, const xAOD::Electron* eg, double mu ) const
{
  ATH_MSG_VERBOSE("\t AsgElectronSelectorTool::accept( &ctx, *eg, mu= "<<(&ctx)<<", "<<eg<<", "<<mu<<" )");

  // Setup return accept with AcceptInfo
  asg::AcceptData acceptData(&m_acceptMVA);

  if (!eg){
    throw std::runtime_error("AsgElectronSelectorTool: Failed, no electron object was passed");
  }

  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if (!cluster){
    ATH_MSG_DEBUG("exiting because cluster is NULL " << cluster);
    return acceptData;
  }

  if(!cluster->hasSampling(CaloSampling::CaloSample::EMB2) && !cluster->hasSampling(CaloSampling::CaloSample::EME2)){
    ATH_MSG_DEBUG("Failed, cluster is missing samplings EMB2 and EME2");
    return acceptData;
  }

  const double energy = cluster->e();
  const float eta = cluster->etaBE(2);

  if(isForwardElectron(eg, eta)){
    ATH_MSG_DEBUG("Failed, this is a forward electron! The AsgElectronSelectorTool is only suitable for central electrons!");
    return acceptData;
  }

  const xAOD::TrackParticle* track = eg->trackParticle();
  if (!track){
    ATH_MSG_DEBUG("exiting because track is NULL " << track);
    return acceptData;
  }

  // transverse energy of the electron (using the track eta)
  double et = (std::cosh(track->eta()) != 0.) ? energy / std::cosh(track->eta()) : 0.;

  // number of track hits
  uint8_t nSiHitsPlusDeadSensors(0);
  uint8_t nPixHitsPlusDeadSensors(0);
  bool passBLayerRequirement(false);
  uint8_t ambiguityBit(0);

  bool allFound = true;
  std::string notFoundList = "";

  // get the ambiguity type from the decoration
  if (eg->isAvailable<uint8_t>("ambiguityType")){
    static const SG::AuxElement::Accessor<uint8_t> acc("ambiguityType");
    ambiguityBit = acc(*eg);
  }
  else {
    allFound = false;
    notFoundList += "ambiguityType ";
  }

  nSiHitsPlusDeadSensors = ElectronSelectorHelpers::numberOfSiliconHitsAndDeadSensors(track);
  nPixHitsPlusDeadSensors = ElectronSelectorHelpers::numberOfPixelHitsAndDeadSensors(track);
  passBLayerRequirement = ElectronSelectorHelpers::passBLayerRequirement(track);

  // calculate the output of the selector tool
  double mvaScore = calculate(ctx, eg, mu);

  ATH_MSG_VERBOSE(Form("PassVars: MVA=%8.5f, eta=%8.5f, et=%8.5f, nSiHitsPlusDeadSensors=%i, nHitsPlusPixDeadSensors=%i, passBLayerRequirement=%i, ambiguityBit=%i, mu=%8.5f",
                       mvaScore, eta, et,
                       nSiHitsPlusDeadSensors, nPixHitsPlusDeadSensors,
                       passBLayerRequirement,
                       ambiguityBit, mu));

  if (!allFound){
    throw std::runtime_error("AsgElectronSelectorTool: Not all variables needed for the decision are found. The following variables are missing: " + notFoundList );
  }

  // Set up the individual cuts
  bool passKine(true);
  bool passNSilicon(true);
  bool passNPixel(true);
  bool passNBlayer(true);
  bool passAmbiguity(true);
  bool passMVA(true);

  if (std::abs(eta) > 2.47){
    ATH_MSG_DEBUG("This electron is fabs(eta)>2.47 Returning False.");
    passKine = false;
  }

  unsigned int etBin = getDiscEtBin(et);
  unsigned int etaBin = getDiscEtaBin(eta);

  // sanity
  if (etBin >= s_fnDiscEtBins){
    ATH_MSG_DEBUG("Cannot evaluate model for Et " << et << ". Returning false..");
    passKine = false;
  }

  // Return if the kinematic requirements are not fulfilled
  acceptData.setCutResult(m_cutPosition_kinematic, passKine);
  if (!passKine){return acceptData;}

  // ambiguity bit
  if (m_cutAmbiguity.size()){
    if (!ElectronSelectorHelpers::passAmbiguity((xAOD::AmbiguityTool::AmbiguityType)ambiguityBit, m_cutAmbiguity[etaBin])){
      ATH_MSG_DEBUG("MVA macro: ambiguity Bit Failed.");
      passAmbiguity = false;
    }
  }

  // blayer cut
  if (m_cutBL.size()) {
    if(m_cutBL[etaBin] == 1 && !passBLayerRequirement){
      ATH_MSG_DEBUG("MVA macro: Blayer cut failed.");
      passNBlayer = false;
    }
  }
  // pixel cut
  if (m_cutPi.size()){
    if (nPixHitsPlusDeadSensors < m_cutPi[etaBin]){
      ATH_MSG_DEBUG("MVA macro: Pixels Failed.");
      passNPixel = false;
    }
  }
  // SCT cut
  if (m_cutSCT.size()){
    if (nSiHitsPlusDeadSensors < m_cutSCT[etaBin]){
      ATH_MSG_DEBUG( "MVA macro: Silicon Failed.");
      passNSilicon = false;
    }
  }

  double cutDiscriminant;
  unsigned int ibin_combinedMVA = etBin*s_fnDiscEtaBins+etaBin; // Must change if number of eta bins changes!.

  if (m_cutSelector.size()){
    // To protect against a binning mismatch, which should never happen
    if (ibin_combinedMVA >= m_cutSelector.size()){
      throw std::runtime_error("AsgElectronSelectorTool: The desired eta/pt bin is outside of the range specified by the input. This should never happen! This indicates a mismatch between the binning in the configuration file and the tool implementation." );
    }
    if (m_doSmoothBinInterpolation){
      cutDiscriminant = interpolateCuts(m_cutSelector, et, eta);
    }
    else{
      cutDiscriminant = m_cutSelector.at(ibin_combinedMVA);
    }
    // Determine if the calculated mva score value passes the cut
    ATH_MSG_DEBUG("MVA macro: Discriminant: ");
    if (mvaScore < cutDiscriminant){
      ATH_MSG_DEBUG("MVA macro: Disciminant Cut Failed.");
      passMVA = false;
    }
  }

  // Set the individual cut bits in the return object
  acceptData.setCutResult(m_cutPosition_NSilicon, passNSilicon);
  acceptData.setCutResult(m_cutPosition_NPixel, passNPixel);
  acceptData.setCutResult(m_cutPosition_NBlayer, passNBlayer);
  acceptData.setCutResult(m_cutPosition_ambiguity, passAmbiguity);
  acceptData.setCutResult(m_cutPosition_MVA, passMVA);

  return acceptData;

}

//=============================================================================
// The main result method: the actual mvaScore is calculated here
//=============================================================================
double AsgElectronSelectorTool::calculate( const EventContext& ctx, const xAOD::Electron* eg, double mu ) const
{
  ATH_MSG_VERBOSE("\t AsgElectronSelectorTool::calculate( &ctx, *eg, mu= "<<(&ctx)<<", "<<eg<<", "<<mu<<" )");
  if (!eg){
    throw std::runtime_error("AsgElectronSelectorTool: Failed, no electron object was passed" );
  }

  const xAOD::CaloCluster* cluster = eg->caloCluster();
  if (!cluster){
    ATH_MSG_DEBUG("Failed, no cluster.");
    return -999.;
  }

  if (!cluster->hasSampling(CaloSampling::CaloSample::EMB2) && !cluster->hasSampling(CaloSampling::CaloSample::EME2)){
    ATH_MSG_DEBUG("Failed, cluster is missing samplings EMB2 and EME2.");
    return -999.;
  }

  const double energy = cluster->e();
  const float eta = cluster->etaBE(2);

  if (isForwardElectron(eg, eta)){
    ATH_MSG_DEBUG("Failed, this is a forward electron! The AsgElectronSelectorTool is only suitable for central electrons!");
    return -999.;
  }

  const xAOD::TrackParticle* track = eg->trackParticle();
  if (!track){
    ATH_MSG_DEBUG("Failed, no track.");
    return -999.;
  }

  // transverse energy of the electron (using the track eta)
  const double et = energy / std::cosh(track->eta());

  // Variables used in the ML model
  // track quantities
  uint8_t nSCTHitsPlusDeadSensors(0);
  uint8_t nPixHitsPlusDeadSensors(0);
  float d0(0.0), d0sigma(0.0), d0significance(0.0);
  float trackqoverp(0.0);
  double dPOverP(0.0);
  float TRT_PID(0.0);
  double trans_TRTPID(0.0);

  // Track Cluster matching
  float deltaEta1(0), deltaPhiRescaled2(0), EoverP(0);

  // Calorimeter
  float Reta(0), Rphi(0), Rhad1(0), Rhad(0), w2(0), f1(0), Eratio(0), f3(0), wtots1(0);

  bool allFound = true;
  std::string notFoundList = "";

  // retrieve track variables
  trackqoverp = track->qOverP();
  d0 = track->d0();
  float vard0 = track->definingParametersCovMatrix()(0, 0);
  if (vard0 > 0){
    d0sigma = std::sqrt(vard0);
  }
  d0significance = std::abs(d0 / d0sigma);

  if (!track->summaryValue(TRT_PID, xAOD::eProbabilityHT)){
    allFound = false;
    notFoundList += "eProbabilityHT ";
  }

  //Transform the TRT PID output for use in the ML tool.
  double tau = 15.0;
  double fEpsilon = 1.0e-30; // to avoid zero division
  double pid_tmp = TRT_PID;
  if (pid_tmp >= 1.0) pid_tmp = 1.0 - 1.0e-15; //this number comes from TMVA
  else if (pid_tmp <= fEpsilon) pid_tmp = fEpsilon;
  trans_TRTPID = -std::log(1.0 / pid_tmp - 1.0) * (1. / tau);

  // Change default value of TRT PID to 0.15 instead of 0 when there is no information from the TRT
  if ((std::abs(trans_TRTPID) < 1.0e-6) && (std::abs(eta) > 2.01)){
      trans_TRTPID = 0.15;
  }

  unsigned int index;
  if (track->indexOfParameterAtPosition(index, xAOD::LastMeasurement)){
    double refittedTrack_LMqoverp = track->charge() / std::sqrt(std::pow(track->parameterPX(index), 2) +
                                                                std::pow(track->parameterPY(index), 2) +
                                                                std::pow(track->parameterPZ(index), 2));

    dPOverP = 1 - trackqoverp / (refittedTrack_LMqoverp);
  }
  else {
    allFound = false;
    notFoundList += "deltaPoverP ";
  }

  EoverP =  energy * std::abs(trackqoverp);

  nPixHitsPlusDeadSensors = ElectronSelectorHelpers::numberOfPixelHitsAndDeadSensors(track);
  nSCTHitsPlusDeadSensors = ElectronSelectorHelpers::numberOfSCTHitsAndDeadSensors(track);

  // retrieve Calorimeter variables
  // reta = e237/e277
  if (!eg->showerShapeValue(Reta, xAOD::EgammaParameters::Reta)){
    allFound = false;
    notFoundList += "Reta ";
  }
  // rphi e233/e237
  if (!eg->showerShapeValue(Rphi, xAOD::EgammaParameters::Rphi)){
    allFound = false;
    notFoundList += "Rphi ";
  }
  // rhad1 = ethad1/et
  if (!eg->showerShapeValue(Rhad1, xAOD::EgammaParameters::Rhad1)){
    allFound = false;
    notFoundList += "Rhad1 ";
  }
  // rhad = ethad/et
  if (!eg->showerShapeValue(Rhad, xAOD::EgammaParameters::Rhad)){
    allFound = false;
    notFoundList += "Rhad ";
  }
  // shower width in 2nd sampling
  if (!eg->showerShapeValue(w2, xAOD::EgammaParameters::weta2)){
    allFound = false;
    notFoundList += "weta2 ";
  }
  // fraction of energy reconstructed in the 1st sampling
  if (!eg->showerShapeValue(f1, xAOD::EgammaParameters::f1)){
    allFound = false;
    notFoundList += "f1 ";
  }
  // E of 2nd max between max and min in strips
  if (!eg->showerShapeValue(Eratio, xAOD::EgammaParameters::Eratio)){
    allFound = false;
    notFoundList += "Eratio ";
  }
  // fraction of energy reconstructed in the 3rd sampling
  if (!eg->showerShapeValue(f3, xAOD::EgammaParameters::f3)){
    allFound = false;
    notFoundList += "f3 ";
  }

  // Set f3 to default value in eta region where it is poorly modelled
  if (std::abs(eta) > 2.01) {
    f3 = 0.05;
  }

  // Shower width in first sampling of the calorimeter
  if (!eg->showerShapeValue(wtots1, xAOD::EgammaParameters::wtots1)){
    allFound = false;
    notFoundList += "wtots1 ";
  }

  // retrieve Track Cluster matching variables
  // difference between cluster eta (sampling 1) and the eta of the track
  if (!eg->trackCaloMatchValue(deltaEta1, xAOD::EgammaParameters::deltaEta1)){
    allFound = false;
    notFoundList += "deltaEta1 ";
  }
  // difference between the cluster phi (sampling 2) and the phi of the track extrapolated from the last measurement point.
  if (!eg->trackCaloMatchValue(deltaPhiRescaled2, xAOD::EgammaParameters::deltaPhiRescaled2)){
    allFound = false;
    notFoundList += "deltaPhiRescaled2 ";
  }


  ATH_MSG_VERBOSE(Form("Vars: eta=%8.5f, et=%8.5f, f3=%8.5f, rHad==%8.5f, rHad1=%8.5f, Reta=%8.5f, w2=%8.5f, f1=%8.5f, Emaxs1=%8.5f, deltaEta1=%8.5f, d0=%8.5f, d0significance=%8.5f, Rphi=%8.5f, dPOverP=%8.5f, deltaPhiRescaled2=%8.5f, TRT_PID=%8.5f, trans_TRTPID=%8.5f, mu=%8.5f, wtots1=%8.5f, EoverP=%8.5f, nPixHitsPlusDeadSensors=%2df, nSCTHitsPlusDeadSensors=%2df",
                       eta, et, f3, Rhad, Rhad1, Reta,
                       w2, f1, Eratio,
                       deltaEta1, d0,
                       d0significance,
                       Rphi, dPOverP, deltaPhiRescaled2,
                       TRT_PID, trans_TRTPID,
                       mu,
                       wtots1, EoverP, int(nPixHitsPlusDeadSensors), int(nSCTHitsPlusDeadSensors)));

  if (!allFound){
    throw std::runtime_error("AsgElectronSelectorTool: Not all variables needed for MVA calculation are found. The following variables are missing: " + notFoundList );
  }


  MVAEnum::MVACalcVars vars;
  vars.eta = std::abs(eta);
  vars.et = et;
  vars.f3 = f3;
  vars.Rhad = Rhad;
  vars.Rhad1 = Rhad1;
  vars.Reta = Reta;
  vars.weta2 = w2;
  vars.f1 = f1;
  vars.Eratio = Eratio;
  vars.deltaEta1 = deltaEta1;
  vars.d0 = d0;
  vars.d0significance = d0significance;
  vars.Rphi = Rphi;
  vars.dPOverP = dPOverP;
  vars.deltaPhiRescaled2 = deltaPhiRescaled2;
  vars.trans_TRTPID = trans_TRTPID;
  vars.wtots1 = wtots1;
  vars.EoverP = EoverP;
  vars.nPixHitsPlusDeadSensors = nPixHitsPlusDeadSensors;
  vars.nSCTHitsPlusDeadSensors = nSCTHitsPlusDeadSensors;

  double mvaScore = m_mvaTool->calculate(vars);

  return transformMLOutput(mvaScore);

}

//=============================================================================
/// Get the name of the current operating point
//=============================================================================
std::string AsgElectronSelectorTool::getOperatingPointName() const
{
  return m_workingPoint;
}

//=============================================================================
asg::AcceptData AsgElectronSelectorTool::accept( const xAOD::IParticle* part ) const
{
  return accept(Gaudi::Hive::currentContext(), part);
}
asg::AcceptData AsgElectronSelectorTool::accept( const EventContext& ctx, const xAOD::IParticle* part ) const
{
  ATH_MSG_VERBOSE("\t AsgElectronSelectorTool::accept( &ctx, *part= "<<(&ctx)<<", "<<part<<" )");
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if (eg){
    return accept(eg);
  }
  else {
    ATH_MSG_DEBUG("AsgElectronSelectorTool::could not cast to const Electron");
    // Setup return accept with AcceptInfo
    asg::AcceptData acceptData(&m_acceptMVA);
    return acceptData;
  }
}

double AsgElectronSelectorTool::calculate( const xAOD::IParticle* part ) const
{
  return calculate(Gaudi::Hive::currentContext(), part);
}
double AsgElectronSelectorTool::calculate( const EventContext& ctx, const xAOD::IParticle* part ) const
{
  ATH_MSG_VERBOSE("\t AsgElectronSelectorTool::calculate( &ctx, *part"<<(&ctx)<<", "<<part<<" )");
  const xAOD::Electron* eg = dynamic_cast<const xAOD::Electron*>(part);
  if (eg){
    return calculate(eg);
  }
  else {
    ATH_MSG_DEBUG("AsgElectronSelectorTool::could not cast to const Electron");
    // Return a default value
    return -999.;
  }
}

asg::AcceptData AsgElectronSelectorTool::accept( const EventContext& ctx, const xAOD::Egamma* eg, double mu ) const
{
  ATH_MSG_VERBOSE("\t AsgElectronSelectorTool::accept( &ctx, *eg, mu= "<<(&ctx)<<", "<<eg<<", "<<mu<<" )");
  const xAOD::Electron* ele = dynamic_cast<const xAOD::Electron*>(eg);
  if (ele){
    return accept(ctx, ele, mu);
  }
  else {
    ATH_MSG_DEBUG("AsgElectronSelectorTool::could not cast to const Electron");
    // Setup return accept with AcceptInfo
    asg::AcceptData acceptData(&m_acceptMVA);
    return acceptData;
  }
}

double AsgElectronSelectorTool::calculate( const EventContext& ctx, const xAOD::Egamma* eg, double mu ) const
{
  ATH_MSG_VERBOSE("\t AsgElectronSelectorTool::calculate( &ctx, *eg, mu= "<<(&ctx)<<", "<<eg<<", "<<mu<<" )");
  const xAOD::Electron* ele = dynamic_cast<const xAOD::Electron*>(eg);
  if (ele){
    return calculate(ctx, ele, mu);
  }
  else {
    ATH_MSG_DEBUG("AsgElectronSelectorTool::could not cast to const Electron");
    return -999.;
  }
}


bool AsgElectronSelectorTool::isForwardElectron( const xAOD::Egamma* eg, const float eta ) const
{
  static const SG::AuxElement::ConstAccessor< uint16_t > accAuthor( "author" );

  if (accAuthor.isAvailable(*eg)){
    // cannot just do eg->author() because it isn't always filled
    // at trigger level
    if (accAuthor(*eg) == xAOD::EgammaParameters::AuthorFwdElectron){
      ATH_MSG_DEBUG("Failed, this is a forward electron! The AsgElectronSelectorTool is only suitable for central electrons!");
      return true;
    }
  }
  else{
    //Check for fwd via eta range the old logic
    if (std::abs(eta) > 2.5){
      ATH_MSG_DEBUG("Failed, cluster->etaBE(2) range due to " << eta << " seems like a fwd electron" );
      return true;
    }
  }

  return false;
}



double AsgElectronSelectorTool::transformMLOutput( double score ) const
{
  // returns transformed or non-transformed output
  constexpr double oneOverTau = 1. / 10;
  constexpr double fEpsilon = 1.0e-30; // to avoid zero division
  if (score >= 1.0) score = 1.0 - 1.0e-15; // this number comes from TMVA
  else if (score <= fEpsilon) score = fEpsilon;
  score = -std::log(1.0 / score - 1.0) * oneOverTau;
  ATH_MSG_DEBUG("score is " << score);
  return score;
}


// Gets the Discriminant Eta bin [0,s_fnDiscEtaBins-1] given the eta
unsigned int AsgElectronSelectorTool::getDiscEtaBin( double eta ) const
{
  const unsigned int nEtaBins = s_fnDiscEtaBins;
  const double etaBins[nEtaBins] = {0.1, 0.6, 0.8, 1.15, 1.37, 1.52, 1.81, 2.01, 2.37, 2.47};
  for (unsigned int etaBin = 0; etaBin < nEtaBins; ++etaBin){
    if (std::abs(eta) < etaBins[etaBin]) return etaBin;
  }
  return (nEtaBins-1);
}

// Gets the Discriminant Et bin (MeV) [0,s_fnDiscEtBins-1]
unsigned int AsgElectronSelectorTool::getDiscEtBin( double et ) const
{
  static const double GeV = 1000;
  const unsigned int nEtBins = s_fnDiscEtBins;
  const double etBins[nEtBins] = {7*GeV,10*GeV,15*GeV,20*GeV,25*GeV,30*GeV,35*GeV,40*GeV,45*GeV,6000*GeV};
  for (unsigned int etBin = 0; etBin < nEtBins; ++etBin){
    if (et < etBins[etBin]) return etBin;
  }
  return (nEtBins-1);
}


// Note that this will only perform the cut interpolation up to ~45 GeV, so
// no smoothing is done above this for the high ET LH binning yet
double AsgElectronSelectorTool::interpolateCuts( const std::vector<double>& cuts,double et,double eta ) const
{
  const int etbin = getDiscEtBin(et);
  const int etabin = getDiscEtaBin(eta);
  unsigned int ibin_combinedML = etbin*s_fnDiscEtaBins+etabin;
  double cut = cuts.at(ibin_combinedML);
  const double GeV = 1000;
  const double eTBins[10] = {5.5*GeV,8.5*GeV,12.5*GeV,17.5*GeV,22.5*GeV,27.5*GeV,32.5*GeV,37.5*GeV,42.5*GeV,47.5*GeV};

  if (et >= eTBins[9]) return cut; // no interpolation for electrons above 47.5 GeV
  if (et <= eTBins[0]) return cut; // no interpolation for electrons below 5.5 GeV

  // find the bin where the value is smaller than the next bin
  // Start with bin = 1, since it always has to be at least in
  // bin 1 because of previous cut
  int bin = 1;
  while ( et > eTBins[bin] ) bin++;

  double etLow = eTBins[bin-1];
  double etUp = eTBins[bin];
  double discLow = cuts.at((bin-1) * s_fnDiscEtaBins+etabin);
  double discUp = cuts.at((bin) * s_fnDiscEtaBins+etabin);

  double gradient = ( discUp - discLow ) / ( etUp - etLow );

  return discLow + (et - etLow) * gradient;
}
