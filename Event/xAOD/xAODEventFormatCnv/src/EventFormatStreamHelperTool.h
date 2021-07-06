// Dear emacs, this is -*- c++ -*-
/* Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration */
#ifndef XAODEVENTFORMATCNV_EVENTFORMATSTREAMHELPERTOOL
#define XAODEVENTFORMATCNV_EVENTFORMATSTREAMHELPERTOOL

#include <mutex>
#include <set>
#include <string>
#include <vector>

// Gaudi/Athena include(s).
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthenaOutputTool.h"
#include "AthenaKernel/IMetaDataSvc.h"

namespace xAODMaker {
/// Tool creating and maintaining @c xAOD::EventFormat at the end-of-events
///
/// This tool is meant to be added to every xAOD output stream, so that it
/// would maintain @c xAOD::EventFormat object every time a new event is
/// written out.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
/// @author Frank Berghaus <fberghaus@anl.gov>
///
class EventFormatStreamHelperTool : public extends< AthAlgTool, IAthenaOutputTool > {
 public:
  // Inherit the base class's constructor(s).
  using extends::extends;

  /// @name Interface inherited from @c AthAlgTool
  /// @{

  /// Initialise the tool
  StatusCode initialize() override;

  /// @}

  /// @name Interface inherited from @c IAthenaOutputTool
  /// @{

  /// Called at the end of initialize
  StatusCode postInitialize() override {return StatusCode::SUCCESS;}

  /// Called at the beginning of execute
  StatusCode preExecute() override {return StatusCode::SUCCESS;}

  /// Called at the end of execute
  StatusCode postExecute() override;

  /// Called at the beginning of finalize
  StatusCode preFinalize() override {return StatusCode::SUCCESS;}

  /// Called at the
  StatusCode preStream() override {return StatusCode::SUCCESS;}

  /// @}

 private:
  /// Use the metadata tool interface to store the EventFormat object
  ServiceHandle< IMetaDataSvc > m_metadataStore{ this, "MetaDataSvc",
    "MetaDataSvc", "The metadata service use to record the xAOD::EventFormat" };

  /// Connection to the CLID service
  ServiceHandle< IClassIDSvc > m_clidSvc{ this, "ClassIDSvc", "ClassIDSvc",
    "The ClassID service instance to use" };

  Gaudi::Property< std::string > m_key{ this, "Key", "EventInfo",
    "Key for EventFormat object in metadata store" };

  /// Type names for which a metadata entry should be added
  Gaudi::Property< std::vector< std::string > > m_typeNames{ this,
    "TypeNames", { ".*xAOD::.*" },
    "Type names for which metadata entries are added" };

  /// StoreGate keys that should be ignored during the metadata collection
  Gaudi::Property< std::vector< std::string > > m_ignoreKeys{ this,
    "IgnoreKeys", { "HLTAutoKey_.*" },
    "SG keys that should be ignored during the metadata collection" };

  Gaudi::Property< std::string > m_dataHeaderKey{this, "DataHeaderKey", "",
    "Key of DataHeader produced by output stream" };

  StatusCode collectFormatMetadata();

  /** @brief look up hash coresponding to primary class ID

      We can retrieve the list of all class IDs and hashes corresponding to an
      element in the output stream. The two collections should be of the same
      size. Element one of the @p classIDs corresponds to element one of the
      @p hashes and so on. This function steps through the two collections and
      retruns the hash corresponding to the classID matching the
      @p primaryClassID.

      @param [in] primaryClassID  the primary class ID
      @param [in] classIDs        the set of classIDs associated with a
                                  DataHeaderElement
      @param [in] hashes          the vector of hashes associated with a
                                  DataHeaderElement

      @warning classIDs and hashes must have the same number of entries

      @throws std::runtime_error  classIDs and hashes not the same size
      @throws std::range_error    primaryClassID not found in classIDs

      @returns the hash corresponding to the primaryClassID
   */
  unsigned int lookUpHash(
    CLID primaryClassID,
    const std::set< CLID >& classIDs,
    const std::vector< unsigned int >& hashes) const;

  /// CLIDs about which warnings have already been printed
  std::set< CLID > m_warnedCLIDs;
  /// Mutex for the @c m_warnedCLIDs variable
  std::mutex m_warnedCLIDsMutex;

  /// Mutex for the @c m_ef variable
  mutable std::mutex m_efMutex;
};  // class EventFormatStreamHelperTool
}  // namespace xAODMaker

#endif  // XAODEVENTFORMATCNV_EVENTFORMATSTREAMHELPERTOOL
