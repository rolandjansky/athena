/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECODER_AFP_RAWDATAPROVIDER_H
#define DECODER_AFP_RAWDATAPROVIDER_H 1

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "AFP_RawDataProviderTool.h"

#include "AFP_RawEv/AFP_ROBID.h"

#include "eformat/ROBFragment.h"

#include <string>
#include <vector>

class AFP_RawDataProvider : public ::AthReentrantAlgorithm {
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
  virtual StatusCode execute(const EventContext &ctx) const;
  
private:
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  ToolHandle<AFP_RawDataProviderTool> m_rawDataTool{this, "ProviderTool", "AFP_RawDataProviderTool"};

  /// name used to store AFP_RawContainer in StoreGate
  SG::WriteHandleKey<AFP_RawContainer> m_AFP_RawContainerKey{this, "AFP_RawContainerKey", "AFP_RawData",
      		  "Name under which AFP_RawContainer object will be saved in StoreGate"};
  
  /// vector of robIDs from which data should be processed
  const std::vector<unsigned int> m_robIDs = {AFP_ROBID::sideA, AFP_ROBID::sideC, AFP_ROBID::sideC_2016};

};

#endif //> !DECODER_AFP_RAWDATAPROVIDER_H
