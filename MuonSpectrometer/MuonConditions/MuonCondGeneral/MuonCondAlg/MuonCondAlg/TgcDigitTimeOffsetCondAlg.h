/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONCONDALG_TGCDIGITTIMEOFFSETCONDALG_H_
#define MUONCONDALG_TGCDIGITTIMEOFFSETCONDALG_H_

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include "GaudiKernel/ICondSvc.h"
#include "MuonCondData/TgcDigitTimeOffsetData.h"
#include "StoreGate/ReadCondHandleKey.h"
#include "StoreGate/WriteCondHandleKey.h"

class TgcDigitTimeOffsetCondAlg : public AthReentrantAlgorithm
{
 public:
  TgcDigitTimeOffsetCondAlg (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TgcDigitTimeOffsetCondAlg() = default;
  virtual StatusCode initialize() override;
  virtual StatusCode execute(const EventContext& ctx) const override;

 private:
  SG::ReadCondHandleKey<CondAttrListCollection> m_readKey{this, "ReadKey", "/TGC/DIGIT/TOFFSET", "SG key for TGCDIGITTOFFSET"};
  SG::WriteCondHandleKey<TgcDigitTimeOffsetData> m_writeKey{this, "WriteKey", "TGCDigitTimeOffsetData", "SG Key of TgcDigitTimeOffset"};

  ServiceHandle<ICondSvc> m_condSvc{this, "CondSvc", "CondSvc"};
};

#endif   // MUONCONDALG_TGCDIGITTIMEOFFSETCONDALG_H_

