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
    m_metaData(nullptr),
    m_uninitialized(true),
    m_evtInfo(nullptr) {
    declareProperty("MetaObjectName", m_metaName = "TruthMetaData");
  }

  StatusCode PMGTruthWeightTool::initialize() {
    ATH_MSG_DEBUG("Initialising... ");
    return StatusCode::SUCCESS;
  }

  std::shared_ptr<IPMGTruthWeightIndexRetriever> PMGTruthWeightTool::spawnTruthWeightIndexRetriever(
    std::string weightName) const {
    auto sp = m_indexRetrievers[weightName].lock();

    if (!sp) {
      m_indexRetrievers[weightName] = sp = std::make_shared<PMGTruthWeightIndexRetriever>(weightName);
      if (!m_metaData) {
        sp->update(m_poolWeightNames);
      } else {
        sp->update(m_metaData);
      }
    }

    return sp;
  }

  const std::vector<std::string>& PMGTruthWeightTool::getWeightNames() const {
    if (m_uninitialized) {
      ATH_MSG_ERROR("Weight name not found in event, is the meta data already loaded?");
      throw std::runtime_error("Weight name not found in event, is the meta data already loaded?");
    }

    if (m_poolWeightNames.size()) {
      return m_poolWeightNames;
    }

    return m_metaData->weightNames();
  }

  const std::vector<float>& PMGTruthWeightTool::getWeights() const {
    if (m_uninitialized || m_evtInfo == nullptr) {
      throw std::runtime_error("Cannot access MC weights. Tool is not properly initialized.");
    }

    // TODO: first check truth event. Else take from EventInfo. Check number of weights
    return m_evtInfo->mcEventWeights();
  }

  float PMGTruthWeightTool::getWeight(const std::string& weightName) const {
    return getWeights().at(getWeightIndex(weightName));
  }

  size_t PMGTruthWeightTool::getWeightIndex(const std::string& weightName) const {
    auto sp = this->spawnTruthWeightIndexRetriever(weightName);

    if (!sp) {
      ATH_MSG_ERROR("Requested weight \'" + weightName + "\' doesn't exist");
      return 0;
    }

    return sp->getIndex();
  }

  bool PMGTruthWeightTool::hasWeight(const std::string& weightName) const {
    const std::vector<std::string>& wns = getWeightNames();

    return std::find(wns.begin(), wns.end(), weightName) != wns.end();
  }

  StatusCode PMGTruthWeightTool::beginInputFile() {
    ATH_MSG_DEBUG("Retrieving truth meta data from a new file");
    m_metaDataContainer = nullptr;
    m_poolWeightNames.clear();

    if (inputMetaStore()->retrieve(m_metaDataContainer, m_metaName).isFailure()) {
#ifdef XAOD_STANDALONE
      // it's all over for eventloop release
      throw std::runtime_error("Cannot access metadata: " + m_metaName);
      return StatusCode::FAILURE;

#else
      // for athena release, one more try ... the POOL metadata!
      // See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_read_the_truth_weight_nam
      std::map<std::string, int> weightNamesMap;

      if (AAH::retrieveMetadata("/Generation/Parameters", "HepMCWeightNames", weightNamesMap,
                                inputMetaStore()).isFailure()) {
        //ok now it really is all over
        throw std::runtime_error(
                name() + " : Cannot access metadata: " + m_metaName + " and failed to get names from IOVMetadata");
        return StatusCode::FAILURE;
      }

      // Convert map into ordered vector
      m_poolWeightNames.resize(weightNamesMap.size());
      for (auto& w : weightNamesMap) {
        m_poolWeightNames[w.second] = w.first;
      }

      // Now weightNames is a vector of weight names in the correct order => we are done here
      return StatusCode::SUCCESS;
#endif
    }

    if (m_metaDataContainer == nullptr) {
      throw std::runtime_error("Cannot access metadata: " + m_metaName);
    }

    // Set the current MC channel number to a large number to force updating
    // the meta data on the first event of the new file
    m_mcChanNo = 999999999;
    return StatusCode::SUCCESS;
  }

  StatusCode PMGTruthWeightTool::beginEvent() {
    m_evtInfo = nullptr;
    ATH_CHECK(evtStore()->retrieve(m_evtInfo, "EventInfo"));
    uint32_t mcChannelNumber = m_evtInfo->mcChannelNumber();

    if (m_poolWeightNames.size()) {
      m_uninitialized = false;
      return StatusCode::SUCCESS;
    }

    if (!m_metaDataContainer) {
      return StatusCode::FAILURE;
    }

    if (m_uninitialized || (mcChannelNumber != m_mcChanNo)) {
      for (auto metaData : *m_metaDataContainer) {
        if (metaData->mcChannelNumber() == mcChannelNumber) {
          m_uninitialized = false;
          m_mcChanNo = mcChannelNumber;
          m_metaData = metaData;
          this->onNewMetaData();
          return StatusCode::SUCCESS;
        }
      }

      ATH_MSG_ERROR("No matching TruthMetaData found");
      return StatusCode::FAILURE;
    }

    return StatusCode::SUCCESS;
  }

  void PMGTruthWeightTool::onNewMetaData() {
    ATH_MSG_DEBUG("Updating all spawned index retrievers");

    for (auto& indexRetriever : m_indexRetrievers) {
      auto sp = indexRetriever.second.lock();
      if (sp) {
        sp->update(m_metaData);
      }
    }
  }
} // namespace PMGTools
