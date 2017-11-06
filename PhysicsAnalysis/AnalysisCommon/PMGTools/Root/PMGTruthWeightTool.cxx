/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#ifndef XAOD_STANDALONE
  #include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

// Local include(s):
#include "PMGTools/PMGTruthWeightTool.h"
#include "xAODMetaData/FileMetaData.h"


namespace PMGTools
{
  PMGTruthWeightTool::PMGTruthWeightTool(const std::string& name) :
    asg::AsgMetadataTool(name),
    m_metaDataContainer(nullptr),
    m_evtInfo(nullptr) {
    declareProperty("MetaObjectName", m_metaName = "TruthMetaData");
  }


  StatusCode PMGTruthWeightTool::initialize() {
    ATH_MSG_INFO("Initialising...");
    return StatusCode::SUCCESS;
  }


  const std::vector<std::string>& PMGTruthWeightTool::getWeightNames() const {
    return m_weightNames;
  }


  float PMGTruthWeightTool::getWeight(const std::string& weightName) const {
    // Check that we have a valid EventInfo pointer
    if (m_evtInfo == nullptr) {
      ATH_MSG_FATAL("Cannot access MC weights as EventInfo could not be read. Maybe beginEvent() was not called?");
      throw std::runtime_error(name() + ": Cannot access MC weights as EventInfo could not be read.  Maybe beginEvent() was not called?");
    }

    // Return appropriate weight from EventInfo: this should be identical to the TruthEvent
    try {
      return m_evtInfo->mcEventWeights().at(m_weightIndices.at(weightName));
    } catch (const std::out_of_range& e) {
      ATH_MSG_FATAL("Weight \"" + weightName + "\" could not be found");
      throw std::runtime_error(name() + ": Weight \"" + weightName + "\" could not be found");
    }
  }


  bool PMGTruthWeightTool::hasWeight(const std::string& weightName) const {
    return (m_weightIndices.count(weightName) > 0);
  }


  StatusCode PMGTruthWeightTool::beginInputFile() {
    // Retrieve weight names and indices
    ATH_MSG_INFO("Retrieving truth meta data from a new file");

    // Clear cached weights
    this->clearWeightCaches();

    // Set the MC channel number to an impossible number (max-uint) to force meta data updating
    m_mcChannelNumber = uint32_t(-1);

    // Try to load MC channel number from file metadata
    const xAOD::FileMetaData *fmd = nullptr;
    if (inputMetaStore()->contains<xAOD::TruthMetaDataContainer>("FileMetaData")) {
      ATH_CHECK(inputMetaStore()->retrieve(fmd, "FileMetaData"));
      float _flt_channel_number(-1);
      if (fmd->value(xAOD::FileMetaData::mcProcID, _flt_channel_number)) {
        m_mcChannelNumber = uint32_t(_flt_channel_number);
      }
    }
    ATH_MSG_DEBUG("MC channel number identified as " << m_mcChannelNumber);

    // Start by trying to load metadata from the store
    m_metaDataContainer = nullptr;
    if (inputMetaStore()->contains<xAOD::TruthMetaDataContainer>(m_metaName)) {
      ATH_MSG_DEBUG("Using xAOD::TruthMetaDataContainer");
      ATH_CHECK(inputMetaStore()->retrieve(m_metaDataContainer, m_metaName));
      // Check for incorrectly stored McChannelNumber
      m_hasInvalidMcChannel = true;
      for (auto truthMetaData : *m_metaDataContainer) {
        if (truthMetaData->mcChannelNumber() != 0) { m_hasInvalidMcChannel = false; }
      }
      if (m_hasInvalidMcChannel) {
        ATH_MSG_WARNING("McChannelNumber in TruthMetaData is invalid - assuming that channel 0 has the correct information.");
      }
      // Load metadata from TruthMetaDataContainer if we have a valid channel number
      // ... otherwise wait until we can load a channel number from EventInfo
      if (m_mcChannelNumber != uint32_t(-1)) { ATH_CHECK(this->loadMetaData()); }
    } else {
#ifdef XAOD_STANDALONE
      // AnalysisBase can only use the xAOD::TruthMetaDataContainer, so abort here
      ATH_MSG_FATAL("Cannot access xAOD::TruthMetaDataContainer named " << m_metaName);
      throw std::runtime_error(name() + ": Cannot access xAOD::TruthMetaDataContainer named " + m_metaName);
      return StatusCode::FAILURE;
#else
      // ... now try to load the weight container using the POOL metadata
      // see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_read_the_truth_weight_nam
      std::map<std::string, int> hepMCWeightNamesMap;
      if (AAH::retrieveMetadata("/Generation/Parameters", "HepMCWeightNames", hepMCWeightNamesMap, inputMetaStore()).isFailure()) {
        ATH_MSG_FATAL("Cannot access metadata " << m_metaName << " and failed to get names from IOVMetadata");
        throw std::runtime_error(name() + " : Cannot access metadata: " + m_metaName + " and failed to get names from IOVMetadata");
        return StatusCode::FAILURE;
      }
      // Load metadata from POOL metadata
      ATH_CHECK(this->loadMetaData(hepMCWeightNamesMap));
#endif
    }

    return StatusCode::SUCCESS;
  }


