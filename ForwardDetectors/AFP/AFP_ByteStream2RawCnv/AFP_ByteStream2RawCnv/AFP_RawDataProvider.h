/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECODER_AFP_RAWDATAPROVIDER_H
#define DECODER_AFP_RAWDATAPROVIDER_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "AFP_RawDataProviderTool.h"

#include "AFP_RawEv/AFP_ROBID.h"

#include "eformat/ROBFragment.h"

#include <string>
#include <vector>

class AFP_RawDataProvider : public ::AthAlgorithm {
public:

  AFP_RawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);

  /// Does nothing
  virtual ~AFP_RawDataProvider();

  /// Initialise tool and service
  virtual StatusCode initialize();

  /// Does nothing
  virtual StatusCode finalize() { return StatusCode::SUCCESS; }

  /// @brief Creates raw objects from bytestream
  ///
  /// Creates a new AFP_RawDataContainer saves it to StoreGate and
  /// fills with collections based on information from robIDs
  /// specified in #s_robIDs
  virtual StatusCode execute();
  
private:
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  ToolHandle<AFP_RawDataProviderTool> m_rawDataTool;

  /// name used to store AFP_RawContainer in StoreGate
  std::string m_AFP_RawContainerKey;
  
  /// vector of robIDs from which data should be processed
  static const std::vector<unsigned int> s_robIDs;
};

#endif //> !DECODER_AFP_RAWDATAPROVIDER_H
