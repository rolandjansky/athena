/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AthElectronEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in Athena
*/

// Include this class's header
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"
#include "PathResolver/PathResolver.h"

// STL includes
#include <string>
#include <cfloat>
#include <iostream>
#include <iomanip>
#include <iostream>
#include <limits.h>
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

// Include the return object
#include "PATCore/PATCoreEnums.h"
// xAOD includes
#include "xAODEgamma/Electron.h"
#include "xAODEventInfo/EventInfo.h"

#include <boost/algorithm/string.hpp>

#ifndef ROOTCORE
#include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

#include "xAODMetaData/FileMetaData.h"
#include "PathResolver/PathResolver.h"
#include "ElectronEfficiencyCorrection/TElectronEfficiencyCorrectionTool.h"


// ROOT includes
#include "TSystem.h"

namespace correlationModel{
  enum model { COMBMCTOYS =0,
	       MCTOYS=1,
	       FULL=2,
	       SIMPLIFIED=3,
	       TOTAL=4,
	       SYST=5
  };
}

// =============================================================================
// Standard constructor
// =============================================================================
AsgElectronEfficiencyCorrectionTool::AsgElectronEfficiencyCorrectionTool(std::string myname) :
  asg::AsgMetadataTool(myname),
  m_rootTool(0),
  m_affectedSys(),
  m_appliedSystematics(0),
  m_correlation_model(correlationModel::SIMPLIFIED),
  m_sysSubstring(""),
  m_dataType(PATCore::ParticleDataType::Full),
  m_nCorrSyst(0),
  m_nUncorrSyst(0),
  m_UncorrRegions(0),
  m_nSimpleUncorrSyst(0) {
  // Create an instance of the underlying ROOT tool
  m_rootTool = new Root::TElectronEfficiencyCorrectionTool(("T" + (this->name())).c_str());
  // Declare the needed properties
  declareProperty("CorrectionFileNameList", m_corrFileNameList,
                  "List of file names that store the correction factors for simulation.");
  declareProperty("MapFilePath", m_mapFile = "ElectronEfficiencyCorrection/2015_2016/rel20.7/Moriond_February2017_v2/map0.txt" ,
                  "Full path to the map file");
  declareProperty("RecoKey", m_recoKey = "" ,
                  "Key associated with reconstruction");
  declareProperty("IdKey", m_idKey = "" ,
                  "Key associated with identification working point");
  declareProperty("IsoKey", m_isoKey = "" ,
                  "Key associated with isolation working point");
  declareProperty("TriggerKey", m_trigKey = "" ,
                  "Key associated with trigger working point");

  declareProperty("ForceDataType", m_dataTypeOverwrite = -1,
		  "Force the DataType of the electron to specified value (to circumvent problem of incorrect DataType for forward electrons in some old releases)");

  declareProperty("ResultPrefix", m_resultPrefix = "", "The prefix string for the result");
  declareProperty("ResultName", m_resultName = "", "The string for the result");

  declareProperty("CorrelationModel", m_correlation_model_name = "SIMPLIFIED",
      "Uncertainty correlation model. At the moment TOTAL, FULL, SIMPLIFIED, SYST, MCTOYS and COMBMCTOYS are implemented. SIMPLIFIED is the default option.");
  declareProperty("NumberOfToys", m_number_of_toys = 100,"Number of ToyMC replica, affecting MCTOYS and COMBMCTOYS correlation models only.");
  declareProperty("MCToySeed", m_seed_toys = 0,"Seed for ToyMC replica, affecting MCTOYS and COMBMCTOYS correlation models only." );
  declareProperty("MCToyScale", m_scale_toys = 1,"Scales Toy systematics up by this factor, affecting MCTOYS and COMBMCTOYS correlation models only." );
  declareProperty("EventInfoCollectionName",  m_eventInfoCollectionName= "EventInfo", "The EventInfo Collection Name");
  declareProperty("UseRandomRunNumber",  m_useRandomRunNumber = true);
  declareProperty("DefaultRandomRunNumber",  m_defaultRandomRunNumber = 999999);
}

// =============================================================================
// Standard destructor
// =============================================================================
AsgElectronEfficiencyCorrectionTool::~AsgElectronEfficiencyCorrectionTool() {
  if (m_UncorrRegions) {
    delete m_UncorrRegions;
  }
  if (finalize().isFailure()) {
    ATH_MSG_ERROR("Failure in AsgElectronEfficiencyCorrectionTool finalize()");
  }
  delete m_rootTool;
}

