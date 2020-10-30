/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// System include(s):
#include <algorithm>
#include <sstream>

// Athena metadata EDM:
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "EventInfo/EventStreamInfo.h"

// Local include(s):
#include "FileMetaDataCreatorTool.h"

namespace xAODMaker {

/// The folder in DetectorStore that we get the information from
static const char FOLDER_NAME[] = "/TagInfo";
static const char SIMFOLDER_NAME[] = "/Simulation/Parameters";

/// Helper macro to execute the setter functions correctly
#define CHECK_BOOL(EXP)                                      \
do {                                                         \
  try {                                                      \
    if (!EXP) {                                              \
      ATH_MSG_ERROR("Failed to execute: " << #EXP);          \
      return StatusCode::FAILURE;                            \
    }                                                        \
  } catch(const std::exception& ex) {                        \
    ATH_MSG_INFO("Couldn't extract attribute with command: " \
                 << #EXP);                                   \
  }                                                          \
} while (0)

FileMetaDataCreatorTool::FileMetaDataCreatorTool(const std::string& name)
  : asg::AsgMetadataTool(name)
  , m_md()
  , m_mdAux() {
    declareProperty("OutputKey", m_outputKey = "FileMetaData");
  }

StatusCode
    FileMetaDataCreatorTool::start() {
      const DataHandle< CondAttrListCollection > dummy;
      StatusCode sc = detStore()->regFcn(
          &xAODMaker::FileMetaDataCreatorTool::update,
          this,
          dummy,
          FOLDER_NAME,
          true);

      if (sc.isFailure()) {
        ATH_MSG_WARNING("Unable to register callback for " << FOLDER_NAME);
        // return StatusCode::FAILURE;
      }

      if (detStore()->contains< AthenaAttributeList >(SIMFOLDER_NAME)) {
        const DataHandle< AthenaAttributeList > dummy2;
        sc = detStore()->regFcn(
            &xAODMaker::FileMetaDataCreatorTool::update,
            this,
            dummy2,
            SIMFOLDER_NAME,
            true);

        if (sc.isFailure()) {
          ATH_MSG_WARNING("Unable to register callback for " << SIMFOLDER_NAME);
          // return StatusCode::FAILURE;
        }
      } else {
        ATH_MSG_INFO("Processing file with no " << SIMFOLDER_NAME);
      }
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::initialize() {
      // Greet the user:
      ATH_MSG_DEBUG("Initialising - Package version: " << PACKAGE_VERSION);
      ATH_MSG_DEBUG("  OutputKey = " << m_outputKey);

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::beginInputFile() {
      // Create the output objects if they don't exist yet:
      if ((!m_md.get()) && (!m_mdAux.get())) {
        ATH_MSG_DEBUG("Creating output objects");
        m_md.reset(new xAOD::FileMetaData());
        m_mdAux.reset(new xAOD::FileMetaDataAuxInfo());
        m_md->setStore(m_mdAux.get());
      }

      ATH_MSG_DEBUG("DetectorStore contents:\n\n" << detStore()->dump());

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::endInputFile() {
      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::metaDataStop() {
      // Don't be offended if the metadata already exists in the output:
      if (outputMetaStore()->contains< xAOD::FileMetaData >(m_outputKey)) {
        ATH_MSG_DEBUG("xAOD::FileMetaData already in the output");
        return StatusCode::SUCCESS;
      }

      SG::ConstIterator<EventStreamInfo> esi_h1;
      SG::ConstIterator<EventStreamInfo> esi_h2;
      StatusCode sc = outputMetaStore()->retrieve(esi_h1, esi_h2);
      if (sc.isSuccess() && esi_h1 != esi_h2) {
        if (esi_h1->getProcessingTags().size() == 1) {
          CHECK_BOOL(m_md->setValue(
                  xAOD::FileMetaData::dataType,
                  *(esi_h1->getProcessingTags().begin())));
        } else {
          ATH_MSG_WARNING("Found multiple output process tags");
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::dataType, "multiple"));
        }
        if (esi_h1->getEventTypes().size() == 1) {
          float id = esi_h1->getEventTypes().begin()->mc_channel_number();
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::mcProcID, id));
        } else {
          ATH_MSG_WARNING("Found multiple eventtypes");
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::mcProcID, 0.0f));
        }
      }

      // Record the metadata, if any was found on the input:
      if (m_md.get() && m_mdAux.get()) {
        ATH_MSG_DEBUG("Recoding file level metadata");
        ATH_CHECK(outputMetaStore()->record(m_md.release(), m_outputKey));
        ATH_CHECK(outputMetaStore()->record(m_mdAux.release(),
                                            m_outputKey + "Aux."));
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataCreatorTool::update(IOVSVC_CALLBACK_ARGS_K(keys)) {
      ATH_MSG_DEBUG("update() invoked for key(s): " << keys);

      // Bail if the requested folder is not amongst the keys:
      if (std::find(keys.begin(), keys.end(), FOLDER_NAME) == keys.end()
          && std::find(keys.begin(), keys.end(), SIMFOLDER_NAME) == keys.end()) {
        ATH_MSG_WARNING("Update callback received without the \""
                        << FOLDER_NAME << "\" key");
        return StatusCode::SUCCESS;
      }

      // Retrieve the needed metadata:
      if (std::find(keys.begin(), keys.end(), FOLDER_NAME) != keys.end()) {
        const CondAttrListCollection* tagInfo = 0;
        ATH_CHECK(detStore()->retrieve(tagInfo, FOLDER_NAME));

        ATH_MSG_DEBUG("Retrieved object: " << FOLDER_NAME);

        // Make sure that it has some the expected size:
        if (tagInfo->size() > 1) {
          ATH_MSG_WARNING("Collection with >1 size received for \""
                          << FOLDER_NAME << "\"");
        }
        if (!tagInfo->size()) {
          ATH_MSG_ERROR("Empty collection received for \""
                        << FOLDER_NAME << "\"");
          return StatusCode::FAILURE;
        }
        // Access the first, and only channel of the object:
        const CondAttrListCollection::AttributeList& al =
            tagInfo->attributeList(0);

        if (al.exists("AtlasRelease")) {
          CHECK_BOOL(m_md->setValue(
                  xAOD::FileMetaData::productionRelease,
                  al[ "AtlasRelease" ].data< std::string >()));
        } else {
          ATH_MSG_WARNING("Did not find AtlasRelease in TagInfo setting to none");
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::productionRelease, "none"));
        }

        if (al.exists("AMITag")) {
          CHECK_BOOL(m_md->setValue(
                  xAOD::FileMetaData::amiTag,
                  al["AMITag"].data< std::string >()));
        } else {
          ATH_MSG_WARNING("Did not find AMITag in TagInfo setting to none");
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::amiTag, "none"));
        }

