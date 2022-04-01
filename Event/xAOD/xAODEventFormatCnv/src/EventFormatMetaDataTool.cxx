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
      if (!m_inputKey.empty()) ATH_MSG_VERBOSE("  " << m_inputKey);

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

      std::vector< std::string > keys;
      if (m_inputKey.empty()) {
        m_inputMetaStore->keys<xAOD::EventFormat>(keys);
        // remove duplicates - expecting none
        std::sort(keys.begin(), keys.end());
        keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
      } else {
        // copy only the specified key
        keys.push_back(m_inputKey);
      }

      bool found = false;
      for (auto itr = keys.begin(); itr != keys.end();) {
        if (m_inputMetaStore->contains< xAOD::EventFormat >(*itr)) {
          found = true;
          ATH_MSG_VERBOSE("Found xAOD::EventFormat with key " << *itr);
          ++itr;
        } else {
          itr = keys.erase(itr);
        }
      }

      // If the input file doesn't have any event format metadata,
      // then finish right away:
      if (!found) return StatusCode::SUCCESS;

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