// =============================================================================
// Athena initialize method
// =============================================================================
StatusCode
AsgElectronEfficiencyCorrectionTool::initialize() {
  // Forward the message level
  m_rootTool->msg().setLevel(this->msg().level());

  if (m_corrFileNameList.empty() &&  m_recoKey == ""  &&  m_idKey == "" && m_trigKey == "" && m_isoKey== "" ) {
    ATH_MSG_ERROR("CorrectionFileNameList as well as SFKeys are empty! Please configure it properly...");
    return StatusCode::FAILURE;
  }

  // The user wants to overwrite the m_dataType
  // Otherwise it will be full
  // When we will use metadata  m_dataType will be whatever the metadata says i.e Full or Fast
  // We will need to check for the existence of overwrite
  // and if overwrite is not set we will set the m_dataType based on metadata
  if (m_dataTypeOverwrite != -1) {
    if (m_dataTypeOverwrite != static_cast<int> (PATCore::ParticleDataType::Full)
	&& m_dataTypeOverwrite != static_cast<int> (PATCore::ParticleDataType::Fast)) {
      ATH_MSG_ERROR("Unsupported Particle Data Type Overwrite" << m_dataTypeOverwrite);
      return StatusCode::FAILURE;
    }
    m_dataType = static_cast<PATCore::ParticleDataType::DataType>(m_dataTypeOverwrite);
  }

  //Find the relevant input files
  //Fill the vector with filename using keys if the user
  //has not passed the full filename as a property
  if (m_corrFileNameList.size() == 0) {
    if(getFile(m_recoKey, m_idKey, m_isoKey, m_trigKey).isFailure()){
      ATH_MSG_ERROR("No Root file input specified, and not available map file");
      return StatusCode::FAILURE;
    }
  }
  // Resolve the paths to the input files for the full Geant4 simualtion corrections
  for (size_t i = 0; i < m_corrFileNameList.size(); ++i) {

    std::string filename = PathResolverFindCalibFile(m_corrFileNameList.at(i));
    if (filename.empty()) {
      ATH_MSG_ERROR("Could NOT resolve file name " << m_corrFileNameList.at(i));
      return StatusCode::FAILURE;
    }else {
      ATH_MSG_INFO(" Path found = " << filename);
    }
    m_rootTool->addFileName(filename);
    // Determine the systematics substring according to the name of the input file
    if (m_corrFileNameList.at(i).find("efficiencySF.") != std::string::npos) {
      m_sysSubstring = "Trigger_";
    }
    if (m_corrFileNameList.at(i).find("efficiencySF.offline") != std::string::npos) {
      m_sysSubstring = "ID_";
    }
    if (m_corrFileNameList.at(i).find("efficiencySF.offline.RecoTrk") != std::string::npos) {
      m_sysSubstring = "Reco_";
    }
    if (m_corrFileNameList.at(i).find("efficiencySF.Isolation") != std::string::npos) {
      m_sysSubstring = "Iso_";
    }
    if (m_corrFileNameList.at(i).find("efficiency.") != std::string::npos) {
      m_sysSubstring = "TriggerEff_";
    }
    if (m_corrFileNameList.at(i).find("efficiencySF.ChargeID") != std::string::npos) {
      m_sysSubstring = "ChargeIDSel_";
    }
    if (m_sysSubstring == "") {
      ATH_MSG_ERROR("Could NOT find systematics Substring file name " << m_sysSubstring);
      return StatusCode::FAILURE;
    }
  }
  //
  m_rootTool->setResultPrefix(m_resultPrefix);
  m_rootTool->setResultName(m_resultName);
  //
  //
  //Find the proper correlation Model
  if (m_correlation_model_name == "COMBMCTOYS"){
    m_correlation_model= correlationModel::COMBMCTOYS;
  }
  else if(m_correlation_model_name == "MCTOYS"){
    m_correlation_model= correlationModel::MCTOYS;
  }
  else if(m_correlation_model_name == "FULL"){
    m_correlation_model= correlationModel::FULL;
  }
  else if(m_correlation_model_name == "SIMPLIFIED"){
    m_correlation_model= correlationModel::SIMPLIFIED;
  }
  else if(m_correlation_model_name == "TOTAL"){
    m_correlation_model= correlationModel::TOTAL;
  }
  else if(m_correlation_model_name == "SYST"){
    m_correlation_model= correlationModel::SYST;
  }
  else{
    ATH_MSG_ERROR("Unknown correlation model " + m_correlation_model_name);
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("Correlation model: " + m_correlation_model_name << " Enum " << m_correlation_model);

  //
  //Finish the preaparation of the underlying tool
  //
  //
  //
  if (m_correlation_model == correlationModel::SIMPLIFIED) {
    // cretate uncorrelated eta/pt bins
    //
    std::vector<float> eta;
    eta.push_back(0);
    ///  eta.push_back(0.8);
    eta.push_back(1.37);
    // eta.push_back(2.01);
    eta.push_back(2.47);

    std::vector<float> pt;
    pt.push_back(4500);
    pt.push_back(7000);
    pt.push_back(10000);
    pt.push_back(15000);
    pt.push_back(20000);
    pt.push_back(25000);
    pt.push_back(30000);
    pt.push_back(60000);
    pt.push_back(80000);
    pt.push_back(13000000);
    m_UncorrRegions = new TH2F("UncorrRegions", "UncorrRegions", pt.size() - 1, &(pt[0]), eta.size() - 1, &(eta[0]));
    m_UncorrRegions->SetDirectory(0);
    // bins not entries here
    m_nSimpleUncorrSyst = (eta.size() - 1) * (pt.size() - 1);
  }
  //
  if (m_seed_toys != 0) {
    m_rootTool->setSeed(m_seed_toys);
  }
  //
  if (m_correlation_model == correlationModel::COMBMCTOYS) {
    m_rootTool->bookCombToyMCScaleFactors(m_number_of_toys);
  }
  //
  if (m_correlation_model == correlationModel::MCTOYS) {
    m_rootTool->bookToyMCScaleFactors(m_number_of_toys);
  }
  // We need to initialize the underlying ROOT TSelectorTool
  if (0 == m_rootTool->initialize()) {
    ATH_MSG_ERROR("Could not initialize the TElectronEfficiencyCorrectionTool!");
    return StatusCode::FAILURE;
  }
  //
  // Copy the now filled TResult to the dummy
  m_resultDummy = m_rootTool->getTResult();
  // get Nsyst
  m_nCorrSyst = m_rootTool->getNSyst();
  //
  if (m_correlation_model == correlationModel::FULL) {
    m_nUncorrSyst = m_rootTool->getNbins(m_pteta_bins);
  }
  //
  //
  //
  //Initialize the systematics
  if (InitSystematics() != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("(InitSystematics() != CP::SystematicCode::Ok)");
    return StatusCode::FAILURE;
  }
  // Add the recommended systematics to the registry
  if (registerSystematics() != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("(registerSystematics() != CP::SystematicCode::Ok)");
    return StatusCode::FAILURE;
  }
  // Configure for nominal systematics
  if (applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Could not configure for nominal settings");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

/// =============================================================================
// Athena finalize method
// =============================================================================
StatusCode
AsgElectronEfficiencyCorrectionTool::finalize() {
  if (!(m_rootTool->finalize())) {
    ATH_MSG_ERROR("Something went wrong at finalize!");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

CP::CorrectionCode
AsgElectronEfficiencyCorrectionTool::getEfficiencyScaleFactor(const xAOD::Electron &inputObject,
							      double &efficiencyScaleFactor) const {

  //Retrieve the proper random Run Number
  unsigned int runnumber = m_defaultRandomRunNumber;
  if (m_useRandomRunNumber) {
    const xAOD::EventInfo *eventInfo = evtStore()->retrieve< const xAOD::EventInfo> (m_eventInfoCollectionName);
    if (!eventInfo) {
      ATH_MSG_ERROR("Could not retrieve EventInfo object!");
      efficiencyScaleFactor = 1;
      return CP::CorrectionCode::Error;
    }
    static const SG::AuxElement::Accessor<unsigned int> randomrunnumber("RandomRunNumber");
    if (!randomrunnumber.isAvailable(*eventInfo)) {
      efficiencyScaleFactor = 1;
      ATH_MSG_WARNING(
          "Pileup tool not run before using ElectronEfficiencyTool! SFs do not reflect PU distribution in data");
      return CP::CorrectionCode::Error;
    }
    runnumber = randomrunnumber(*(eventInfo));
  }
  //
  //Get the result 
  double cluster_eta(-9999.9);
  double et(0.0);


  et = inputObject.pt();
  const xAOD::CaloCluster *cluster = inputObject.caloCluster();
  if (cluster) {
    cluster_eta = cluster->etaBE(2);
  }

  const Root::TResult& result = calculate(cluster_eta,et, runnumber);
  efficiencyScaleFactor = result.getScaleFactor();
  //
  // The default of the underlying tool is -999 , if we are in a valid range
  // Reset it to 1 fow now to keep old behaviour
  if (efficiencyScaleFactor <= -999.0) {
    efficiencyScaleFactor = 1;
    return CP::CorrectionCode::OutOfValidityRange;
  }
  if (appliedSystematics().empty()) {
    return CP::CorrectionCode::Ok;
  }
  // ==============================================================================//
  //Systematic Variations
  //We pass only one variation per time
  // The applied systemetic is always one
  // Either is relevant and acquires a values
  // or stays 0.
  double sys(0);
  // First the Toys
  if (m_correlation_model == correlationModel::MCTOYS || m_correlation_model == correlationModel::COMBMCTOYS) {
    if (m_correlation_model == correlationModel::MCTOYS) {
      auto toy = appliedSystematics().getToyVariationByBaseName("EL_EFF_" + m_sysSubstring + "MCTOY");
      toy.second = m_scale_toys;
      sys = result.getResult((m_rootTool->getFirstToyMCPosition() - 1 + toy.first) * m_scale_toys);
    }
    else if (m_correlation_model == correlationModel::COMBMCTOYS) {
      auto toy = appliedSystematics().getToyVariationByBaseName("EL_EFF_" + m_sysSubstring + "COMBMCTOY");
      toy.second = m_scale_toys;
      sys = result.getResult((m_rootTool->getFirstToyMCPosition() - 1 + toy.first) * m_scale_toys);
    }
    // return here for Toy variations
    efficiencyScaleFactor = sys;
    return CP::CorrectionCode::Ok;
  }
  //The rest of the models
  //
  //Helper function
  //
  //Calculate and return at the spot
  auto func = [](double &sf, const double input) {
    sf = sf + input;
    return CP::CorrectionCode::Ok;
  };
  //
  //
  if (m_correlation_model == correlationModel::TOTAL) { // one "TOTAL" uncertainty
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
                                                                     m_correlation_model_name + "_" +
                                                                     "1NPCOR_PLUS_UNCOR" ,1))) {
      sys = result.getTotalUncertainty();
      func(efficiencyScaleFactor, sys);
    }
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
                                                                     m_correlation_model_name + "_" +
                                                                     "1NPCOR_PLUS_UNCOR" ,-1))) {
      sys =  -1*result.getTotalUncertainty();
      func(efficiencyScaleFactor, sys);
    }
  }
  //If there are not correlated systematic
  if (m_nCorrSyst == 0) {
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + "CorrUncertainty",1))) {

      sys = sqrt(result.getTotalUncertainty() * result.getTotalUncertainty()
		 - result.getResult(4) * result.getResult(4)); // total -stat
      func(efficiencyScaleFactor, sys);
    }
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + "CorrUncertainty" ,-1))) {

      sys = -1* sqrt(result.getTotalUncertainty() * result.getTotalUncertainty()
		     - result.getResult(4) * result.getResult(4)); // total -stat
      func(efficiencyScaleFactor, sys);
    }
  }else if (m_correlation_model == correlationModel::TOTAL) { // one "TOTAL" uncertainty
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
                                                                     m_correlation_model_name + "_" +
                                                                     "1NPCOR_PLUS_UNCOR" ,1))) {
      sys = result.getTotalUncertainty();
      func(efficiencyScaleFactor, sys);
    }
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
                                                                     m_correlation_model_name + "_" +
                                                                     "1NPCOR_PLUS_UNCOR" ,-1))) {
      sys =  -1*result.getTotalUncertainty();
      func(efficiencyScaleFactor, sys);
    }
  }


  // =======================================================================
  // Then the uncorrelated, we just need to see if the applied matches the current electron pt and eta
  if (m_correlation_model == correlationModel::FULL) {// The Full Model

    int currentUncorrSystReg = currentUncorrSystRegion( cluster_eta, et);

    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
								     m_correlation_model_name + "_" +
								     Form("UncorrUncertaintyNP%d",
									  currentUncorrSystReg),1))) {
      sys = result.getResult(4);//
      func(efficiencyScaleFactor, sys);
    }
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
								     m_correlation_model_name + "_" +
								     Form("UncorrUncertaintyNP%d",
									  currentUncorrSystReg),-1))) {
      sys = -1*result.getResult(4);//
      func(efficiencyScaleFactor, sys);
    }
  }
  else if (m_correlation_model == correlationModel::SIMPLIFIED) {

    int currentSimplifiedUncorrSystReg = currentSimplifiedUncorrSystRegion( cluster_eta, et);

    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
								     m_correlation_model_name + "_" +
								     Form("UncorrUncertaintyNP%d",
									  currentSimplifiedUncorrSystReg),1))) {
      sys = result.getResult(4);//
      func(efficiencyScaleFactor, sys);
    }

    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
								     m_correlation_model_name + "_" +
								     Form("UncorrUncertaintyNP%d",
									  currentSimplifiedUncorrSystReg),-1))) {
      sys = -1*result.getResult(4);//
      func(efficiencyScaleFactor, sys);
    }
  }

  //If it has not returned so far , it means we wants to do the correlated for the full models
  for (int i = 0; i < m_nCorrSyst; ++i) {/// number of correlated sources
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
								     Form("CorrUncertaintyNP%d", i),1))) {
      sys = result.getResult(5 + i);
      func(efficiencyScaleFactor, sys);
    }
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
								     Form("CorrUncertaintyNP%d", i),-1))) {
      sys = -1* result.getResult(5 + i);
      func(efficiencyScaleFactor, sys);
    }
  }
  return CP::CorrectionCode::Ok;

}

