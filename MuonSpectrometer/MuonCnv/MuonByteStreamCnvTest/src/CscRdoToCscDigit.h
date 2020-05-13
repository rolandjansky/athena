/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_CSCRDOTOCSCDIGIT_H
#define MUONBYTESTREAMCNVTEST_CSCRDOTOCSCDIGIT_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "CscCalibTools/ICscCalibTool.h"
#include "MuonCSC_CnvTools/ICSC_RDO_Decoder.h"
#include "MuonRDO/CscRawDataContainer.h"
#include "MuonDigitContainer/CscDigitContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class CscRdoToCscDigit : public AthReentrantAlgorithm {

 public:

  CscRdoToCscDigit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CscRdoToCscDigit() = default;

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:

  StatusCode decodeCsc( const CscRawDataCollection *, CscDigitContainer *, CscDigitCollection*&, Identifier&) const;

  ToolHandle<ICscCalibTool>     m_cscCalibTool{this, "cscCalibTool", "CscCalibTool", ""};
  ToolHandle<Muon::ICSC_RDO_Decoder>  m_cscRdoDecoderTool{this, "cscRdoDecoderTool", "Muon::CscRDO_Decoder", ""};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  SG::ReadHandleKey<CscRawDataContainer> m_cscRdoKey{this, "CscRdoContainer", "CSCRDO", "Csc RDO Input"};
  SG::WriteHandleKey<CscDigitContainer> m_cscDigitKey{this, "CscDigitContainer", "CSC_DIGITS", "Csc Digit Output"};
};

#endif

