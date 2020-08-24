/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUONBYTESTREAMCNVTEST_TGCRDOTOTGCDIGIT_H
#define MUONBYTESTREAMCNVTEST_TGCRDOTOTGCDIGIT_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/ToolHandle.h"

#include "MuonTGC_CnvTools/ITGC_RDO_Decoder.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

class TgcRdoToTgcDigit : public AthReentrantAlgorithm {

 public:

  TgcRdoToTgcDigit(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TgcRdoToTgcDigit() = default;

  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

 private:
  StatusCode decodeTgc( const TgcRdo *, TgcDigitContainer *, Identifier&) const;

  ToolHandle<Muon::ITGC_RDO_Decoder>  m_tgcRdoDecoderTool{this, "tgcRdoDecoderTool", "Muon::TgcRDO_Decoder", ""};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  Gaudi::Property<bool> m_show_warning_level_invalid_TGC_A09_SSW6_hit{this, "show_warning_level_invalid_TGC_A09_SSW6_hit", false, ""};

  ServiceHandle<ITGCcablingServerSvc> m_tgcCablingServerSvc{this, "TGCcablingServerSvc", "TGCcablingServerSvc", ""};
  const ITGCcablingSvc * m_tgcCabling{};
  /** Flag to distinguish 12-fold TGC cabling and 8-fold TGC cabling */
  bool m_is12foldTgc{true};

  SG::ReadHandleKey<TgcRdoContainer> m_tgcRdoKey{this, "TgcRdoContainer", "TGCRDO", "Tgc RDO Input"};
  SG::WriteHandleKey<TgcDigitContainer> m_tgcDigitKey{this, "TgcDigitContainer", "TGC_DIGITS", "Tgc Digit Output"};
};

#endif