CP::CorrectionCode
AsgElectronEfficiencyCorrectionTool::applyEfficiencyScaleFactor(const xAOD::Electron &inputObject) const {
  double efficiencyScaleFactor = 1.0;
  CP::CorrectionCode result = getEfficiencyScaleFactor(inputObject, efficiencyScaleFactor);
  const static SG::AuxElement::Decorator<float> dec(m_resultPrefix + m_resultName + "SF");

  dec(inputObject) = efficiencyScaleFactor;
  return result;
}

// =======================================================================
//   Systematics Interface
// =======================================================================
bool
AsgElectronEfficiencyCorrectionTool::isAffectedBySystematic(const CP::SystematicVariation &systematic) const {

  if (systematic.empty()) {
    return false;
  }

  CP::SystematicSet sys = affectingSystematics();

  if (m_correlation_model == correlationModel::MCTOYS
     || m_correlation_model == correlationModel::COMBMCTOYS ){
    return(sys.begin()->ensembleContains(systematic)) ;
  }
  else{
    return (sys.find(systematic) != sys.end());
  }
}

/// returns: the list of all systematics this tool can be affected by
CP::SystematicSet
AsgElectronEfficiencyCorrectionTool::affectingSystematics() const {
  return m_affectedSys;
}

// Register the systematics with the registry and add them to the recommended list
CP::SystematicCode
AsgElectronEfficiencyCorrectionTool::registerSystematics() {
  CP::SystematicRegistry &registry = CP::SystematicRegistry::getInstance();

  if (registry.registerSystematics(*this) != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Failed to add systematic to list of recommended systematics.");
    return CP::SystematicCode::Unsupported;
  }
  return CP::SystematicCode::Ok;
}

