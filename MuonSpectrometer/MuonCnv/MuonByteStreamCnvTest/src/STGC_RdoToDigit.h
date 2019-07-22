/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_STGCRDOTOSTGCDIGIT_H
#define MUONBYTESTREAMCNVTEST_STGCRDOTOSTGCDIGIT_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonSTGC_CnvTools/ISTGC_RDO_Decoder.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

class sTgcIdHelper;

class STGC_RdoToDigit : public AthAlgorithm {

 public:

  STGC_RdoToDigit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~STGC_RdoToDigit() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

 private:

  StatusCode decodeSTGC( const Muon::STGC_RawDataCollection *, sTgcDigitContainer *, sTgcDigitCollection*&, Identifier& ) const;

  ToolHandle<Muon::ISTGC_RDO_Decoder> m_stgcRdoDecoderTool{this, "sTgcRdoDecoderTool", "Muon::STGC_RDO_Decoder", ""};
  const sTgcIdHelper *   m_stgcHelper{};
  sTgcDigitContainer *   m_stgcContainer{};
  SG::ReadHandleKey<Muon::STGC_RawDataContainer> m_stgcRdoKey{this, "sTgcRdoContainer", "sTGCRDO", "sTGC RDO Input"};
  SG::WriteHandleKey<sTgcDigitContainer> m_stgcDigitKey{this, "sTgcDigitContainer", "sTGC_DIGITS", "sTGC Digit Output"};
};

#endif