  StatusCode PMGTruthWeightTool::beginEvent() {
    // Load EventInfo from the event store
    m_evtInfo = nullptr;
    ATH_CHECK(evtStore()->retrieve(m_evtInfo, "EventInfo"));
    uint32_t mcChannelNumber = m_evtInfo->mcChannelNumber();

    // If the TruthMetaDataContainer was successfully loaded and the McChannelNumber has changed then reload.
    // If mcChannelNumber was not extracted from FileMetadata in beginInputFile() then
    // this will be triggered on the first event of the file.
    if (m_metaDataContainer != nullptr && (mcChannelNumber != m_mcChannelNumber)) {
      m_mcChannelNumber = mcChannelNumber;
      ATH_CHECK(this->loadMetaData());
    }

    // Validate weight caches against event information
    if (m_weightNames.size() != m_evtInfo->mcEventWeights().size()) {
      ATH_MSG_ERROR("Expected " << m_weightNames.size() << " from the metadata but this event has " << m_weightIndices.size());
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }


  StatusCode PMGTruthWeightTool::loadMetaData() {
    // Clear cached weights
    this->clearWeightCaches();

    // Find the correct truth meta data object
    uint32_t targetChannelNumber = (m_hasInvalidMcChannel ? 0 : m_mcChannelNumber);
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
    for(std::size_t idx = 0; idx < (*itTruthMetaDataPtr)->weightNames().size(); ++idx ) {
      m_weightNames.push_back((*itTruthMetaDataPtr)->weightNames().at(idx));
      m_weightIndices[(*itTruthMetaDataPtr)->weightNames().at(idx)] = idx;
    }
    return this->validateWeightCaches();
  }


  StatusCode PMGTruthWeightTool::loadMetaData(const std::map<std::string, int>& hepMCWeightNamesMap) {
    // Clear cached weights
    this->clearWeightCaches();

    // Use input map to fill the index map and the weight names
    ATH_MSG_INFO("Attempting to load weight meta data from HepMC IOVMetaData container");
    for (auto& kv : hepMCWeightNamesMap) {
      m_weightNames.push_back(kv.first);
      m_weightIndices[kv.first] = kv.second;
    }
    return this->validateWeightCaches();
  }


  StatusCode PMGTruthWeightTool::validateWeightCaches() const {
    // Validate weight caches against one another
    if (m_weightNames.size() != m_weightIndices.size()) {
      ATH_MSG_ERROR("Found " << m_weightNames.size() << " but " << m_weightIndices.size() << " weight indices!");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Successfully loaded information about " << m_weightNames.size() << " weights");
    return StatusCode::SUCCESS;
  }


  void PMGTruthWeightTool::clearWeightCaches() {
    m_weightNames.clear();
    m_weightIndices.clear();
  }

} // namespace PMGTools