/// returns: the list of all systematics this tool recommends to use
CP::SystematicSet
AsgElectronEfficiencyCorrectionTool::recommendedSystematics() const {
  return affectingSystematics();
}

/// Apply one variation at a time
CP::SystematicCode
AsgElectronEfficiencyCorrectionTool::applySystematicVariation(const CP::SystematicSet &systConfig) {
  // First, check if this configuration exists in the filtered map/registy
  auto itr = m_systFilter.find(systConfig);

  if (itr != m_systFilter.end()) {
    CP::SystematicSet &mySysConf = itr->second;
    m_appliedSystematics = &mySysConf;
  }
  // if not , we should register it, after it passes sanity checks
  else {
    // If it's a new input set, we need to filter it
    CP::SystematicSet affectingSys = affectingSystematics();
    CP::SystematicSet filteredSys;
    if (!CP::SystematicSet::filterForAffectingSystematics(systConfig, affectingSys, filteredSys)) {
      ATH_MSG_ERROR("Unsupported combination of systematic variations passed to the tool!");
      return CP::SystematicCode::Unsupported;
    }
    // Does filtered make sense ,  only one per time
    if (filteredSys.size() > 1) {
      ATH_MSG_ERROR("More than one systematic variation passed at the same time");
      return CP::SystematicCode::Unsupported;
    }

    if (filteredSys.size() == 0 && systConfig.size() > 0) {
      ATH_MSG_DEBUG("systematics : ");
      for (auto &syst : systConfig) {
	ATH_MSG_DEBUG(syst.name());
      }
      ATH_MSG_DEBUG(" Not supported ");
    }

    // insert to the registy
    itr = m_systFilter.insert(std::make_pair(systConfig, filteredSys)).first;
    // And return directly
    CP::SystematicSet &mySysConf = itr->second;
    m_appliedSystematics = &mySysConf;
  }
  return CP::SystematicCode::Ok;
}

