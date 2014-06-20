/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DIGITRAWDATACNV_H
#define LUCID_DIGITRAWDATACNV_H

#include <stdint.h>
#include <string>

#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthAlgorithm.h"

#include "LUCID_RawEvent/LUCID_DigitContainer.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"
#include "LUCID_RawEvent/LUCID_RawData.h"

#include "LUCID_RawDataByteStreamCnv/LUCID_RodEncoder.h"

class StoreGateSvc;
class LUCID_RodEncoder;

class LUCID_DigitRawDataCnv: public AthAlgorithm {

 public:
  
  LUCID_DigitRawDataCnv (const std::string &name, ISvcLocator* pSvcLocator);
  ~LUCID_DigitRawDataCnv();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  
  ServiceHandle<StoreGateSvc> m_storeGate;
  std::string                 m_lucid_RawDataContainerKey;
  std::string                 m_digitContainerKey;

  const LUCID_DigitContainer* m_digitContainer;

  LUCID_RawDataContainer* m_LUCID_RawDataContainer;
  LUCID_RodEncoder        m_rodEncoder;
};

#endif

