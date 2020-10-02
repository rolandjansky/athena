// Dear emacs, this is -*- c++ -*-
/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */

// $Id: EventFormatMetaDataTool.cxx 651874 2015-03-05 14:16:19Z krasznaa $

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
      ATH_MSG_DEBUG("Initialising - Package version: " << PACKAGE_VERSION);
      ATH_MSG_DEBUG("  " << m_inputMetaStore);
      ATH_MSG_DEBUG("  " << m_outputMetaStore);
      ATH_MSG_DEBUG("  " << m_outputKey);
      if (!m_inputKey.empty()) ATH_MSG_DEBUG("  " << m_inputKey);

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

      StatusCode sc = collectMetaData(format);
      if (sc.isFailure()) {
        REPORT_ERROR(sc) << "Failed to collect metadata from the input file";
        throw std::runtime_error("Failed to collect event format "
                                 "metadata from the input");
      }

      ATH_CHECK(writeMetaData(format));

      return StatusCode::SUCCESS;
    }

StatusCode
    EventFormatMetaDataTool::metaDataStop() {
      // wait for threads currenly writing to finish
      std::lock_guard< std::mutex > guard(m_outputMutex);

      return StatusCode::SUCCESS;
    }

StatusCode
    EventFormatMetaDataTool::collectMetaData(
        std::unique_ptr< xAOD::EventFormat >& format) {

      std::vector< std::string > keys;
      m_inputMetaStore->keys< xAOD::EventFormat >(keys);
      if (!m_inputKey.empty()) keys.push_back(m_inputKey);

      // remove duplicates
      std::sort(keys.begin(), keys.end());
      keys.erase(std::unique(keys.begin(), keys.end()), keys.end());

      bool found = false;
      for (auto itr = keys.begin(); itr != keys.end();) {
        if (m_inputMetaStore->contains< xAOD::EventFormat >(*itr)) {
          found = true;
          ATH_MSG_DEBUG("Found xAOD::EventFormat with key " << *itr);
          ++itr;
        } else {
          itr = keys.erase(itr);
        }
      }

      // If the input file doesn't have any event format metadata,
      // then finish right away:
      if (!found) return StatusCode::SUCCESS;

      // Retrieve the input container:
      const xAOD::EventFormat* input = nullptr;
      for (const std::string& key : keys) {
        ATH_CHECK(m_inputMetaStore->retrieve(input, key));

        // Merge the new object into the output one:
        for (auto itr = input->begin(); itr != input->end(); ++itr) {
          if (!format->exists(itr->second.hash())) {
            format->add(itr->second);
          }
        }
      }

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

StatusCode
    EventFormatMetaDataTool::writeMetaData(
        std::unique_ptr< xAOD::EventFormat >& format) {

      // The output may already have such an object, coming from some
      // other source. Like the event format service.

      xAOD::EventFormat * output =
          m_outputMetaStore->tryRetrieve< xAOD::EventFormat >(m_outputKey);

      if (output == nullptr) {

        // Record the event format metadata, if any was found in the
        // processed input files.
        ATH_MSG_DEBUG("Recording event format metadata");
        ATH_CHECK(m_outputMetaStore->record(std::move(format), m_outputKey));

      } else {

        ATH_MSG_DEBUG("updating existing xAOD::EventFormat object");
        // Update the object in the MetaDataStore with new content
        for (auto itr = format->begin(); itr != format->end(); ++itr)
          if (!output->exists(itr->second.hash()))
            output->add(itr->second);

      }

      // Return gracefully:
      return StatusCode::SUCCESS;
    }

}  // namespace xAODMaker