// =============================================================================
// private methods
// The main Result  method:
// TRANSFER RESULT OF UNDERLYING TOOL TO xAOD TOOL
// =============================================================================
const Root::TResult& AsgElectronEfficiencyCorrectionTool::calculate(const double cluster_eta, const double et, const unsigned int runnumber) const {

  // Call the ROOT tool to get an answer
  return m_rootTool->calculate(m_dataType,
			       runnumber,
			       cluster_eta,
			       et /* in MeV */
			       );
}
/// returns: initialized the list for a specific model

CP::SystematicCode
AsgElectronEfficiencyCorrectionTool::InitSystematics() {

  // Correlated
  if (m_correlation_model == correlationModel::COMBMCTOYS) {
    m_affectedSys.insert((CP::SystematicVariation::makeToyEnsemble("EL_EFF_" + m_sysSubstring + "COMBMCTOY")));
  }else if (m_correlation_model == correlationModel::MCTOYS) {
    m_affectedSys.insert((CP::SystematicVariation::makeToyEnsemble("EL_EFF_" + m_sysSubstring + "MCTOY")));

  }else if (m_nCorrSyst == 0) {
    m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + "CorrUncertainty", 1));
    m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + "CorrUncertainty", -1));

  }else if (m_correlation_model != correlationModel::TOTAL) {
    if ( m_nCorrSyst == 0 ) {
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + "CorrUncertainty", 1));
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + "CorrUncertainty", -1));
    } else for (int i = 0; i < m_nCorrSyst; ++i) {
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + Form("CorrUncertaintyNP%d", i), 1));
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + Form("CorrUncertaintyNP%d", i), -1));
    }
  }
  // Different tratement for the uncorrelated
  if (m_correlation_model == correlationModel::TOTAL) {
    m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
						 "1NPCOR_PLUS_UNCOR", 1));
    m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
                                                 "1NPCOR_PLUS_UNCOR", -1));
  }else if (m_correlation_model == correlationModel::FULL) {
    for (int i = 0; i < m_nUncorrSyst; ++i) {
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
						   Form("UncorrUncertaintyNP%d", i), 1));
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
						   Form("UncorrUncertaintyNP%d", i), -1));
    }
  }else if (m_correlation_model == correlationModel::SIMPLIFIED) {
    for (int i = 0; i < m_nSimpleUncorrSyst; ++i) {
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
						   Form("UncorrUncertaintyNP%d", i), 1));
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
						   Form("UncorrUncertaintyNP%d", i), -1));
    }
  }
  return CP::SystematicCode::Ok;
}

