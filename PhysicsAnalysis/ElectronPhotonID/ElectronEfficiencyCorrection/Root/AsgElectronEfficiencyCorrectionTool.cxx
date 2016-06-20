/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
   @class AthElectronEfficiencyCorrectionTool
   @brief Calculate the egamma scale factors in Athena

   @author Rob Roy Fletcher <rob.fletcher@cern.ch>, Karsten Koeneke
   @date   May 2014
*/

// Include this class's header
#include "ElectronEfficiencyCorrection/AsgElectronEfficiencyCorrectionTool.h"

// STL includes
#include <string>
#include <cfloat>
// #include <climits>
#include <iomanip>
#include <iostream>
#include <limits.h>

// Include the return object
#include "PATCore/TResult.h"
#include "PATCore/PATCoreEnums.h"

// xAOD includes
#include "xAODEgamma/Electron.h"
#include "xAODEventInfo/EventInfo.h"
#include "PathResolver/PathResolver.h"


// ROOT includes
#include "TSystem.h"



// =============================================================================
// Standard constructor
// =============================================================================
AsgElectronEfficiencyCorrectionTool::AsgElectronEfficiencyCorrectionTool(std::string myname) :
  AsgTool(myname),
  m_rootTool(0),
  m_affectedSys(),
  m_appliedSystematics(0),
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
  if (m_corrFileNameList.empty()) {
    ATH_MSG_ERROR("CorrectionFileNameList is empty! Please configure it properly...");
    return StatusCode::FAILURE;
  }

  // The user wants to overwrite the m_dataType
  // Otherwise it will be full
  // When we will use metadata  m_dataType will be whatever the metadata says i.e Full or Fast
  // And we will need to check for the existence of overwrite also there so as it not get overwritten again.
  if (m_dataTypeOverwrite != -1) {
    if (m_dataTypeOverwrite != static_cast<int> (PATCore::ParticleDataType::Full)
        && m_dataTypeOverwrite != static_cast<int> (PATCore::ParticleDataType::Fast)) {
      ATH_MSG_ERROR("Unsupported Particle Data Type Overwrite" << m_dataTypeOverwrite);
      return StatusCode::FAILURE;
    }
    m_dataType = static_cast<PATCore::ParticleDataType::DataType>(m_dataTypeOverwrite);
  }

  // Resolve the paths to the input files for the full Geant4 simualtion corrections
  for (unsigned int i = 0; i < m_corrFileNameList.size(); ++i) {
    std::string filename = PathResolverFindCalibFile(m_corrFileNameList.at(i));
    if (filename.empty()) {
      ATH_MSG_ERROR("Could NOT resolve file name " << m_corrFileNameList.at(i));
      return StatusCode::FAILURE;
    }else {
      ATH_MSG_INFO(" Path found = " << filename);
    }
    m_corrFileNameList.at(i) = filename;
  }

  // Configure the underlying Root tool
  for (unsigned int i = 0; i < m_corrFileNameList.size(); ++i) {
    m_rootTool->addFileName(m_corrFileNameList[i]);

    // Determine the systematics substring according to the name of the input file
    if (m_corrFileNameList[i].find("efficiencySF.") != std::string::npos) {
      m_sysSubstring = "Trigger_";
    }
    if (m_corrFileNameList[i].find("efficiencySF.offline") != std::string::npos) {
      m_sysSubstring = "ID_";
    }
    if (m_corrFileNameList[i].find("efficiencySF.offline.RecoTrk") != std::string::npos) {
      m_sysSubstring = "Reco_";
    }
    if (m_corrFileNameList[i].find("efficiencySF.Isolation") != std::string::npos) {
      m_sysSubstring = "Iso_";
    }
    if (m_corrFileNameList[i].find("efficiency.") != std::string::npos) {
      m_sysSubstring = "TriggerEff_";
    }
    if (m_sysSubstring == "") {
      return StatusCode::FAILURE;
    }
  }

  m_rootTool->setResultPrefix(m_resultPrefix);
  m_rootTool->setResultName(m_resultName);

  if (m_correlation_model_name != "COMBMCTOYS"
      && m_correlation_model_name != "MCTOYS"
      && m_correlation_model_name != "FULL"
      && m_correlation_model_name != "SIMPLIFIED"
      && m_correlation_model_name != "TOTAL"
      && m_correlation_model_name != "SYST"
      ) {
    ATH_MSG_ERROR("Unknown correlation model " + m_correlation_model_name);
    return StatusCode::FAILURE;
  }

  ATH_MSG_INFO("Correlation model: " + m_correlation_model_name);

  if (m_correlation_model_name == "SIMPLIFIED") {
    // cretate uncorrelated eta/pt bins
    //
    std::vector<float> eta;
    eta.push_back(0);
    eta.push_back(1.37);
    eta.push_back(2.47);
    std::vector<float> pt;
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

  if (m_seed_toys != 0) {
    m_rootTool->setSeed(m_seed_toys);
  }

  if (m_correlation_model_name == "COMBMCTOYS") {
    m_rootTool->bookCombToyMCScaleFactors(m_number_of_toys);
  }
  if (m_correlation_model_name == "MCTOYS") {
    m_rootTool->bookToyMCScaleFactors(m_number_of_toys);
  }

  // We need to initialize the underlying ROOT TSelectorTool
  if (0 == m_rootTool->initialize()) {
    ATH_MSG_ERROR("Could not initialize the TElectronEfficiencyCorrectionTool!");
    return StatusCode::FAILURE;
  }
  // Copy the now filled TResult to the dummy
  m_resultDummy = m_rootTool->getTResult();
  // get Nsyst
  m_nCorrSyst = m_rootTool->getNSyst();

  if (m_correlation_model_name == "FULL") {
    m_nUncorrSyst = m_rootTool->getNbins(m_pteta_bins);
  }

  // Add the recommended systematics to the registry
  if (registerSystematics() != CP::SystematicCode::Ok) {
    return StatusCode::FAILURE;
  }

  if (InitSystematics() != CP::SystematicCode::Ok) {
    return StatusCode::FAILURE;
  }
  // Add the recommended systematics to the registry
  if (registerSystematics() != CP::SystematicCode::Ok) {
    return StatusCode::FAILURE;
  }
  // Configure for nominal systematics
  if (applySystematicVariation(CP::SystematicSet()) != CP::SystematicCode::Ok) {
    ATH_MSG_ERROR("Could not configure for nominal settings");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

// =============================================================================
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
  int currentUncorrSystRegion = 0;
  int currentSimplifiedUncorrSystRegion = 0;

  unsigned int runnumber = m_defaultRandomRunNumber;

  if (m_useRandomRunNumber) {
    const xAOD::EventInfo *eventInfo = evtStore()->retrieve< const xAOD::EventInfo> (m_eventInfoCollectionName);
    if (!eventInfo) {
      ATH_MSG_ERROR("Could not retrieve EventInfo object!");
      efficiencyScaleFactor = 1;
      return CP::CorrectionCode::Error;
    }

    static SG::AuxElement::Decorator<unsigned int> randomrunnumber("RandomRunNumber");
    if (!randomrunnumber.isAvailable(*eventInfo)) {
      efficiencyScaleFactor = 1;
      ATH_MSG_WARNING(
		      "Pileup tool not run before using ElectronEfficiencyTool! SFs do not reflect PU distribution in data");
      return CP::CorrectionCode::Error;
    }
    runnumber = randomrunnumber(*(eventInfo));
  }
  const Root::TResult m_result = calculate(inputObject, runnumber, currentSimplifiedUncorrSystRegion,
                                           currentUncorrSystRegion);
  efficiencyScaleFactor = m_result.getScaleFactor();

  // The default of the underlying tool is -999 , if we are in a valid range
  // Reset it to 1 fow now to keep old behaviour
  if (efficiencyScaleFactor <= -999.0) {
    efficiencyScaleFactor = 1;
    return CP::CorrectionCode::OutOfValidityRange;
  }

  if (appliedSystematics().empty()) {
    return CP::CorrectionCode::Ok;
  }
  // The applied systemetic is always one
  // Either is relevant and acquires a values
  // or stays 0.
  double sys(0);
  // ==============================================================================//
  // First the Toys
  if (m_correlation_model_name == "MCTOYS" || m_correlation_model_name == "COMBMCTOYS") {
    // Check if more than one set is requested
    if (appliedSystematics().size() > 1) {
      // This should not pass applySystematicVariation ...
      efficiencyScaleFactor = 1.0;
      return CP::CorrectionCode::Error;
    }else {
      if (m_correlation_model_name == "MCTOYS") {
        auto toy = appliedSystematics().getToyVariationByBaseName("EL_EFF_" + m_sysSubstring + "MCTOY");
        toy.second = m_scale_toys;
        sys = m_result.getResult((m_rootTool->getFirstToyMCPosition() - 1 + toy.first + 1) * m_scale_toys);
      }
      if (m_correlation_model_name == "COMBMCTOYS") {
        auto toy = appliedSystematics().getToyVariationByBaseName("EL_EFF_" + m_sysSubstring + "COMBMCTOY");
        toy.second = m_scale_toys;
        sys = m_result.getResult((m_rootTool->getFirstToyMCPosition() - 1 + toy.first) * m_scale_toys);
      }
    }
    // return here for Toy variations
    efficiencyScaleFactor = sys;
    return CP::CorrectionCode::Ok;
  }

  // ==============================================================================//
  //We pass only one variation per time
  std::string variation("__" + appliedSystematics().begin()->subvariation());
  float param = appliedSystematics().begin()->parameter();
  auto func = [](double &sf, const double input) {
    sf = sf + input;
    return CP::CorrectionCode::Ok;
  };
  // ==============================================================================//
  // First the correlated
  if (m_nCorrSyst == 0) {
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + "CorrUncertainty" +
                                                                     variation))) {
      sys = param * sqrt(m_result.getTotalUncertainty() * m_result.getTotalUncertainty() 
			 - m_result.getResult(4) * m_result.getResult(4)); // total -stat
      func(efficiencyScaleFactor, sys);
    }
  }else if (m_correlation_model_name == "TOTAL") { // one "TOTAL" uncertainty
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
                                                                     m_correlation_model_name + "_" +
                                                                     "1NPCOR_PLUS_UNCOR" + variation))) {
      sys = param * m_result.getTotalUncertainty();
      func(efficiencyScaleFactor, sys);
    }
  }else { // Then do the proper models
    for (int i = 0; i < m_nCorrSyst; ++i) {/// number of correlated sources
      if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
                                                                       Form("CorrUncertaintyNP%d", i) + variation))) {
        sys = param * m_result.getResult(5 + i);
        func(efficiencyScaleFactor, sys);
      }
    }
  }
  // =======================================================================
  // Then the uncorrelated, we just need to see if the applied matches the current electron pt and eta
  if (m_correlation_model_name == "FULL") {// The Full Model
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
                                                                     m_correlation_model_name + "_" +
                                                                     Form("UncorrUncertaintyNP%d",
                                                                          currentUncorrSystRegion) + variation))) {
      sys = param * m_result.getResult(4);//
      func(efficiencyScaleFactor, sys);
    }
  }
  if (m_correlation_model_name == "SIMPLIFIED") {
    if (appliedSystematics().matchSystematic(CP::SystematicVariation("EL_EFF_" + m_sysSubstring +
                                                                     m_correlation_model_name + "_" +
                                                                     Form("UncorrUncertaintyNP%d",
                                                                          currentSimplifiedUncorrSystRegion) +
								     variation))) {
      sys = param * m_result.getResult(4);//
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
  if (!systematic.empty()) {
    CP::SystematicSet sys = affectingSystematics();
    return sys.find(systematic) != sys.end();
  }
  return true;
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
const Root::TResult &
AsgElectronEfficiencyCorrectionTool::calculate(const xAOD::Electron &egam, const unsigned int runnumber,
                                               int &currentSimplifiedUncorrSystRegion, int &currentUncorrSystRegion
                                               ) const {
  double cluster_eta(-9999.9);
  double et(0.0);

  et = egam.pt();
  const xAOD::CaloCluster *cluster = egam.caloCluster();
  if (cluster) {
    cluster_eta = cluster->etaBE(2);
  }
  /* For now the dataType must be set by the user. May be added to the IParticle class later.  */
  if (m_correlation_model_name == "SIMPLIFIED") {
    int ptbin = m_UncorrRegions->GetXaxis()->FindBin(et) - 1;
    int etabin = m_UncorrRegions->GetYaxis()->FindBin(fabs(cluster_eta)) - 1;
    int reg = ((etabin) * m_UncorrRegions->GetNbinsX() + ptbin);
    currentSimplifiedUncorrSystRegion = reg;
  }
  if (m_correlation_model_name == "FULL") {
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
          (et > itr_ptBEGIN->first && et < itr_ptBEGINplusOne->first)) {// find the pt bin
        etabin++;
        // if it is ordered in eta from smaller to larger ascending order
        // consider using std::lower_bound(begin,end) to find the position?
        if ((itr_ptBEGIN->second).at(0) >= 0) {
          cluster_eta_electron = fabs(cluster_eta);
        }else {
          cluster_eta_electron = (cluster_eta);
        };

        for (unsigned int etab = 0; etab < ((itr_ptBEGIN->second).size() - 1); ++etab) {// find the eta bin
          if ((cluster_eta_electron) > (itr_ptBEGIN->second).at(etab) &&
              (cluster_eta_electron) < (itr_ptBEGIN->second).at(etab + 1)) {
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
    currentUncorrSystRegion = reg;
  }
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
  if (m_correlation_model_name == "COMBMCTOYS") {
    m_affectedSys.insert((CP::SystematicVariation::makeToyEnsemble("EL_EFF_" + m_sysSubstring + "COMBMCTOY")));
  }else if (m_correlation_model_name == "MCTOYS") {
    m_affectedSys.insert((CP::SystematicVariation::makeToyEnsemble("EL_EFF_" + m_sysSubstring + "MCTOY")));
  }else if (m_nCorrSyst == 0) {
    m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + "CorrUncertainty", 1));
    m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + "CorrUncertainty", -1));
  }else if (!(m_correlation_model_name == "TOTAL")) {
    for (int i = 0; i < m_nCorrSyst; ++i) {
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + Form("CorrUncertaintyNP%d", i), 1));
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + Form("CorrUncertaintyNP%d", i), -1));
    }
  }
  // Different tratement for the uncorrelated
  if (m_correlation_model_name == "TOTAL") {
    m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
                                                 "1NPCOR_PLUS_UNCOR", 1));
    m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
                                                 "1NPCOR_PLUS_UNCOR", -1));
  }else if (m_correlation_model_name == "FULL") {
    for (int i = 0; i < m_nUncorrSyst; ++i) {
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
                                                   Form("UncorrUncertaintyNP%d", i), 1));
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
                                                   Form("UncorrUncertaintyNP%d", i), -1));
    }
  }else if (m_correlation_model_name == "SIMPLIFIED") {
    for (int i = 0; i < m_nSimpleUncorrSyst; ++i) {
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
                                                   Form("UncorrUncertaintyNP%d", i), 1));
      m_affectedSys.insert(CP::SystematicVariation("EL_EFF_" + m_sysSubstring + m_correlation_model_name + "_" +
                                                   Form("UncorrUncertaintyNP%d", i), -1));
    }
  }

  return CP::SystematicCode::Ok;
}
