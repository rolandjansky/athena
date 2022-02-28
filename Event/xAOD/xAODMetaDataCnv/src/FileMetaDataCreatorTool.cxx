/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// Local include(s):
#include "FileMetaDataCreatorTool.h"

// System include(s):
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <utility>

// Athena metadata EDM:
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "StoreGate/ReadHandle.h"
#include "StoreGate/WriteHandle.h"
#include "xAODMetaData/FileMetaData.h"
#include "xAODMetaData/FileMetaDataAuxInfo.h"


namespace xAODMaker {

StatusCode
    FileMetaDataCreatorTool::initialize() {
      ATH_CHECK(m_eventStore.retrieve());
      ATH_CHECK(m_metaDataSvc.retrieve());
      ATH_CHECK(m_inputMetaDataStore.retrieve());
      ATH_CHECK(m_tagInfoMgr.retrieve());

      // If DataHeader key not specified, try determining it
      if (m_dataHeaderKey.empty()) {
        const auto *parentAlg = dynamic_cast< const INamedInterface* >(parent());
        if (parentAlg)
          m_dataHeaderKey = parentAlg->name();
      }

      // Listen for the begin of an input file. Act after MetaDataSvc (prio 80) and
      // TagInfoMgr (prio 50). That means the FileMetaDataTool be called first
      ServiceHandle< IIncidentSvc > incidentSvc("IncidentSvc", name());
      ATH_CHECK(incidentSvc.retrieve());
      incidentSvc->addListener(this, "EndInputFile", 40);

      // Create a fresh object to fill
      ATH_MSG_DEBUG("Creating new xAOD::FileMetaData object to fill");
      m_info = std::make_unique< xAOD::FileMetaData >();
      m_aux  = std::make_unique< xAOD::FileMetaDataAuxInfo >();
      m_info->setStore(m_aux.get());

      // FileMetaData has no content
      m_filledNonEvent = false;
      m_filledEvent = false;

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

void
    FileMetaDataCreatorTool::handle(const Incident& inc) {
      // gracefully ignore unexpected incident types
      if (inc.type() == "EndInputFile") {
        // Lock the tool while we work on the FileMetaData
        std::lock_guard lock(m_toolMutex);
        if (!updateFromNonEvent().isSuccess())
          ATH_MSG_DEBUG("Failed to fill FileMetaData with non-event info");
      }
    }

StatusCode
    FileMetaDataCreatorTool::postInitialize() {
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::preExecute() {
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::preStream() {
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::preFinalize() {
      std::lock_guard lock(m_toolMutex);

      if (!m_filledEvent || !m_filledNonEvent) {
        ATH_MSG_DEBUG("Not writing empty or incomplete FileMetaData object");
        return StatusCode::SUCCESS;
      }

      // Remove any existing objects with this key
      if (!m_metaDataSvc->contains< xAOD::FileMetaData >(m_key)) {
        auto info = std::make_unique< xAOD::FileMetaData >();
        auto aux = std::make_unique< xAOD::FileMetaDataAuxInfo >();
        info->setStore(aux.get());
        ATH_CHECK(m_metaDataSvc->record(std::move(info), m_key));
        ATH_CHECK(m_metaDataSvc->record(std::move(aux), m_key + "Aux."));
      }

      // Replace content in store with content created for this stream
      auto *output = m_metaDataSvc->tryRetrieve< xAOD::FileMetaData >(m_key);
      if (output) *output = *m_info;
      else ATH_MSG_DEBUG("cannot copy FileMetaData payload to output");

      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::finalize() {
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::postExecute() {
      // Lock the tool while working with FileMetaData
      std::lock_guard lock(m_toolMutex);

      // Fill information from TagInfo and Simulation Parameters
      if (!updateFromNonEvent().isSuccess())
        ATH_MSG_DEBUG("Failed to fill FileMetaData with non-event info");

      // Sanity check
      if (!(m_info && m_aux)) {
        ATH_MSG_DEBUG("No xAOD::FileMetaData object to fill");
        return StatusCode::SUCCESS;
      }

      {  // MC channel, run and/or lumi block numbers
        const xAOD::EventInfo* eventInfo = nullptr;
        StatusCode sc = StatusCode::FAILURE;

        if (m_eventStore->contains< xAOD::EventInfo >(m_eventInfoKey))
          sc = m_eventStore->retrieve(eventInfo, m_eventInfoKey);
        else if (m_eventStore->contains< xAOD::EventInfo >("Mc" + m_eventInfoKey))
          sc = m_eventStore->retrieve(eventInfo, "Mc" + m_eventInfoKey);

        if (eventInfo && sc.isSuccess()) {
          addUniqueValue("runNumbers", eventInfo->runNumber());
          addUniqueValue("lumiBlocks", eventInfo->lumiBlock());
          // Return if object has already been filled
          if (m_filledEvent) return StatusCode::SUCCESS;

          try {
            ATH_MSG_DEBUG("Retrieved " << m_eventInfoKey);

            xAOD::FileMetaData::MetaDataType type = xAOD::FileMetaData::mcProcID;
            const float id = static_cast< float >(eventInfo->mcChannelNumber());

            if (m_info->setValue(type, id))
              ATH_MSG_DEBUG("setting " << type << " to " << id);
            else
              ATH_MSG_DEBUG("error setting " << type << " to " << id);
          } catch (std::exception&) {
            // Processing data not generated events
            ATH_MSG_DEBUG("Failed to set " << xAOD::FileMetaData::mcProcID);
          }
        } else {
          ATH_MSG_DEBUG(
            "Failed to retrieve " << m_eventInfoKey << " => cannot set "
            << xAOD::FileMetaData::mcProcID
            << ", runNumbers, or lumiBlockNumbers");
        }
      }

      // Return if object has already been filled
      if (m_filledEvent) return StatusCode::SUCCESS;

      {  // get dataType
        const DataHeader* dataHeader = nullptr;
        StatusCode sc = StatusCode::FAILURE;
        if (m_eventStore->contains< DataHeader >(m_dataHeaderKey)) {
          sc = m_eventStore->retrieve(dataHeader, m_dataHeaderKey);
          ATH_MSG_DEBUG("Retrieved " << m_dataHeaderKey);
        } else {
          ATH_MSG_DEBUG("No DataHeader with key: " << m_dataHeaderKey);
        }

        xAOD::FileMetaData::MetaDataType type = xAOD::FileMetaData::dataType;
        std::string tag = m_dataHeaderKey;
        if (dataHeader && sc.isSuccess()) tag = dataHeader->getProcessTag();

        try {
          if (m_info->setValue(type, tag))
            ATH_MSG_DEBUG("set " << type << " to " << tag);
          else
            ATH_MSG_DEBUG("error setting " << type << " to " << tag);
        } catch (std::exception&) {
          // This is unexpected
          ATH_MSG_DEBUG("Failed to set " << xAOD::FileMetaData::dataType);
        }
      }
      m_filledEvent = true;

      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::updateFromNonEvent() {

      // Have we already done this?
      if (m_filledNonEvent) return StatusCode::SUCCESS;

      // Sanity check
      if (!(m_info && m_aux)) {
        ATH_MSG_DEBUG("No xAOD::FileMetaData object to fill");
        return StatusCode::SUCCESS;
      }

      set(xAOD::FileMetaData::productionRelease,
          m_tagInfoMgr->findTag("AtlasRelease"));

      set(xAOD::FileMetaData::amiTag, m_tagInfoMgr->findTag("AMITag"));

      set(xAOD::FileMetaData::geometryVersion, m_tagInfoMgr->findTag("GeoAtlas"));

      set(xAOD::FileMetaData::conditionsTag,
          m_tagInfoMgr->findTag("IOVDbGlobalTag"));

      set(xAOD::FileMetaData::beamType, m_tagInfoMgr->findTag("beam_type"));

      std::string beamEnergy = m_tagInfoMgr->findTag("beam_energy");
      try {
        set(xAOD::FileMetaData::beamEnergy,
            std::stof(beamEnergy));
      } catch (std::invalid_argument& e) {
        ATH_MSG_DEBUG("beam energy \"" << beamEnergy << "\" tag could not be converted to float");
      } catch (std::out_of_range& e) {
        ATH_MSG_DEBUG("converted beam energy value (\"" << beamEnergy << "\") outside float range");
      }

      // Read simulation parameters
      const IOVMetaDataContainer * simInfo = nullptr;
      StatusCode sc = StatusCode::FAILURE;
      if (m_inputMetaDataStore->contains< IOVMetaDataContainer >(m_simInfoKey))
        sc = m_inputMetaDataStore->retrieve(simInfo, m_simInfoKey);
      const coral::AttributeList * attrList = nullptr;
      if (simInfo && sc.isSuccess())
        for (const CondAttrListCollection* payload : *simInfo->payloadContainer())
          for (const auto& itr : *payload)
            attrList = &(itr.second);
      if (attrList) {
        { // set simulation flavor
          std::string key = "SimulationFlavour";
          std::string value = "none";
          if (attrList->exists(key))
            value = (*attrList)[key].data< std::string >();

          // remap simulation flavor "default" to "FullSim"
          if (value == "default")
            value = "FullSim";

          set(xAOD::FileMetaData::simFlavour, value);
        }

        { // set whether this is overlay
          std::string key = "IsEventOverlayInputSim";
          std::string attr = "False";
          if (attrList->exists(key))
            attr = (*attrList)[key].data< std::string >();
          set(xAOD::FileMetaData::isDataOverlay, attr == "True");
        }

      } else {
        ATH_MSG_DEBUG(
            "Failed to retrieve " << m_simInfoKey << " => cannot set: "
            << xAOD::FileMetaData::simFlavour << ", and "
            << xAOD::FileMetaData::isDataOverlay);
      }

      // FileMetaData object has been filled with non event info
      m_filledNonEvent = true;

      return StatusCode::SUCCESS;
    }

void
    FileMetaDataCreatorTool::set(
            const xAOD::FileMetaData::MetaDataType key,
            bool value) {
      try {
        if (m_info->setValue(key, value))
          ATH_MSG_DEBUG("setting " << key << " to " << value);
        else
          ATH_MSG_DEBUG("error setting " << key << " to " << std::boolalpha << value
                        << std::noboolalpha);
      } catch (std::exception&) {
        // Processing data not generated events
        ATH_MSG_DEBUG("Failed to set " << key);
      }
    }

void
    FileMetaDataCreatorTool::set(
            const xAOD::FileMetaData::MetaDataType key,
            float value) {
      try {
        if (m_info->setValue(key, value))
          ATH_MSG_DEBUG("setting " << key << " to " << value);
        else
          ATH_MSG_DEBUG("error setting " << key << " to " << value);
      } catch (std::exception&) {
        // Processing data not generated events
        ATH_MSG_DEBUG("Failed to set " << key);
      }
    }

void
    FileMetaDataCreatorTool::set(
            const xAOD::FileMetaData::MetaDataType key,
            const std::string& value) {
      if (value.empty()) return;
      try {
        if (m_info->setValue(key, value))
          ATH_MSG_DEBUG("setting " << key << " to " << value);
        else
          ATH_MSG_DEBUG("error setting " << key << " to " << value);
      } catch (std::exception&) {
        // Processing data not generated events
        ATH_MSG_DEBUG("Failed to set " << key);
      }
    }

void FileMetaDataCreatorTool::addUniqueValue(
  std::string type, uint32_t value) {
  try {
    std::vector<uint32_t> list;
    if (m_info->value(type, list)) {
      ATH_MSG_DEBUG("retrieved existing list of " << type);
    } else {
      ATH_MSG_DEBUG("adding new list for " << type);
    }
    // we want a sorted list of unique values (without using std::set)
    std::sort(list.begin(), list.end());
    auto it = std::lower_bound(list.begin(), list.end(), value);
    if (it == list.end() || (*it) != value) {
      list.insert(it, value);
      ATH_MSG_DEBUG("added " << value << " to list of " << type);
    }
    if (!m_info->setValue(type, list)) {
      ATH_MSG_DEBUG("error updating list for " + type);
    }
  } catch (std::exception& e) {
    // Processing generated events not data
    ATH_MSG_WARNING(e.what());
  }
}

}  // namespace xAODMaker