        if (al.exists("GeoAtlas")) {
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::geometryVersion,
                                    al["GeoAtlas"].data< std::string >()));
        } else {
          ATH_MSG_WARNING("Did not find GeoAtlas in TagInfo setting to none");
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::geometryVersion, "none"));
        }

        if (al.exists("IOVDbGlobalTag")) {
          CHECK_BOOL(m_md->setValue(
                  xAOD::FileMetaData::conditionsTag,
                  al["IOVDbGlobalTag"].data< std::string >()));
        } else {
          ATH_MSG_WARNING("Did not find IOVDbGlobalTag in TagInfo setting to none");
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::conditionsTag, "none"));
        }

        // Convert the beam energy to float. (If it exists.)
        if (al.exists("beam_energy")) {
          try {
            const std::string beamEnergyStr = al["beam_energy"].data< std::string >();
            char* endptr = 0;
            float beamEnergy = strtof(beamEnergyStr.c_str(), &endptr);
            if (endptr) {
              if (!m_md->setValue(xAOD::FileMetaData::beamEnergy, beamEnergy)) {
                ATH_MSG_ERROR("Failed to set the beam energy");
                return StatusCode::FAILURE;
              }
            }
          } catch(const std::exception& ex) {
            ATH_MSG_INFO("Couldn't set the beam energy");
          }
        } else {
          ATH_MSG_WARNING("Did not find beam_energy in TagInfo setting to -1");
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::beamEnergy, -1.0f));
        }

        if (al.exists("beam_type")) {
          CHECK_BOOL(m_md->setValue(
                  xAOD::FileMetaData::beamType,
                  al["beam_type"].data< std::string >()));
        } else {
          ATH_MSG_WARNING("Did not find beam_type in TagInfo setting to none");
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::beamType, "none"));
        }
      }

      if (std::find(keys.begin(), keys.end(), SIMFOLDER_NAME) != keys.end()) {
        const AthenaAttributeList* simInfo = 0;
        ATH_CHECK(detStore()->retrieve(simInfo, SIMFOLDER_NAME));

        if (simInfo->exists("SimulationFlavour")) {
          std::string flavor = (*simInfo)["SimulationFlavour"].data< std::string >();
          if (flavor == "default") flavor = "FullSim";
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::simFlavour, flavor));
        } else {
          ATH_MSG_ERROR("Unable to retrieve SimulationFlavour from " << SIMFOLDER_NAME);
        }

        if (simInfo->exists("IsEventOverlayInputSim")) {
          bool isDataOverlay = (*simInfo)["IsEventOverlayInputSim"].data< std::string >() == "True";
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::isDataOverlay, isDataOverlay));
        } else {
          ATH_MSG_INFO("Unable to retrieve IsEventOverlayInputSim from "
                       << SIMFOLDER_NAME << " - assuming not data overlay");
          CHECK_BOOL(m_md->setValue(xAOD::FileMetaData::isDataOverlay, false));
        }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

}  // namespace xAODMaker
