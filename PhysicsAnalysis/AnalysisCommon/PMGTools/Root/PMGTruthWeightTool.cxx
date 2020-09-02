/*
   Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#ifndef XAOD_STANDALONE
  #include "AthAnalysisBaseComps/AthAnalysisHelper.h"
  #include "EventInfo/EventInfo.h"
  #include "EventInfo/EventType.h"
#endif

// Local include(s):
#include <CxxUtils/StringUtils.h>
#include <RootCoreUtils/StringUtil.h>

#include <PATInterfaces/SystematicRegistry.h>
#include <xAODEventInfo/EventInfo.h>
#include <xAODMetaData/FileMetaData.h>

#include <PMGTools/PMGTruthWeightTool.h>

// For replacing substrings
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/case_conv.hpp>

namespace PMGTools
{
  PMGTruthWeightTool::PMGTruthWeightTool(const std::string& name)
    : asg::AsgMetadataTool(name)
  {
    declareProperty("MetaObjectName", m_metaName = "TruthMetaData");
  }


  StatusCode PMGTruthWeightTool::initialize()
  {
    ATH_MSG_DEBUG("Initialising...");

    ATH_MSG_INFO("Attempting to retrieve truth meta data from the first file...");

    // Clear cached weights
    clearWeightLocationCaches();

    // Set the MC channel number to an impossible number (max-uint) to force meta data updating
    m_mcChannelNumber = uint32_t(-1);

    // Try to load MC channel number from file metadata
    ATH_MSG_INFO("Attempting to retrieve MC channel number...");
    const xAOD::FileMetaData *fmd = nullptr;
    if (inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
      ATH_CHECK(inputMetaStore()->retrieve(fmd, "FileMetaData"));
      float fltChannelNumber(-1);
      if (fmd->value(xAOD::FileMetaData::mcProcID, fltChannelNumber)) {
        m_mcChannelNumber = uint32_t(fltChannelNumber);
      }
    }
    if (m_mcChannelNumber == uint32_t(-1)) {
      ATH_MSG_WARNING("... MC channel number could not be loaded");
    } else {
      ATH_MSG_INFO("... MC channel number identified as " << m_mcChannelNumber);
    }

    // Start by trying to load metadata from the store
    m_metaDataContainer = nullptr;
    if (inputMetaStore()->contains<xAOD::TruthMetaDataContainer>(m_metaName)) {
      ATH_CHECK(inputMetaStore()->retrieve(m_metaDataContainer, m_metaName));
      ATH_MSG_INFO("Loaded xAOD::TruthMetaDataContainer");

      // Check for incorrectly stored McChannelNumber
      m_useChannelZeroInMetaData = true;
      for (auto truthMetaData : *m_metaDataContainer) {
        if (truthMetaData->mcChannelNumber() != 0) { m_useChannelZeroInMetaData = false; }
      }
      // If we only have one metadata item take MC channel from there if needed
      if (m_mcChannelNumber == uint32_t(-1) && m_metaDataContainer->size() == 1) {
        m_mcChannelNumber = m_metaDataContainer->at(0)->mcChannelNumber();
        ATH_MSG_WARNING("... MC channel number taken from the metadata as " << m_mcChannelNumber);
      }
      if (m_useChannelZeroInMetaData) { ATH_MSG_WARNING("MC channel number in TruthMetaData is invalid - assuming that channel 0 has the correct information."); }

      // Load metadata from TruthMetaDataContainer if we have a valid channel number or if we're going to use 0 anyway
      // ... otherwise wait until we can load a channel number from EventInfo
      if (m_mcChannelNumber != uint32_t(-1) || m_useChannelZeroInMetaData) {
        if (loadMetaData().isFailure()) {
          ATH_MSG_ERROR("Could not load metadata for MC channel number " << m_mcChannelNumber);
          return StatusCode::FAILURE;
        }
      }
    } else {
      // ... now try to load the weight container using the POOL metadata (not possible in AnalysisBase)
      // see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_read_the_truth_weight_nam
      if (loadPOOLMetaData().isFailure()) {
        ATH_MSG_ERROR("Could not load POOL HepMCWeightNames");
        return StatusCode::FAILURE;
      }
    }

    // Add the affecting systematics to the global registry
    CP::SystematicRegistry& registry = CP::SystematicRegistry::getInstance();
    if (!registry.registerSystematics(*this)) {
      ATH_MSG_ERROR("unable to register the systematics");
      return StatusCode::FAILURE;
    }

    ATH_MSG_DEBUG("Successfully initialized!");

    return StatusCode::SUCCESS;
  }


  const std::vector<std::string>& PMGTruthWeightTool::getWeightNames() const {
    return m_weightNames;
  }


  float PMGTruthWeightTool::getWeight(const std::string& weightName) const {
    // Check that we have loaded event weights
    if (m_weights.size() == 0) {
      ATH_MSG_FATAL("No MC weights found!");
      throw std::runtime_error(name() + ": No MC weights found!");
    }

    // Return appropriate weight from EventInfo: this should be identical to the TruthEvent
    try {
      return m_weights.at(m_weightIndices.at(weightName));
    } catch (const std::out_of_range& e) {
      // Before throwing an exception, try to recover with bad naming conventions
      std::string strippedName = boost::algorithm::to_lower_copy(boost::replace_all_copy(weightName, " ", ""));
      for (const auto & weight:m_weightNames){
        if (strippedName==boost::algorithm::to_lower_copy(boost::replace_all_copy(weight," ", ""))){
          ATH_MSG_WARNING("Using weight name \"" << weight << "\" instead of requested \"" << weightName << "\"");
          return getWeight(weight);
        }
      }
      ATH_MSG_FATAL("Weight \"" + weightName + "\" could not be found");
      throw std::runtime_error(name() + ": Weight \"" + weightName + "\" could not be found");
    }
  }


  bool PMGTruthWeightTool::hasWeight(const std::string& weightName) const {
    return (m_weightIndices.count(weightName) > 0);
  }


  float PMGTruthWeightTool::getSysWeight() const
  {
    return m_currentWeightData->weight;
  }


  size_t PMGTruthWeightTool::getSysWeightIndex() const
  {
    return m_currentWeightData->index;
  }


  bool PMGTruthWeightTool::isAffectedBySystematic(const CP::SystematicVariation& systematic) const
  {
    return m_systematicsSet.find( systematic ) != m_systematicsSet.end();
  }


  CP::SystematicSet PMGTruthWeightTool::affectingSystematics() const
  {
    return m_systematicsSet;
  }


  CP::SystematicSet PMGTruthWeightTool::recommendedSystematics() const
  {
    return affectingSystematics();
  }


  CP::SystematicCode PMGTruthWeightTool::applySystematicVariation(const CP::SystematicSet& systConfig)
  {
    auto iter = m_weightData.find (systConfig);
    if (iter != m_weightData.end())
    {
      m_currentWeightData = &iter->second;
      return CP::SystematicCode::Ok;
    }

    CP::SystematicSet currentSys;
    ANA_CHECK_SET_TYPE (CP::SystematicCode);
    ANA_CHECK (CP::SystematicSet::filterForAffectingSystematics (systConfig, m_systematicsSet, currentSys));

    WeightData currentWeight{};
    currentWeight.index = m_weightIndicesSys.at(currentSys.name());
    if (!m_weights.empty())
    {
      currentWeight.weight = m_weights.at(currentWeight.index);
    }

    auto insert = m_weightData.emplace(systConfig, std::move(currentWeight));
    m_currentWeightData = &insert.first->second;

    return CP::SystematicCode::Ok;
  }


  StatusCode PMGTruthWeightTool::beginInputFile()
  {
    // Detect possible MC channel number change
    uint32_t mcChannelNumber(-1);

    // Try to load MC channel number from file metadata
    ATH_MSG_DEBUG("Attempting to retrieve MC channel number...");
    const xAOD::FileMetaData *fmd = nullptr;
    if (inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
      ATH_CHECK(inputMetaStore()->retrieve(fmd, "FileMetaData"));
      float fltChannelNumber(-1);
      if (fmd->value(xAOD::FileMetaData::mcProcID, fltChannelNumber)) {
        mcChannelNumber = uint32_t(fltChannelNumber);
      }
    }
    if (m_mcChannelNumber != uint32_t(-1) && mcChannelNumber != uint32_t(-1) && mcChannelNumber != m_mcChannelNumber) {
      ATH_MSG_ERROR("MC channel number from a new file does not match the previously processed files.");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  StatusCode PMGTruthWeightTool::beginEvent() {
    // Clear weights and channel number from previous event
    m_weights.clear();
    m_weightData.clear();

    // Try to read information about weights and channel number from EventInfo
    // 1. try the xAOD::EventInfo object
    if (evtStore()->contains<xAOD::EventInfo>("EventInfo")) {
      const xAOD::EventInfo* evtInfo;
      ATH_CHECK(evtStore()->retrieve(evtInfo, "EventInfo"));
      m_weights = evtInfo->mcEventWeights();

    // 2. otherwise, if we're not in AnalysisBase, see if there's an EventInfo object
#ifndef XAOD_STANDALONE
    } else if (evtStore()->contains<EventInfo>("McEventInfo")) {
      const EventInfo* evtInfo;
      ATH_CHECK(evtStore()->retrieve(evtInfo, "McEventInfo"));
      for (unsigned int idx = 0; idx < evtInfo->event_type()->n_mc_event_weights(); ++idx) {
        m_weights.push_back(evtInfo->event_type()->mc_event_weight(idx));
      }
#endif // not XAOD_STANDALONE

    // 3. otherwise let's bail here
    } else {
      ATH_MSG_ERROR("No event information is available in this file!");
      return StatusCode::FAILURE;
    }

    // Validate weight caches against event information
    if (m_weightNames.size() != m_weights.size()) {
      // Special case to allow nominal weight to be used when this is the only weight in the event
      if ((m_weightNames.size() == 0) && (m_weights.size() == 1)) {
        ATH_MSG_WARNING("No weight names were available in this sample! Proceeding under the assumption that the single available weight should be 'nominal'");
        m_weightNames.push_back("nominal");
        m_weightIndices["nominal"] = 0;
        m_weightIndicesSys[""] = 0;
      } else {
        ATH_MSG_ERROR("Expected " << m_weightNames.size() << " weights from the metadata but found " << m_weights.size() << " in this event");
        ATH_MSG_ERROR("Perhaps this sample was made using a release which did not correctly propagate the event weights.");
      }
    }

    // Apply nominal systematics by default
    ANA_CHECK (applySystematicVariation (CP::SystematicSet()));

    return StatusCode::SUCCESS;
  }


  StatusCode PMGTruthWeightTool::loadMetaData() {
    // Clear cached weight names and positions
    this->clearWeightLocationCaches();

    // Find the correct truth meta data object
    uint32_t targetChannelNumber = (m_useChannelZeroInMetaData ? 0 : m_mcChannelNumber);
    ATH_MSG_INFO("Attempting to load weight meta data from xAOD TruthMetaData for channel " << targetChannelNumber);
    auto itTruthMetaDataPtr = std::find_if(m_metaDataContainer->begin(), m_metaDataContainer->end(),
      [targetChannelNumber] (const auto& it) { return it->mcChannelNumber() == targetChannelNumber; }
    );

    // If no such object is found then return
    if (itTruthMetaDataPtr == m_metaDataContainer->end()) {
      ATH_MSG_ERROR("Could not load weight meta data!");
      return StatusCode::FAILURE;
    }

    // Update cached weight data
    const std::vector<std::string> &truthWeightNames = (*itTruthMetaDataPtr)->weightNames();
    for(std::size_t idx = 0; idx < truthWeightNames.size(); ++idx ) {
      ANA_MSG_VERBOSE("    " << truthWeightNames.at(idx));
      m_weightNames.push_back(truthWeightNames.at(idx));
      m_weightIndices[truthWeightNames.at(idx)] = idx;

      std::string sysName = weightNameToSys(truthWeightNames.at(idx));
      if (!sysName.empty()) {
        m_systematicsSet.insert(CP::SystematicVariation(sysName));
      }
      m_weightIndicesSys[sysName] = idx;
    }
    return this->validateWeightLocationCaches();
  }


  StatusCode PMGTruthWeightTool::loadPOOLMetaData() {
    // AnalysisBase can only use the xAOD::TruthMetaDataContainer, so skip this
#ifdef XAOD_STANDALONE
    return StatusCode::SUCCESS;
#else
    ATH_MSG_INFO("Looking for POOL HepMC IOVMetaData...");
    std::map<std::string, int> hepMCWeightNamesMap;
    if (AAH::retrieveMetadata("/Generation/Parameters", "HepMCWeightNames", hepMCWeightNamesMap, inputMetaStore()).isFailure()) {
      ATH_MSG_FATAL("Cannot access metadata " << m_metaName << " and failed to get names from IOVMetadata");
      return StatusCode::FAILURE;
    }

    // Clear cached weight names and positions
    this->clearWeightLocationCaches();

    // Use input map to fill the index map and the weight names
    ATH_MSG_INFO("Attempting to load weight meta data from HepMC IOVMetaData container");
    for (auto& kv : hepMCWeightNamesMap) {
      ANA_MSG_VERBOSE("    " << kv.first);
      m_weightNames.push_back(kv.first);
      m_weightIndices[kv.first] = kv.second;

      std::string sysName = weightNameToSys(kv.first);
      if (!sysName.empty()) {
        m_systematicsSet.insert(CP::SystematicVariation(sysName));
      }
      m_weightIndicesSys[sysName] = kv.second;
    }
    return this->validateWeightLocationCaches();
#endif // XAOD_STANDALONE
  }


  StatusCode PMGTruthWeightTool::validateWeightLocationCaches() {
    // Validate weight caches against one another
    if (m_weightNames.size() != m_weightIndices.size()) {
      ATH_MSG_ERROR("Found " << m_weightNames.size() << " but " << m_weightIndices.size() << " weight indices!");
      return StatusCode::FAILURE;
    }
    // Check if we can work with systematics
    auto it = m_weightIndicesSys.find("");
    if (it == m_weightIndicesSys.end()) {
      ATH_MSG_WARNING("Could not detect nominal weight automatically. The first weight will also be considered nominal.");
      m_weightIndicesSys[""] = 0;
    }

    ATH_MSG_INFO("Successfully loaded information about " << m_weightNames.size() << " weights");
    return StatusCode::SUCCESS;
  }


  void PMGTruthWeightTool::clearWeightLocationCaches() {
    m_weightNames.clear();
    m_weightIndices.clear();
    m_weightIndicesSys.clear();
    m_systematicsSet.clear();
  }


  std::string PMGTruthWeightTool::weightNameToSys (const std::string &name) const
  {
    if (name.empty()) // empty weight is nominal
    {
      return "";
    }

    // Trim trailing whitespace
    std::string sys = CxxUtils::StringUtils::trim (name);
    if (sys == "nominal" // Powheg calls it "nominal"
      || sys == "Weight") // Sherpa names the nominal weight just "Weight"
    {
      return "";
    }

    sys = RCU::substitute (sys, " set = ", "_"); // Powheg
    sys = RCU::substitute (sys, " = ", "_"); // Powheg
    sys = RCU::substitute (sys, "=", "");
    sys = RCU::substitute (sys, ",", "");
    sys = RCU::substitute (sys, ".", "");
    sys = RCU::substitute (sys, ":", "");
    sys = RCU::substitute (sys, " ", "_");
    sys = RCU::substitute (sys, "#", "num");
    sys = RCU::substitute (sys, "\n", "_");
    sys = RCU::substitute (sys, "/", "over"); // MadGraph

    return generatorSystematicsPrefix + sys;
  }
} // namespace PMGTools
