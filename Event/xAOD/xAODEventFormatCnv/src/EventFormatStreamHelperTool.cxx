/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */

// Local include(s).
#include "EventFormatStreamHelperTool.h"

// system includes
#include <memory>
#include <regex>
#include <utility>

// EDM include(s).
#include "xAODEventFormat/EventFormat.h"

namespace xAODMaker {

StatusCode
    EventFormatStreamHelperTool::initialize() {
      // Retrieve all needed components.
      ATH_CHECK(m_metadataStore.retrieve());

      // Return gracefully
      return StatusCode::SUCCESS;
    }

StatusCode
    EventFormatStreamHelperTool::postExecute() {
      // Notify the event format service that it should collect the metadata
      // that it needs.
      ATH_CHECK(collectFormatMetadata());
      ATH_MSG_VERBOSE("Triggered metadata collection on: " << m_key.value());

      // Return gracefully.
      return StatusCode::SUCCESS;
    }

StatusCode
    EventFormatStreamHelperTool::collectFormatMetadata() {
      // Get the EventFormat object
      xAOD::EventFormat * event_format =
          m_metadataStore->tryRetrieve< xAOD::EventFormat >(m_key.value());

      if (!event_format) {
        auto p_event_format = std::make_unique< xAOD::EventFormat >();
        event_format = p_event_format.get();
        ATH_CHECK(m_metadataStore->record(std::move(p_event_format), m_key));
        ATH_MSG_VERBOSE("Created new xAOD::EventFormat: " << m_key.value());
      } else {
        ATH_MSG_VERBOSE("Use existing xAOD::EventFormat: " << m_key.value());
      }

      // Ask StoreGate for all the objects that it's holding on to.
      const std::vector< const SG::DataProxy* > proxies = evtStore()->proxies();

      // Loop over the proxies.
      for (const SG::DataProxy* proxy : proxies) {
        // Construct the branch name that Athena I/O is going to give to
        // this object.
        const std::string branchName = proxy->name();

        // Skip objects that were set up to be ignored.
        {
          bool ignoreObject = false;
          for (const std::string& ignorePattern : m_ignoreKeys.value()) {
            if (std::regex_match(branchName, std::regex(ignorePattern))) {
              ignoreObject = true;
              break;
            }
          }
          if (ignoreObject == true) continue;
        }

        // Get the type name of this object.
        std::string typeName;
        if (m_clidSvc->getTypeInfoNameOfID(proxy->clID(), typeName).isFailure()) {
          // Make sure that nobody else is using @c m_warnedCLIDs right now.
          std::lock_guard< std::mutex > lock(m_warnedCLIDsMutex);

          // Print a warning if this CLID didn't produce a warning yet:
          if (m_warnedCLIDs.insert(proxy->clID()).second)
            ATH_MSG_WARNING("Couldn't get type name for CLID = " << proxy->clID() );

          continue;
        }

        // Now that we have the type name, check whether metadata for this type
        // should be stored.
        {
          bool ignoreObject = true;
          for (const std::string& typePattern : m_typeNames.value()) {
            if (std::regex_match(typeName, std::regex(typePattern))) {
              ignoreObject = false;
              break;
            }
          }
          if (ignoreObject) continue;
        }

        // Update the metadata object.
        uint32_t hash = 0;
        {
          // Make sure that nobody else is modifying @c m_ef or @c m_spool
          // right now.
          std::lock_guard< std::mutex > lock(m_efMutex);
          // If we already know about this object, then don't bother.
          if (event_format->exists(branchName)) continue;

          // Get the hash for this name.
          hash = evtStore()->stringToKey(branchName, proxy->clID());
          // Add the info.
          event_format->add(xAOD::EventFormatElement(branchName, typeName, "", hash));
        }

        // Tell the user what happened.
        ATH_MSG_VERBOSE("Adding info: branchName = \"" << branchName
                        << ", typeName = \"" << typeName << "\""
                        << ", hash = 0x" << std::hex << std::setw(8)
                        << std::setfill('0') << hash);
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
    }

}  // namespace xAODMaker
