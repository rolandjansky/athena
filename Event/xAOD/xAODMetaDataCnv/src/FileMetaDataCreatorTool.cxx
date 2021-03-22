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


inline StatusCode toStatusCode(bool b) {
  if (b)
    return StatusCode::SUCCESS;
  else
    return StatusCode::FAILURE;
}

namespace xAODMaker {

StatusCode
    FileMetaDataCreatorTool::initialize() {
      // Greet the user:
      ATH_MSG_DEBUG("Initialising - Package version: " << PACKAGE_VERSION);

      ATH_CHECK(m_eventStore.retrieve());
      ATH_CHECK(m_metaDataSvc.retrieve());
      ATH_CHECK(m_inputMetaDataStore.retrieve());

      // If DataHeader key not specified, try determining it
      if (m_dataHeaderKey.empty()) {
        const auto *parentAlg = dynamic_cast< const INamedInterface* >(parent());
        if (parentAlg)
          m_dataHeaderKey = parentAlg->name();
      }

      // Listen for the begin of an input file. Act after MetaDataSvc, which
      // has priority 80. That means the FileMetaDataTool be called first
      ServiceHandle< IIncidentSvc > incidentSvc("IncidentSvc", name());
      ATH_CHECK(incidentSvc.retrieve());
      incidentSvc->addListener(this, "BeginInputFile", 70);

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
      if (inc.type() == "BeginInputFile")
        if (!updateFromNonEvent().isSuccess())
          ATH_MSG_DEBUG("Failed to fill FileMetaData with non-event info");
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

      // Return if object has already been filled
      if (m_filledEvent) return StatusCode::SUCCESS;

      // Sanity check
      if (!(m_info && m_aux)) {
        ATH_MSG_ERROR("No xAOD::FileMetaData object to fill");
        return StatusCode::FAILURE;
      }

      // Read xAOD event info
      const xAOD::EventInfo * eventInfo = nullptr;
      StatusCode sc = StatusCode::FAILURE;
      if (m_eventStore->contains< xAOD::EventInfo >(m_eventInfoKey))
        sc = m_eventStore->retrieve(eventInfo, m_eventInfoKey);
      else if (m_eventStore->contains< xAOD::EventInfo >("Mc" + m_eventInfoKey))
        sc = m_eventStore->retrieve(eventInfo, "Mc" + m_eventInfoKey);
      if (eventInfo && sc.isSuccess()) {
        try {
          ATH_MSG_DEBUG("Retrieved " << m_eventInfoKey);

          xAOD::FileMetaData::MetaDataType type = xAOD::FileMetaData::mcProcID;
          const float id = static_cast< float >(eventInfo->mcChannelNumber());

          if (m_info->setValue(type, id))
            ATH_MSG_DEBUG("setting " << type << " to "<< id);
          else
            ATH_MSG_INFO("error setting " << type << " to "<< id);
        } catch (std::exception&) {
          // Processing data not generated events
          ATH_MSG_DEBUG("Failed to set " << xAOD::FileMetaData::mcProcID);
        }
      } else {
        ATH_MSG_DEBUG("Failed to retrieve " << m_eventInfoKey
                     << " => cannot set " << xAOD::FileMetaData::mcProcID);
      }

      // Read data header
      const DataHeader * dataHeader = nullptr;
      if (m_eventStore->contains< DataHeader >(m_dataHeaderKey))
        sc = m_eventStore->retrieve(dataHeader, m_dataHeaderKey);
      if (dataHeader && sc.isSuccess()) {
        try {
          ATH_MSG_DEBUG("Retrieved " << m_dataHeaderKey);

          xAOD::FileMetaData::MetaDataType type = xAOD::FileMetaData::dataType;
          const std::string& tag = dataHeader->getProcessTag();

          if (m_info->setValue(type, tag))
            ATH_MSG_DEBUG("set " << type << " to "<< tag);
          else
            ATH_MSG_INFO("error setting " << type << " to "<< tag);
        } catch (std::exception&) {
          // This is unexpected
          ATH_MSG_WARNING("Failed to set " << xAOD::FileMetaData::dataType);
        }
      } else {
        ATH_MSG_DEBUG("Failed to retrieve " << m_dataHeaderKey
                     << " => cannot set " << xAOD::FileMetaData::dataType);
      }

      m_filledEvent = true;

      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::updateFromNonEvent() {
      // Lock the tool while we work on the FileMetaData
      std::lock_guard lock(m_toolMutex);

      // Have we already done this?
      if (m_filledNonEvent) return StatusCode::SUCCESS;

      // Sanity check
      if (!(m_info && m_aux)) {
        ATH_MSG_ERROR("No xAOD::FileMetaData object to fill");
        return StatusCode::FAILURE;
      }

      // Read TagInfo
      const IOVMetaDataContainer * tagInfo = nullptr;
      StatusCode sc = StatusCode::FAILURE;
      if (m_inputMetaDataStore->contains< IOVMetaDataContainer >(m_tagInfoKey))
        sc = m_inputMetaDataStore->retrieve(tagInfo, m_tagInfoKey);
      if (tagInfo && sc.isSuccess()) {
        for (const auto* payload : *tagInfo->payloadContainer()) {
          for (const auto& itr : *payload) {
            const coral::AttributeList& attributeList = itr.second;
            ATH_CHECK(
                setString(
                    attributeList,
                    "AtlasRelease",
                    xAOD::FileMetaData::productionRelease));

            ATH_CHECK(
                setString(
                    attributeList,
                    "AMITag",
                    xAOD::FileMetaData::amiTag));

            ATH_CHECK(
                setString(
                    attributeList,
                    "GeoAtlas",
                    xAOD::FileMetaData::geometryVersion));

            ATH_CHECK(
                setString(
                    attributeList,
                    "IOVDbGlobalTag",
                    xAOD::FileMetaData::conditionsTag));

            ATH_CHECK(
                setFloat(
                    attributeList,
                    "beam_energy",
                    xAOD::FileMetaData::beamEnergy));

            ATH_CHECK(
                setString(
                    attributeList,
                    "beam_type",
                    xAOD::FileMetaData::beamType));

            // only investigate the first IOV
            break;
          }
          // only investigate the first payload in the container
          break;
        }
      } else {
        ATH_MSG_DEBUG(
            "Failed to retrieve " << m_tagInfoKey << " => cannot set:\n\t"
            << xAOD::FileMetaData::productionRelease << ",\n\t"
            << xAOD::FileMetaData::amiTag << ",\n\t"
            << xAOD::FileMetaData::geometryVersion << ",\n\t"
            << xAOD::FileMetaData::conditionsTag << ",\n\t"
            << xAOD::FileMetaData::beamEnergy << ", and\n\t"
            << xAOD::FileMetaData::beamType);
      }

      // Read simulation parameters
      const IOVMetaDataContainer * simInfo = nullptr;
      if (m_inputMetaDataStore->contains< IOVMetaDataContainer >(m_simInfoKey))
        sc = m_inputMetaDataStore->retrieve(simInfo, m_simInfoKey);
      if (simInfo && sc.isSuccess()) {
        for (const CondAttrListCollection* payload : *simInfo->payloadContainer()) {
          for (const auto& itr : *payload) {
            const coral::AttributeList& attributeList = itr.second;

            ATH_CHECK(
                setString(
                    attributeList,
                    "SimulationFlavour",
                    xAOD::FileMetaData::simFlavour));

            ATH_CHECK(
                setBool(
                    attributeList,
                    "IsEventOverlayInputSim",
                    xAOD::FileMetaData::isDataOverlay));

            break;  // only investigate first IOV
          }
          break;  // only investigate first payload
        }
      } else {
        ATH_MSG_DEBUG("Failed to retrieve " << m_simInfoKey << " => cannot set: "
                     << xAOD::FileMetaData::simFlavour << ", and "
                     << xAOD::FileMetaData::isDataOverlay);
      }

      // FileMetaData object has been filled with non event info
      m_filledNonEvent = true;

      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::setString(
        const coral::AttributeList& attrList,
        const std::string& tag,
        const xAOD::FileMetaData::MetaDataType type) {
      try {
        std::string attr = "none";
        if (attrList.exists(tag)) {
          attr = attrList[tag].data< std::string >();
          // remap simulation flavor "default" to "FullSim"
          if (type == xAOD::FileMetaData::simFlavour && attr == "default")
            attr = "FullSim";
        }
        ATH_MSG_DEBUG("Setting " << type << " to \"" << attr << "\"");
        return toStatusCode(m_info->setValue(type, attr));
      } catch (std::exception&) {
        ATH_MSG_ERROR("unexpected error building FileMetaData");
        return StatusCode::FAILURE;
      }
    }

StatusCode
    FileMetaDataCreatorTool::setFloat(
        const coral::AttributeList& attrList,
        const std::string& tag,
        const xAOD::FileMetaData::MetaDataType type) {
      try {
        float number = -1.0f;
        if (attrList.exists(tag)) {
          try {
            const std::string attr = attrList[tag].data< std::string >();
            number = std::stof(attr);
          } catch (std::invalid_argument& e) {
            ATH_MSG_INFO("beam energy tag could not be converted to float");
          } catch (std::out_of_range& e) {
            ATH_MSG_INFO("converted beam energy value outside float range");
          }
        }
        ATH_MSG_DEBUG("Setting " << type << " to \"" << number << "\"");
        return toStatusCode(m_info->setValue(type, number));
      } catch (std::exception&) {
        ATH_MSG_ERROR("unexpected error building FileMetaData");
        return StatusCode::FAILURE;
      }
    }

StatusCode
    FileMetaDataCreatorTool::setBool(
        const coral::AttributeList& attrList,
        const std::string& tag,
        const xAOD::FileMetaData::MetaDataType type) {
      try {
        bool yesNo = false;
        if (attrList.exists(tag)) {
          yesNo = attrList[tag].data< std::string >() == "True";
        }

        ATH_MSG_DEBUG("Setting " << type << " to " << std::boolalpha << yesNo
                      << std::noboolalpha);
        return toStatusCode(m_info->setValue(type, yesNo));
      } catch (std::exception&) {
        ATH_MSG_ERROR("unexpected error building FileMetaData");
        return StatusCode::FAILURE;
      }
    }

}  // namespace xAODMaker
