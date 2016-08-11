/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef DECODER_AFP_RAWDATAPROVIDER_H
#define DECODER_AFP_RAWDATAPROVIDER_H 1

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "AFP_RawEv/AFP_RawEvDict.h"
#include "ByteStreamCnvSvcBase/IByteStreamEventAccess.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "AFP_RawDataProviderTool.h"
#include <string>

class AFP_RawDataProviderTool;
class ISvcLocator;
class StatusCode;

class AFP_RawDataProvider : public ::AthAlgorithm {
public:

  AFP_RawDataProvider(const std::string &name, ISvcLocator *pSvcLocator);
  virtual ~AFP_RawDataProvider();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize() { return StatusCode::SUCCESS; }

private:
  int m_nRawDataCollection;
  int m_nRawData;

  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  ToolHandle<AFP_RawDataProviderTool> m_rawDataTool;

  std::string m_AFP_RawDataCollectionKey;
  std::string m_collection;
};

#endif //> !DECODER_AFP_RAWDATAPROVIDER_H
