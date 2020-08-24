/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_STGCRDOTOSTGCDIGIT_H
#define MUONBYTESTREAMCNVTEST_STGCRDOTOSTGCDIGIT_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonSTGC_CnvTools/ISTGC_RDO_Decoder.h"
#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class STGC_RdoToDigit : public AthReentrantAlgorithm {

 public:

  STGC_RdoToDigit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~STGC_RdoToDigit() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:

  StatusCode decodeSTGC( const Muon::STGC_RawDataCollection *, sTgcDigitContainer *, sTgcDigitCollection*&, Identifier& ) const;

  ToolHandle<Muon::ISTGC_RDO_Decoder> m_stgcRdoDecoderTool{this, "sTgcRdoDecoderTool", "Muon::STGC_RDO_Decoder", ""};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  SG::ReadHandleKey<Muon::STGC_RawDataContainer> m_stgcRdoKey{this, "sTgcRdoContainer", "sTGCRDO", "sTGC RDO Input"};
  SG::WriteHandleKey<sTgcDigitContainer> m_stgcDigitKey{this, "sTgcDigitContainer", "sTGC_DIGITS", "sTGC Digit Output"};
};

#endif

