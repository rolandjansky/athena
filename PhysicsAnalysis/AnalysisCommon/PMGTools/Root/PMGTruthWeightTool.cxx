/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// EDM include(s):
#ifndef XAOD_STANDALONE
  #include "AthAnalysisBaseComps/AthAnalysisHelper.h"
  #include "EventInfo/EventInfo.h"
  #include "EventInfo/EventType.h"
#endif

// Local include(s):
#include "PMGTools/PMGTruthWeightTool.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMetaData/FileMetaData.h"


namespace PMGTools
{
  PMGTruthWeightTool::PMGTruthWeightTool(const std::string& name)
    : asg::AsgMetadataTool(name)
    , m_metaDataContainer(nullptr)
  {
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
    // Check that we have loaded event weights
    if (m_weights.size() == 0) {
      ATH_MSG_FATAL("No MC weights found!");
      throw std::runtime_error(name() + ": No MC weights found!");
    }

    // Return appropriate weight from EventInfo: this should be identical to the TruthEvent
    try {
      return m_weights.at(m_weightIndices.at(weightName));
    } catch (const std::out_of_range& e) {
      ATH_MSG_FATAL("Weight \"" + weightName + "\" could not be found");
      throw std::runtime_error(name() + ": Weight \"" + weightName + "\" could not be found");
    }
  }


  bool PMGTruthWeightTool::hasWeight(const std::string& weightName) const {
    return (m_weightIndices.count(weightName) > 0);
  }


  StatusCode PMGTruthWeightTool::beginInputFile() {
    ATH_MSG_INFO("Attempting to retrieve truth meta data from a new file...");

    // Clear cached weights
    this->clearWeightLocationCaches();

    // Set the MC channel number to an impossible number (max-uint) to force meta data updating
    m_mcChannelNumber = uint32_t(-1);

    // Try to load MC channel number from file metadata
    ATH_MSG_INFO("Attempting to retrieve MC channel number...");
    const xAOD::FileMetaData *fmd = nullptr;
    if (inputMetaStore()->contains<xAOD::FileMetaData>("FileMetaData")) {
      ATH_CHECK(inputMetaStore()->retrieve(fmd, "FileMetaData"));
      float _flt_channel_number(-1);
      if (fmd->value(xAOD::FileMetaData::mcProcID, _flt_channel_number)) {
        m_mcChannelNumber = uint32_t(_flt_channel_number);
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
      if (m_useChannelZeroInMetaData) { ATH_MSG_WARNING("MC channel number in TruthMetaData is invalid - assuming that channel 0 has the correct information."); }

      // Load metadata from TruthMetaDataContainer if we have a valid channel number or if we're going to use 0 anyway
      // ... otherwise wait until we can load a channel number from EventInfo
      if (m_mcChannelNumber != uint32_t(-1) || m_useChannelZeroInMetaData) {
        if (this->loadMetaData().isFailure()) {
          ATH_MSG_WARNING("Could not load metadata for MC channel number " << m_mcChannelNumber);
        }
      }
    } else {
      // ... now try to load the weight container using the POOL metadata (not possible in AnalysisBase)
      // see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_read_the_truth_weight_nam
      if (this->loadPOOLMetaData().isFailure()) {
        ATH_MSG_WARNING("Could not load POOL HepMCWeightNames");
      }
    }

    return StatusCode::SUCCESS;
  }


  StatusCode PMGTruthWeightTool::beginEvent() {
    // Clear weights and channel number from previous event
    m_weights.clear();
    uint32_t mcChannelNumber(-1);

    // Try to read information about weights and channel number from EventInfo
    // 1. try the xAOD::EventInfo object
    if (evtStore()->contains<xAOD::EventInfo>("EventInfo")) {
      const xAOD::EventInfo* evtInfo;
      ATH_CHECK(evtStore()->retrieve(evtInfo, "EventInfo"));
      m_weights = evtInfo->mcEventWeights();
      mcChannelNumber = evtInfo->mcChannelNumber();

    // 2. otherwise, if we're not in AnalysisBase, see if there's an EventInfo object
#ifndef XAOD_STANDALONE
    } else if (evtStore()->contains<EventInfo>("McEventInfo")) {
      const EventInfo* evtInfo;
      ATH_CHECK(evtStore()->retrieve(evtInfo, "McEventInfo"));
      for (unsigned int idx = 0; idx < evtInfo->event_type()->n_mc_event_weights(); ++idx) {
        m_weights.push_back(evtInfo->event_type()->mc_event_weight(idx));
      }
      mcChannelNumber = evtInfo->event_type()->mc_channel_number();
#endif // not XAOD_STANDALONE

    // 3. otherwise let's bail here
    } else {
      ATH_MSG_ERROR("No event information is available in this file!");
      return StatusCode::SUCCESS;
    }

    // If the TruthMetaDataContainer was successfully loaded and the McChannelNumber has changed then reload.
    // If mcChannelNumber was not extracted from FileMetadata in beginInputFile() then
    // this will be triggered on the first event of the file.
    if (m_metaDataContainer != nullptr && (mcChannelNumber != m_mcChannelNumber)) {
      m_mcChannelNumber = mcChannelNumber;
      if (this->loadMetaData().isFailure()) {
        ATH_MSG_WARNING("Could not load metadata for MC channel number " << m_mcChannelNumber);
      }
    }

    // Validate weight caches against event information
    if (m_weightNames.size() != m_weights.size()) {
      // Special case to allow nominal weight to be used when this is the only weight in the event
      if ((m_weightNames.size() == 0) && (m_weights.size() == 1)) {
        ATH_MSG_WARNING("No weight names were available in this sample! Proceeding under the assumption that the single available weight should be 'nominal'");
        m_weightNames.push_back("nominal");
        m_weightIndices["nominal"] = 0;
      } else {
        ATH_MSG_ERROR("Expected " << m_weightNames.size() << " weights from the metadata but found " << m_weights.size() << " in this event");
        ATH_MSG_ERROR("Perhaps this sample was made using a release which did not correctly propagate the event weights.");
      }
    }

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
    for(std::size_t idx = 0; idx < (*itTruthMetaDataPtr)->weightNames().size(); ++idx ) {
      m_weightNames.push_back((*itTruthMetaDataPtr)->weightNames().at(idx));
      m_weightIndices[(*itTruthMetaDataPtr)->weightNames().at(idx)] = idx;
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
      m_weightNames.push_back(kv.first);
      m_weightIndices[kv.first] = kv.second;
    }
    return this->validateWeightLocationCaches();
#endif // XAOD_STANDALONE
  }


  StatusCode PMGTruthWeightTool::validateWeightLocationCaches() const {
    // Validate weight caches against one another
    if (m_weightNames.size() != m_weightIndices.size()) {
      ATH_MSG_ERROR("Found " << m_weightNames.size() << " but " << m_weightIndices.size() << " weight indices!");
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO("Successfully loaded information about " << m_weightNames.size() << " weights");
    return StatusCode::SUCCESS;
  }


  void PMGTruthWeightTool::clearWeightLocationCaches() {
    m_weightNames.clear();
    m_weightIndices.clear();
  }

} // namespace PMGTools
