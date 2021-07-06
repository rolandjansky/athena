// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODMETADATACNV_FILEMETADATACREATORTOOL_H
#define XAODMETADATACNV_FILEMETADATACREATORTOOL_H

// System include(s):
#include <memory>
#include <string>

// Infrastructure include(s):
#include "Gaudi/Property.h"
#include "GaudiKernel/extends.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ServiceHandle.h"
#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaKernel/IAthenaOutputTool.h"
#include "AthenaKernel/IMetaDataSvc.h"
#include "IOVDbDataModel/IOVMetaDataContainer.h"
#include "CoralBase/AttributeList.h"
#include "StoreGate/ReadHandleKey.h"

// EDM include(s):
#include "PersistentDataModel/DataHeader.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODMetaData/FileMetaData.h"
#include "xAODMetaData/FileMetaDataAuxInfo.h"

namespace xAODMaker {

/// Tool creating xAOD::FileMetaData information
///
/// This Athena-only tool can be used to create xAOD::FileMetaData information
/// out of the non-ROOT-readable metadata available in the input. The
/// FileMetaDataTool may have copied a xAOD::FileMetaData object to the
/// MetaDataStore. If such a xAOD::FileMetaData object is found it is updated
/// to reflect the current data type and MC channel number.
///
/// @author Attila Krasznahorkay <Attila.Krasznahorkay@cern.ch>
/// @author Frank Berghaus <fberghaus@anl.gov>
///
class FileMetaDataCreatorTool
    : public extends< AthAlgTool, IAthenaOutputTool, IIncidentListener > {
 public:
  using extends::extends;

  /// @name AlgTool Methods
  /// @{
  /// Called by AthenaOutputStream::initialize() (via ToolSvc retrieve()).
  StatusCode initialize() override;

  /// Called at the end of AthenaOutputStream::finalize() (via release()).
  StatusCode finalize() override;
  /// @}

  /// @name Methods inherited by IAthenaOutputTool
  /// @{
  /// Called at the end of AthenaOutputStream::initialize().
  StatusCode postInitialize() override;

  /// Called at the beginning of AthenaOutputStream::execute().
  StatusCode preExecute() override;

  /// Called before actually streaming objects.
  StatusCode preStream() override;

  /// Fill the FileMetaData with event information
  ///
  /// Use information from @c xAOD::EventIngo to fill
  /// xAOD::FileMetaData::mcProcID and @c DataHeader to set
  /// xAOD::FileMetaData::dataType
  StatusCode postExecute() override;

  /// Write the FileMetaData object to the MetaDataStore via the MetaDataSvc
  StatusCode preFinalize() override;
  /// @}

  /// @name IIncidentListener methods
  //@{
  /// Handle BeginInputFile incident after MetaDataSvc
  ///
  /// Calls  updateFromNonEvent on BeginInputFile
  void handle(const Incident&) override;
  //@}

 private:
  /// output key for produced xAOD::FileMetaData in MetaDataStore
  Gaudi::Property< std::string > m_key{
      this,
      "OutputKey",
      "FileMetaData",
      "Key used to write FileMetaData into MetaDataStore"
  };

  /// Read tag information
  Gaudi::Property< std::string > m_tagInfoKey {
      this,
      "TagInfoKey",
      "/TagInfo",
      "Key to look up tags from InputMetaDataStore"
  };

  /// Read simulation parameters
  Gaudi::Property< std::string > m_simInfoKey {
      this,
      "SimInfoKey",
      "/Simulation/Parameters",
      "Key to look up simulation parameters from InputMetaDataStore"
  };

  /// Key for xAOD::EventInfo to update MC channel number
  Gaudi::Property< std::string > m_eventInfoKey {
      this,
      "EventInfoKey",
      "EventInfo",
      "Key to read xAOD::EventInfo from EventStore"
  };

  /// Key for DataHeader in StoreGateSvc
  Gaudi::Property< std::string > m_dataHeaderKey {
      this,
      "StreamName",
      "",
      "Key to read data header from event store"
  };

  /// DataHeader is produced by another OutputTool, so need StoreGateSvc
  ServiceHandle< StoreGateSvc > m_eventStore{"StoreGateSvc", name()};

  /// Use MetaDataSvc store interface to support output in EventService
  ServiceHandle< IMetaDataSvc > m_metaDataSvc{"MetaDataSvc", name()};

  /// Use MetaDataSvc store interface to support output in EventService
  ServiceHandle< StoreGateSvc > m_inputMetaDataStore{
    "InputMetaDataStore", name()};

  /// Update from Simulation Parameters and TagInfo
  StatusCode updateFromNonEvent();

  /// helper tool to update file meta data with IOV string content
  StatusCode setString(
      const coral::AttributeList& attributeList,
      const std::string& tag,
      const xAOD::FileMetaData::MetaDataType type);

  /// helper tool to update file meta data with IOV float content
  StatusCode setFloat(
      const coral::AttributeList& attrList,
      const std::string& tag,
      const xAOD::FileMetaData::MetaDataType);

  /// helper tool to update file meta data with IOV boolean content
  StatusCode setBool(
      const coral::AttributeList& attrList,
      const std::string& tag,
      const xAOD::FileMetaData::MetaDataType);

  /// The object created for this output stream
  std::unique_ptr< xAOD::FileMetaData > m_info;

  /// The auxiliary containing the created object
  std::unique_ptr< xAOD::FileMetaDataAuxInfo > m_aux;

  /// FileMetaData has been filled with non-event info
  bool m_filledNonEvent{false};

  /// FileMetaData has been filled with event information
  bool m_filledEvent{false};

  /// creation of FileMetaData should happen on a single thread
  std::mutex m_toolMutex;
};  // class FileMetaDataCreatorTool

}  // namespace xAODMaker

#endif  // XAODMETADATACNV_FILEMETADATACREATORTOOL_H
