// Dear emacs, this is -*- c++ -*-
/* Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration */

// $Id: EventFormatMetaDataTool.h 651874 2015-03-05 14:16:19Z krasznaa $
#ifndef XAODEVENTFORMATCNV_EVENTFORMATMETADATATOOL
#define XAODEVENTFORMATCNV_EVENTFORMATMETADATATOOL

// System include(s):
#include <string>
#include <memory>
#include <mutex>

// Gaudi/Athena include(s):
#include "Gaudi/Property.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaKernel/IMetaDataSvc.h"
#include "AthenaKernel/IMetaDataTool.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "StoreGate/StoreGateSvc.h"

// EDM include(s):
#include "xAODEventFormat/EventFormat.h"

namespace xAODMaker {

/// Tool taking care of copying the event format object from file to file
///
/// This tool does the heavy lifting when fast-merging DxAOD files to
/// make sure that the xAOD::EventFormat metadata object is propagated
/// correctly from the input files to the output.
///
/// @author Jack Cranshaw <cranshaw@anl.gov>
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
/// @author Frank Berghaus <fberghaus@anl.gov>
///
/// $Revision: 651874 $
/// $Date: 2015-03-05 15:16:19 +0100 (Thu, 05 Mar 2015) $
///
class EventFormatMetaDataTool : public virtual ::IMetaDataTool, public ::AthAlgTool {
 public:
  /// Regular AlgTool constructor
  EventFormatMetaDataTool(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  /// Function initialising the tool
  virtual StatusCode initialize();

  /// Function collecting the metadata from a new input file
  virtual StatusCode beginInputFile(const SG::SourceID&) {return beginInputFile();}

  /// Function collecting the metadata from a new input file
  virtual StatusCode endInputFile(const SG::SourceID&) {return endInputFile();}

  /// Wait for metadata write operations to finish, then returns SUCCESS
  virtual StatusCode metaDataStop(const SG::SourceID&) {return metaDataStop();}

  /// Function called when a new input file is opened
  virtual StatusCode beginInputFile();

  /// Function called when the currently open input file got completely
  /// processed
  virtual StatusCode endInputFile() {return StatusCode::SUCCESS;}

  /// Wait for metadata write operations to finish, then return SUCCESS
  virtual StatusCode metaDataStop();

 private:
  /// Function collecting the event format metadata from the input file
  StatusCode collectMetaData(std::unique_ptr< xAOD::EventFormat >&);

  /// Function writing out the collected metadata
  StatusCode writeMetaData(std::unique_ptr< xAOD::EventFormat >&);

  /// Connection to the input metadata store
  ServiceHandle< ::StoreGateSvc > m_inputMetaStore{this, "InputMetaStore",
    "StoreGateSvc/InputMetaDataStore", name()};

  /// Connection to the output metadata store
  ServiceHandle< IMetaDataSvc > m_outputMetaStore{this, "MetaDataSvc",
    "MetaDataSvc", name()};

  /// The key of the trigger menu in the input file
  Gaudi::Property< std::string > m_inputKey{this, "InputKey", "",
    "optionally specify the key of the xAOD::EventFormat in the input."};

  /// The key of the trigger menu for the output file
  Gaudi::Property< std::string > m_outputKey{this, "OutputKey",
    "EventFormat", "Specifies the key of the xAOD::EventFormat object in "
        "the MetaDataStore, default: EventFormat"};

  /// MetaDataStop need to wait for ongoing writes
  std::mutex m_outputMutex;
};  // class EventFormatMetaDataTool
}  // namespace xAODMaker

#endif  // XAODEVENTFORMATCNV_EVENTFORMATMETADATATOOL
