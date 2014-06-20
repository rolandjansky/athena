/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_BYTESTREAMRAWDATACNV_H
#define LUCID_BYTESTREAMRAWDATACNV_H

#include <stdint.h>
#include <map>
#include <string>

#include "AthenaBaseComps/AthAlgorithm.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "LUCID_RawEvent/LUCID_RawDataContainer.h"
#include "LUCID_RawEvent/LUCID_RawData.h"

#include "LUCID_RawDataByteStreamCnv/LUCID_RodDecoder.h"

class StoreGateSvc;
class LUCID_RodDecoder;

using OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment;

class LUCID_ByteStreamRawDataCnv: public AthAlgorithm {

 public:
  
  LUCID_ByteStreamRawDataCnv (const std::string &name, ISvcLocator* pSvcLocator);
  ~LUCID_ByteStreamRawDataCnv();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  StatusCode fillContainer(std::vector<const ROBFragment*>); 
  
private:
  
  ServiceHandle<StoreGateSvc>        m_storeGate;
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  std::string                        m_lucid_RawDataContainerKey;
  
  LUCID_RawDataContainer* m_LUCID_RawDataContainer;
  LUCID_RodDecoder        m_rodDecoder;
};

#endif
