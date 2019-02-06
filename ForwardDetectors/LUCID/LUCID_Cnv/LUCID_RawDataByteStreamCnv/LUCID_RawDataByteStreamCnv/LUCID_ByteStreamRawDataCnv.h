/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
#include "StoreGate/WriteHandleKey.h"

class LUCID_RodDecoder;

class LUCID_ByteStreamRawDataCnv: public AthAlgorithm {

 public:
  
  LUCID_ByteStreamRawDataCnv (const std::string &name, ISvcLocator* pSvcLocator);
  virtual ~LUCID_ByteStreamRawDataCnv();

  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

  StatusCode fillContainer(const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>&,
                           LUCID_RawDataContainer& container) const;
  
private:
  
  ServiceHandle<IROBDataProviderSvc> m_robDataProvider;
  SG::WriteHandleKey<LUCID_RawDataContainer>  m_lucid_RawDataContainerKey
  { this, "lucid_RawDataContainerKey", "Lucid_RawData", "" };
  
  LUCID_RodDecoder        m_rodDecoder;
};

#endif
