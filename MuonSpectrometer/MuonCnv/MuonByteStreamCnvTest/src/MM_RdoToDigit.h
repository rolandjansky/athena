/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_MMRDOTOMMDIGIT_H
#define MUONBYTESTREAMCNVTEST_MMRDOTOMMDIGIT_H

#include "GaudiKernel/ToolHandle.h"
#include "AthenaBaseComps/AthAlgorithm.h"
#include "MuonMM_CnvTools/IMM_RDO_Decoder.h"
#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonDigitContainer/MmDigitContainer.h"

class MmIdHelper;

class MM_RdoToDigit : public AthAlgorithm {

 public:

  MM_RdoToDigit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MM_RdoToDigit() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute() override final;

 private:

  StatusCode decodeMM( const Muon::MM_RawDataCollection *, MmDigitContainer *, MmDigitCollection*&, Identifier& ) const;

  ToolHandle<Muon::IMM_RDO_Decoder> m_mmRdoDecoderTool{this, "mmRdoDecoderTool", "Muon::MM_RDO_Decoder", ""};
  const MmIdHelper *   m_mmHelper{};
  SG::ReadHandleKey<Muon::MM_RawDataContainer> m_mmRdoKey{this, "MmRdoContainer", "MMRDO", "MM RDO Input"};
  SG::WriteHandleKey<MmDigitContainer> m_mmDigitKey{this, "MmDigitContainer", "MM_DIGITS", "MM Digit Output"};
};

#endif