//===============================================================================
// begin input file
//===============================================================================
StatusCode AsgElectronEfficiencyCorrectionTool::beginInputFile(){

  // User preference of dataType, already done in initialize
  if (m_dataTypeOverwrite != -1) return StatusCode::SUCCESS;

  PATCore::ParticleDataType::DataType dataType_metadata;
  const StatusCode status = get_simType_from_metadata(dataType_metadata);

  if (status == StatusCode::SUCCESS) {
    //m_metadata_retrieved isn't useful (might remove it later)
    m_metadata_retrieved = true;
    ATH_MSG_DEBUG("metadata from new file: " << (dataType_metadata == PATCore::ParticleDataType::Data ? "data" : (dataType_metadata == PATCore::ParticleDataType::Full ? "full simulation" : "fast simulation")));

    if (dataType_metadata != PATCore::ParticleDataType::Data) {

      if (m_dataTypeOverwrite == -1) { m_dataType = dataType_metadata; }
      else {ATH_MSG_DEBUG("Use should set the dataType, otherwise it will take FullSim Type");}
    }
  }

  else { // not able to retrieve metadata
    m_metadata_retrieved = false;
    ATH_MSG_DEBUG("not able to retrieve metadata, please set the dataType");
  }

  return StatusCode::SUCCESS;
}
//===============================================================================
// end input file
//===============================================================================
StatusCode AsgElectronEfficiencyCorrectionTool::endInputFile(){
  m_metadata_retrieved = false;
  return StatusCode::SUCCESS;

}
//===============================================================================
// end input file
//===============================================================================
StatusCode AsgElectronEfficiencyCorrectionTool::beginEvent(){

  return StatusCode::SUCCESS;
}

