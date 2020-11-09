/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODMETADATACNV_FILEMETADATAFIXTOOL
#define XAODMETADATACNV_FILEMETADATAFIXTOOL

#include <string>

#include "GaudiKernel/extends.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/ServiceHandle.h"

#include "AthenaBaseComps/AthAlgTool.h"
#include "AthenaPoolKernel/IMetaDataTool.h"
#include "StoreGate/StoreGateSvc.h"


namespace xAODMaker {

class FileMetaDataFixTool
    : public extends< AthAlgTool,
                      IMetaDataTool,
                      IIncidentListener > {
 public:
  using extends::extends;

  FileMetaDataFixTool(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  /// Register to listen to MetaDataStop
  StatusCode initialize() override;

  /// Updates FileMetaData with given MC channel number
  StatusCode metaDataStop();

  /// React to MetaDataStop incident by calling metaDataStop()
  void handle(const Incident& inc) override;

 private:
  /// The MC channel number to use
  unsigned int m_mcChannelNumber;

  std::string m_fileMetaDataKey;

  ServiceHandle< StoreGateSvc > m_metaDataStore{"MetaDataStore", name()};
};  // FileMetaDataFixTool

}  // namespace xAODMaker

#endif  // XAODMETADATACNV_FILEMETADATAFIXTOOL
