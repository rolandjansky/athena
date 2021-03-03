/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */

// Local include(s).
#include "EventFormatStreamHelperTool.h"

// system includes
#include <iterator>
#include <memory>
#include <regex>
#include <utility>

#include "GaudiKernel/Algorithm.h"
#include "PersistentDataModel/DataHeader.h"

// EDM include(s).
#include "xAODEventFormat/EventFormat.h"

namespace xAODMaker {

StatusCode
    EventFormatStreamHelperTool::initialize() {
      // Retrieve all needed components.
      ATH_CHECK(m_metadataStore.retrieve());

      if (m_dataHeaderKey.empty()) {
        // find out name of stream we are working for
        const Gaudi::Algorithm *parentAlg =
            dynamic_cast< const Gaudi::Algorithm* >(parent());
        if (parentAlg) m_dataHeaderKey = parentAlg->name();
      }

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

      // Grab output stream data header
      SG::ReadHandle< DataHeader > dataHeader(m_dataHeaderKey);

      // Loop over objects in output stream
      for (const DataHeaderElement& elem : *dataHeader) {
        // Caveat: assume branch name that Athena I/O is going to give to
        // this object is the key name
        const std::string key = elem.getKey();
        const CLID classID = elem.getPrimaryClassID();

        // Skip objects that were set up to be ignored.
        {
          bool ignoreObject = false;
          for (const std::string& ignorePattern : m_ignoreKeys.value()) {
            if (std::regex_match(key, std::regex(ignorePattern))) {
              ignoreObject = true;
              break;
            }
          }
          if (ignoreObject == true) continue;
        }

        // Get the type name of this object.
        std::string typeName;
        if (m_clidSvc->getTypeInfoNameOfID(classID, typeName).isFailure()) {
          // Make sure that nobody else is using @c m_warnedCLIDs right now.
          std::lock_guard< std::mutex > lock(m_warnedCLIDsMutex);

          // Print a warning if this CLID didn't produce a warning yet:
          if (m_warnedCLIDs.insert(classID).second)
            ATH_MSG_WARNING("Couldn't get type name for CLID = " << classID );

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

        // Update the metadata object
        try {
          const unsigned int hash =
              lookUpHash(classID, elem.getClassIDs(), elem.getHashes());

          // Make sure that nobody else is modifying @c m_ef or @c m_spool
          // right now.
          std::lock_guard< std::mutex > lock(m_efMutex);
          // If we already know about this object, then don't bother.
          if (event_format->exists(key)) continue;

          // Add the info.
          event_format->add(xAOD::EventFormatElement(key, typeName, "", hash));

          // Tell the user what happened.
          ATH_MSG_VERBOSE("Adding info: key = \"" << key
                        << ", typeName = \"" << typeName << "\""
                        << ", hash = 0x" << std::hex << std::setw(8)
                        << std::setfill('0') << hash);
        } catch (const std::exception& e) {
          ATH_MSG_WARNING(e.what());
        }
      }

      // Return gracefully.
      return StatusCode::SUCCESS;
    }

unsigned int
    EventFormatStreamHelperTool::lookUpHash(
      CLID primaryClassID,
      const std::set<CLID>& classIDs,
      const std::vector<unsigned int>& hashes) const {
        // two collections of different size would cause undefined behaviour
        if (classIDs.size() != hashes.size())
          throw(std::runtime_error("CLID and hash sets not equal in size"));

        auto it = classIDs.find(primaryClassID);

        // bail if we don't find the primary class ID
        if (it == classIDs.end())
          throw(std::range_error("Primary class ID not in list of class IDs"));

        return hashes[std::distance(classIDs.begin(), it)];
      }

}  // namespace xAODMaker
