/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
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

      ATH_CHECK(m_tagInfoKey.initialize());
      ATH_CHECK(m_simInfoKey.initialize());

      // If DataHeader key not specified, try determining it
      if (m_dataHeaderKey.empty()) {
        auto parentAlg = dynamic_cast< const INamedInterface* >(parent());
        if (parentAlg)
          m_dataHeaderKey = parentAlg->name();
      }

      // Listen for the begin of an input file. Act after MetaDataSvc, which
      // has priority 80. That means the FileMetaDataTool be called first
      ServiceHandle< IIncidentSvc > incidentSvc("IncidentSvc", name());
      ATH_CHECK(incidentSvc.retrieve());
      incidentSvc->addListener(this, "BeginInputFile", 70);

      // Create a fresh object to fill
      ATH_MSG_DEBUG("Creating new xAOD::FileMetaData object to output");
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
      if (inc.type() == "BeginInputFile") {
        if (!updateFromNonEvent().isSuccess()) {
          ATH_MSG_INFO("Failed to fill FileMetaData with non-event info");
        }
      }
    }

StatusCode
    FileMetaDataCreatorTool::finalize() {
      // Nothing to do here
      return StatusCode::SUCCESS;
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
      auto eventInfo =
          m_eventStore->tryConstRetrieve< xAOD::EventInfo >(m_eventInfoKey);
      if (eventInfo) {
        try {
          ATH_MSG_DEBUG("Valid " << m_eventInfoKey << " handle");

          xAOD::FileMetaData::MetaDataType type = xAOD::FileMetaData::mcProcID;
          const float id = static_cast< float >(eventInfo->mcChannelNumber());

          if (m_info->setValue(type, id))
            ATH_MSG_DEBUG("setting " << type << " to "<< id);
          else
            ATH_MSG_WARNING("error setting " << type << " to "<< id);
        } catch (std::exception&) {
          // Probably processing data
          ATH_MSG_INFO("No xAOD::FileMetaData:mcProcID update from "
                       << m_eventInfoKey);
        }
      } else {
        ATH_MSG_WARNING("No " << m_eventInfoKey);
      }

      // Read data header
      auto dataHeader =
          m_eventStore->tryConstRetrieve< DataHeader >(m_dataHeaderKey);
      if (dataHeader) {
        try {
          ATH_MSG_DEBUG("valid " << m_dataHeaderKey << " handle");

          xAOD::FileMetaData::MetaDataType type = xAOD::FileMetaData::dataType;
          const std::string tag = dataHeader->getProcessTag();

          if (m_info->setValue(type, tag))
            ATH_MSG_DEBUG("set " << type << " to "<< tag);
          else
            ATH_MSG_WARNING("error setting " << type << " to "<< tag);
        } catch (std::exception&) {
          ATH_MSG_INFO("No xAOD::FileMetaData::dataType update from "
                       << m_dataHeaderKey);
        }
      } else {
        ATH_MSG_WARNING("No " << m_dataHeaderKey);
      }

      m_filledEvent = true;

      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::preFinalize() {
      std::lock_guard lock(m_toolMutex);

      // Create and object if there isn't one already present
      if (!m_metaDataSvc->contains< xAOD::FileMetaData >(m_key)) {
        ATH_MSG_DEBUG("Creating new xAOD::FileMetaData object to output");
        auto info = std::make_unique< xAOD::FileMetaData >();
        auto aux  = std::make_unique< xAOD::FileMetaDataAuxInfo >();
        info->setStore(aux.get());
        ATH_CHECK(m_metaDataSvc->record(std::move(info), m_key));
        ATH_CHECK(m_metaDataSvc->record(std::move(aux), m_key + "Aux."));
      }

      // Replace content in store with content created for this stream
      auto output = m_metaDataSvc->tryRetrieve< xAOD::FileMetaData >(m_key);
      if (!output) {
        ATH_MSG_DEBUG("Cannot fill xAOD::FileMetaData output object");
        return StatusCode::FAILURE;
      }
      *output = *m_info;

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
      auto tagInfo = SG::makeHandle(m_tagInfoKey);
      if (!tagInfo.isValid()) {
        ATH_MSG_INFO("Invalid \"" << m_tagInfoKey.key() << "\" handle");
        return StatusCode::SUCCESS;
      }

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

      // Read simulation parameters
      SG::ReadHandle< IOVMetaDataContainer > simInfo(m_simInfoKey);
      // Bail if the requested folder is not amongst the keys:
      if (!simInfo.isValid()) {
        ATH_MSG_INFO("Invalid \"" << m_simInfoKey.key() << "\" handle");
        return StatusCode::SUCCESS;
      }

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

          // only investigate first IOV
          break;
        }
        // only investigate first payload
        break;
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
