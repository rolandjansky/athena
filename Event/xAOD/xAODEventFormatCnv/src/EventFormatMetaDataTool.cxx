// Dear emacs, this is -*- c++ -*-
/* Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration */


// System include(s):
#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// Local include(s):
#include "EventFormatMetaDataTool.h"

namespace xAODMaker {


EventFormatMetaDataTool::EventFormatMetaDataTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent):
    AthAlgTool(type, name, parent) {
      declareInterface< ::IMetaDataTool >(this);
    }

StatusCode
    EventFormatMetaDataTool::initialize() {
      // Greet the user:
      ATH_MSG_VERBOSE("Initialising");
      ATH_MSG_VERBOSE("  " << m_inputMetaStore);
      ATH_MSG_VERBOSE("  " << m_outputMetaStore);
      if (!m_keys.empty()) {
        ATH_MSG_VERBOSE("Asked to copy EventFormat with keys:");
        for (const std::string& key : m_keys) {
          ATH_MSG_VERBOSE("  - " << key);
        }
      }

      // Connect to the metadata stores:
      ATH_CHECK(m_inputMetaStore.retrieve());
      ATH_CHECK(m_outputMetaStore.retrieve());

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    EventFormatMetaDataTool::beginInputFile() {
      // lock the tool, so metaDataStop will wait for write to finish
      std::lock_guard< std::mutex > guard(m_outputMutex);

      // Create object to hold information from the new file
      auto format = std::make_unique< xAOD::EventFormat >();

      StatusCode sc = collectMetaData();
      if (sc.isFailure()) {
        REPORT_ERROR(sc) << "Failed to collect metadata from the input file";
        throw std::runtime_error("Failed to collect event format "
                                 "metadata from the input");
      }

      return StatusCode::SUCCESS;
    }

StatusCode
    EventFormatMetaDataTool::metaDataStop() {
      // wait for threads currenly writing to finish
      std::lock_guard< std::mutex > guard(m_outputMutex);

      return StatusCode::SUCCESS;
    }

StatusCode
    EventFormatMetaDataTool::collectMetaData() {

      std::vector< std::string > keys = m_keys;
      if (keys.empty()) {
        m_inputMetaStore->keys<xAOD::EventFormat>(keys);
      } else {
        // remove keys not in the InputMetaDataStore
        keys.erase(
            std::remove_if(
                keys.begin(), keys.end(),
                [this](std::string& key) {
                  return !m_inputMetaStore->contains<xAOD::EventFormat>(key);
                }),
            keys.end());
      }

      // If the input file doesn't have any event format metadata,
      // then finish right away:
      if (keys.empty()) return StatusCode::SUCCESS;

      // Retrieve the input container:
      for (const std::string& key : keys) {
        std::list<SG::ObjectWithVersion<xAOD::EventFormat> > allVersions;
        if (!m_inputMetaStore->retrieveAllVersions(allVersions, key)
                 .isSuccess()) {
          ATH_MSG_DEBUG("Failed to retrieve \""
                        << key << "\" from InputMetaDataStore");
          continue;  // try next key
        }

        // get output object
        auto output = m_outputMetaStore->tryRetrieve<xAOD::EventFormat>(key);
        if (!output) {
          auto ef = std::make_unique<xAOD::EventFormat>();
          output = ef.get();
          if (!m_outputMetaStore->record(std::move(ef), key).isSuccess()) {
            ATH_MSG_DEBUG("Failed to create output object \""
                          << key << "\" in MetaDataSvc");
            continue;
          }
        }

        ATH_MSG_VERBOSE("Merging all versions of " << key);
        for (SG::ObjectWithVersion<xAOD::EventFormat>& version : allVersions) {
          const xAOD::EventFormat* input = version.dataObject.cptr();
          // Merge the new object into the output one:
          for (auto itr = input->begin(); itr != input->end(); ++itr) {
            if (!output->exists(itr->second.hash())) {
              output->add(itr->second);
            }
          }
        }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

}  // namespace xAODMaker