//===============================================================================
// Get Simulation flavor (FastSim or FullSim) from METADATA
//===============================================================================
StatusCode
AsgElectronEfficiencyCorrectionTool::get_simType_from_metadata(PATCore::ParticleDataType::DataType& result) const
{
  // adapted from https://svnweb.cern.ch/trac/atlasoff/browser/PhysicsAnalysis/AnalysisCommon/CPAnalysisExamples/trunk/Root/MetadataToolExample.cxx

#ifndef ROOTCORE
  //Determine MC/Data
  std::string dataType("");
  if ( (AthAnalysisHelper::retrieveMetadata("/TagInfo", "project_name", dataType, inputMetaStore())).isFailure() ){
    ATH_MSG_DEBUG("Failure to retrieve the project_name, Either running on data or something is wrong?");
  }
  if (!(dataType == "IS_SIMULATION")) {
    result = PATCore::ParticleDataType::Data;
    ATH_MSG_DEBUG("Running on simulation");
    return StatusCode::SUCCESS;
  }

  // Determine Fast/FullSim
  if (dataType == "IS_SIMULATION") {
    std::string simType("");
    ATH_CHECK(AthAnalysisHelper::retrieveMetadata("/Simulation/Parameters", "SimulationFlavour", simType, inputMetaStore()));
    boost::to_upper(simType);
    result = (simType.find("ATLFASTII")==std::string::npos) ?  PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Fast;
    return StatusCode::SUCCESS;
  }
#endif

  //Here's how things will work dual use, when file metadata is available in files
  if (inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
    const xAOD::FileMetaData* fmd = 0;
    ATH_CHECK(inputMetaStore()->retrieve(fmd, "FileMetaData"));

    std::string simType("");
    const bool s = fmd->value(xAOD::FileMetaData::simFlavour, simType);
    if (!s) {
      ATH_MSG_DEBUG("no sim flavour from metadata: must be data");
      result = PATCore::ParticleDataType::Data;
      return StatusCode::SUCCESS;
    }
    else {
      ATH_MSG_DEBUG("sim type = " + simType);
      boost::to_upper(simType);
      result = (simType.find("ATLFASTII")==std::string::npos) ?  PATCore::ParticleDataType::Full : PATCore::ParticleDataType::Fast;
      return StatusCode::SUCCESS;
    }
  }
  else {  // no metadata in the file
    ATH_MSG_DEBUG("no metadata found in the file");
    return StatusCode::FAILURE;
  }
}

//===============================================================================
// Map Key Feature
//===============================================================================
// Gets the correction filename from map
StatusCode
AsgElectronEfficiencyCorrectionTool::getFile(const std::string& recokey, const std::string& idkey, const std::string& isokey, const std::string& trigkey) {

  std::string key = convertToOneKey(recokey, idkey, isokey, trigkey);
  std::string mapFileName = PathResolverFindCalibFile(m_mapFile);
  std::string value = getValueByKey(mapFileName, key);

  if (value != "") {
    m_corrFileNameList.push_back(value);
  } else {
    if (mapFileName == "") {
      ATH_MSG_ERROR("Map file does not exist, Please set the path and version properly..");
    }
    else {
      ATH_MSG_ERROR("Key does not exist in the map file, Please configure it properly..");
    }
    return StatusCode::FAILURE;
  }

  ATH_MSG_DEBUG("Full File Name is " + value);
  return StatusCode::SUCCESS;
}

// Convert reco, ID, iso and trigger key values into a
// single key according to the map file key format
std::string
AsgElectronEfficiencyCorrectionTool::convertToOneKey(const std::string& recokey, const std::string& idkey, const std::string& isokey, const std::string& trigkey) const {

  std::string key;

  // Reconstruction Key
  if (recokey != ""){ key = recokey; }

  // Identification Key
  if (idkey != "" && (recokey == "" && isokey == "" && trigkey == "")){ key = idkey; }

  // Isolation Key
  if ((idkey != "" && isokey != "") && recokey == "" && trigkey == ""){ key = std::string(idkey + "_" + isokey); }

  // Trigger Key
  if (trigkey != "" && idkey != "") {

    // Trigger SF file with isolation
    if (isokey != "") {
      key = std::string (trigkey + "_" + idkey + "_" + isokey);
    } else {
      // Trigger SF file without isolation
      key = std::string(trigkey + "_" + idkey);
    }
  }
  ATH_MSG_DEBUG("Full Key is " + key);
  return key;
}

