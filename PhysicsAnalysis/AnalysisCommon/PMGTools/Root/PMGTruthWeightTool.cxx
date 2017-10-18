/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ROOT include(s):
#include "TString.h"

// EDM include(s):
#ifndef XAOD_STANDALONE
  #include "AthAnalysisBaseComps/AthAnalysisHelper.h"
#endif

// Local include(s):
#include "PMGTools/PMGTruthWeightTool.h"
#include "PMGTools/PMGTruthWeightIndexRetriever.h"


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


  const std::vector<float>& PMGTruthWeightTool::getWeights() const {
    if (m_evtInfo == nullptr) {
      ATH_MSG_ERROR("Cannot access MC weights as EventInfo could not be read. Maybe beginEvent() was not called?");
      throw std::runtime_error(name() + ": Cannot access MC weights as EventInfo could not be read.  Maybe beginEvent() was not called?");
    }

    // Read weights from EventInfo: this should be identical to the TruthEvent
    return m_evtInfo->mcEventWeights();
  }


  float PMGTruthWeightTool::getWeight(const std::string& weightName) const {
    return getWeights().at(getWeightIndex(weightName));
  }


  size_t PMGTruthWeightTool::getWeightIndex(const std::string& weightName) const {
    if (!this->hasWeight(weightName)) {
      ATH_MSG_ERROR("Requested weight \'" + weightName + "\' doesn't exist");
      return 0;
    }

    return m_weightIndices.at(weightName);
  }


  bool PMGTruthWeightTool::hasWeight(const std::string& weightName) const {
    return (m_weightIndices.count(weightName) > 0);
  }


  StatusCode PMGTruthWeightTool::beginInputFile() {
    // Retrieve weight names and indices
    ATH_MSG_INFO("Retrieving truth meta data from a new file");

    // Start by checking whether metadata exists in the store
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
        ATH_MSG_WARNING("TruthMetaData McChannelNumber is invalid - assuming that channel 0 has the correct information.");
      }
    } else {
#ifdef XAOD_STANDALONE
      // AnalysisBase can only use the xAOD::TruthMetaDataContainer, so abort here
      throw std::runtime_error(name() + ": Cannot access xAOD::TruthMetaDataContainer named " + m_metaName);
      return StatusCode::FAILURE;
#else
      // Try to load the weight container using the POOL metadata
      // see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_read_the_truth_weight_nam
      std::map<std::string, int> hepMCWeightNamesMap;
      if (AAH::retrieveMetadata("/Generation/Parameters", "HepMCWeightNames", hepMCWeightNamesMap, inputMetaStore()).isFailure()) {
        throw std::runtime_error(name() + " : Cannot access metadata: " + m_metaName + " and failed to get names from IOVMetadata");
        return StatusCode::FAILURE;
      }
      ATH_CHECK(this->loadMetaData(hepMCWeightNamesMap));
#endif
    }

    // Set the current MC channel number to an impossible number to force
    // the meta data to update on the first event of the new file
    m_mcChannelNumber = 999999999;
    return StatusCode::SUCCESS;
  }


  StatusCode PMGTruthWeightTool::loadMetaData(const xAOD::TruthMetaData* truthMetaData) {
    ATH_MSG_INFO("Loading meta data from xAOD TruthMetaData");
    // Update cached weight data
    this->clearWeightCaches();
    for(std::size_t idx = 0; idx < truthMetaData->weightNames().size(); ++idx ) {
      m_weightNames.push_back(truthMetaData->weightNames().at(idx));
      m_weightIndices[truthMetaData->weightNames().at(idx)] = idx;
    }
    return this->validateWeightCaches();
  }


  StatusCode PMGTruthWeightTool::loadMetaData(const std::map<std::string, int>& hepMCWeightNamesMap) {
    ATH_MSG_INFO("Loading meta data from HepMC IOVMetaDataContainer");
    this->clearWeightCaches();
    // Convert map into ordered vector
    for (auto& w : hepMCWeightNamesMap) {
      m_weightNames.push_back(w.first);
      m_weightIndices[w.first] = w.second;
    }
    return this->validateWeightCaches();
  }


  StatusCode PMGTruthWeightTool::validateWeightCaches() {
    // Validate weight caches against one another
    if (m_weightNames.size() != m_weightIndices.size()) {
      ATH_MSG_ERROR("Found " << m_weightNames.size() << " but " << m_weightIndices.size() << " weight indices!");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Loaded information about " << m_weightNames.size() << " weights");
    return StatusCode::SUCCESS;
  }


  void PMGTruthWeightTool::clearWeightCaches() {
    m_weightNames.clear();
    m_weightIndices.clear();
  }


  StatusCode PMGTruthWeightTool::beginEvent() {
    // Load EventInfo from the event store
    m_evtInfo = nullptr;
    ATH_CHECK(evtStore()->retrieve(m_evtInfo, "EventInfo"));
    uint32_t mcChannelNumber = m_evtInfo->mcChannelNumber();

    // If metadata was successfully loaded and the McChannelNumber has changed then reload.
    // As beginInputFile() sets m_mcChannelNumber to an impossible number,
    // this will be triggered on the first event of each file.
    if (m_metaDataContainer != nullptr && (mcChannelNumber != m_mcChannelNumber)) {
      this->clearWeightCaches();
      for (auto truthMetaData : *m_metaDataContainer) {
        // Take the truthMetaData object with the correct channel number (or the one with channel 0 if metaData is invalid)
        if (truthMetaData->mcChannelNumber() == mcChannelNumber ||
            (m_hasInvalidMcChannel && (truthMetaData->mcChannelNumber() == 0))) {
          ATH_CHECK(this->loadMetaData(truthMetaData));
          m_mcChannelNumber = mcChannelNumber;
        }
      }
    }

    // Validate weight caches against event information
    if (m_weightNames.size() != m_evtInfo->mcEventWeights().size()) {
      ATH_MSG_ERROR("Expected " << m_weightNames.size() << " from the metadata but this event has " << m_weightIndices.size());
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

} // namespace PMGTools
