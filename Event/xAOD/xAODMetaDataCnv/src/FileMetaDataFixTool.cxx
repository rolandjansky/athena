/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#include "FileMetaDataFixTool.h"

#include <stdexcept>

#include "GaudiKernel/IIncidentSvc.h"

#include "xAODMetaData/FileMetaData.h"
#include "xAODMetaData/FileMetaDataAuxInfo.h"


namespace xAODMaker {

FileMetaDataFixTool::FileMetaDataFixTool(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
    : base_class(type, name, parent) {
  declareProperty("McChannelNumber", m_mcChannelNumber=0,
                  "Value to replace content of FileMetaData::mcProcID.");

  declareProperty("FileMetaDataKey", m_fileMetaDataKey="FileMetaData",
                  "Key of FileMetaData object in MetaDataStore.");
}

StatusCode
    FileMetaDataFixTool::initialize() {
      ATH_CHECK(m_metaDataStore.retrieve());
      ATH_MSG_DEBUG("Retreived MetaDataStore handle");

      ServiceHandle< IIncidentSvc > incidentSvc( "IncidentSvc", name() );
      ATH_CHECK( incidentSvc.retrieve() );

      // Set priority lower than AsgMetaDataTool, which uses value 70.
      // That way this tool's MetaDataStop happens after
      // FileMetaDataCreatorTool::metaDataStop
      incidentSvc->addListener( this, "MetaDataStop", 50 );

      return StatusCode::SUCCESS;
    }

StatusCode
    FileMetaDataFixTool::metaDataStop() {
      ATH_MSG_DEBUG("Fixing FileMetaData:mcProcID");

      // Abort if FileMetaData is not available
      if (!m_metaDataStore->contains< xAOD::FileMetaData >(m_fileMetaDataKey)) {
        ATH_MSG_FATAL("No FileMetaData object to correct");
        return StatusCode::FAILURE;
      }

      // Retrieve FileMetaData
      xAOD::FileMetaData * file_info = 0;
      ATH_CHECK(m_metaDataStore->retrieve(file_info, m_fileMetaDataKey));

      // update mcProcID, abort if this fails
      if (!file_info->setValue(xAOD::FileMetaData::mcProcID,
                               static_cast< float >(m_mcChannelNumber))) {
        ATH_MSG_FATAL("Failed to update FileMetaData::mcProcID");
        return StatusCode::FAILURE;
      }

      return StatusCode::SUCCESS;
    }

void
    FileMetaDataFixTool::handle(const Incident& inc) {
      if (inc.type() == "MetaDataStop") {
        if (metaDataStop().isFailure()) {
          throw std::runtime_error("Failed to adjust xAOD::FileMetaData");
        }
      }
    }

}  // namespace xAODMaker