// Retrieves the value from the provided map file as
// associated with the provided key
std::string
AsgElectronEfficiencyCorrectionTool::getValueByKey(const std::string& mapFile, const std::string& key) {

  std::string value;
  if (read(mapFile).isFailure()) {
    ATH_MSG_ERROR("Couldn't read Map File" + mapFile);
    return "" ;
  }
  if (getValue(key, value) == "") {
    ATH_MSG_DEBUG("Error(" + key + ") not found ");
    return "";
  } else {
    ATH_MSG_DEBUG("Full Path of the correction file is " + value);
    return value;
  }
}
// Reads the provided map file
// and construct the map
StatusCode
AsgElectronEfficiencyCorrectionTool::read(const std::string& strFile) {

  std::ifstream is(strFile.c_str());
  if (!is.is_open()){
    ATH_MSG_ERROR("Couldn't read Map File" + strFile);
    return StatusCode::FAILURE;
  }
  while (!is.eof()) {
    std::string strLine;
    getline(is,strLine);

    int nPos = strLine.find('=');

    if ((signed int)std::string::npos == nPos) continue; // no '=', invalid line;
    std::string strKey = strLine.substr(0,nPos);
    std::string strVal = strLine.substr(nPos + 1, strLine.length() - nPos + 1);
    m_map.insert(std::map<std::string,std::string>::value_type(strKey,strVal));
  }
  return StatusCode::SUCCESS;
}
// Retrieves the value from the map file if
// the provided key is found. If the key has an
// association then, the actual retrieved value would
// be assigned to the 2nd argument of this method
std::string
AsgElectronEfficiencyCorrectionTool::getValue(const std::string& strKey, std::string& strValue) {

  std::map<std::string,std::string>::const_iterator i;
  i = m_map.find(strKey);

  if (i != m_map.end()) {
    strValue = i->second;
    return strValue;
  }
  return "";
}

int AsgElectronEfficiencyCorrectionTool::currentSimplifiedUncorrSystRegion(const double cluster_eta, const double et) const {
  int ptbin = m_UncorrRegions->GetXaxis()->FindBin(et) - 1;
  int etabin = m_UncorrRegions->GetYaxis()->FindBin(fabs(cluster_eta)) - 1;
  int reg = ((etabin) * m_UncorrRegions->GetNbinsX() + ptbin);
  return reg;
}



int AsgElectronEfficiencyCorrectionTool::currentUncorrSystRegion(const double cluster_eta, const double et) const {
  int etabin = -1;
  int reg = 0; 
  bool found = false;
  float cluster_eta_electron = 0;
  std::map<float, std::vector<float> >::const_iterator itr_ptBEGIN = m_pteta_bins.begin();
  std::map<float, std::vector<float> >::const_iterator itr_ptEND = m_pteta_bins.end();
  // Consider using std::map::lower_bound, returns the iterator to the first element that is greater-or-equal to a pt
  for (; itr_ptBEGIN != itr_ptEND; itr_ptBEGIN++) {
    std::map<float, std::vector<float> >::const_iterator itr_ptBEGINplusOne = itr_ptBEGIN;
    itr_ptBEGINplusOne++;

    if ((et > itr_ptBEGIN->first && itr_ptBEGINplusOne == itr_ptEND) ||
	(et > itr_ptBEGIN->first && et <= itr_ptBEGINplusOne->first)) {// find the pt bin
      etabin=0;
      // if it is ordered in eta from smaller to larger ascending order
      // consider using std::lower_bound(begin,end) to find the position?
      if ((itr_ptBEGIN->second).at(0) >= 0) {
	cluster_eta_electron = fabs(cluster_eta);
      }else {
	cluster_eta_electron = (cluster_eta);
      };
      for (unsigned int etab = 0; etab < ((itr_ptBEGIN->second).size() - 1); ++etab) {// find the eta bin
	etabin++;       
	if ((cluster_eta_electron) > (itr_ptBEGIN->second).at(etab) &&
	    (cluster_eta_electron) <= (itr_ptBEGIN->second).at(etab + 1)) {
	  found = true;
	  break;
	} // if ( (cluster_eta_electron)
      } // for (unsigned int etab=0;etab<((itr_ptBEGIN->second).size()-1) ; ++etab)
    } 
    if (found) {
      break;
    } 
    reg = reg + (itr_ptBEGIN->second).size();
  } 
  reg = reg + etabin;
  return reg;
}

int AsgElectronEfficiencyCorrectionTool::systUncorrVariationIndex( const xAOD::Electron &inputObject) const{
  int currentSystRegion=-999;
  double cluster_eta(-9999.9);
  double et(0.0);
 
  et = inputObject.pt();
  const xAOD::CaloCluster *cluster = inputObject.caloCluster();
  if (cluster) {
    cluster_eta = cluster->etaBE(2);
  }



  if (m_correlation_model == correlationModel::SIMPLIFIED) {
    currentSystRegion = currentSimplifiedUncorrSystRegion( cluster_eta, et);
  }

  if (m_correlation_model == correlationModel::FULL) {
    currentSystRegion = currentUncorrSystRegion( cluster_eta, et);
  }

  return currentSystRegion;
}

