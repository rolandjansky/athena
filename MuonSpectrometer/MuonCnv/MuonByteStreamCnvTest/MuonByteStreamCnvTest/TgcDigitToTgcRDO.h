/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDIGITTOTGCRDO_H
#define TGCDIGITTOTGCRDO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "StoreGate/DataHandle.h"
#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdo.h"
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"

/////////////////////////////////////////////////////////////////////////////

class TgcDigitToTgcRDO : public AthReentrantAlgorithm
{

public:

  TgcDigitToTgcRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~TgcDigitToTgcRDO() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:

  StatusCode getCabling();

  StatusCode fillTagInfo() const;

  StatusCode fill_TGCdata(const EventContext& ctx) const;
  TgcRdo * getTgcRdo(const TgcRawData * rawData,  std::map<uint16_t, TgcRdo *>& tgcRdoMap) const;

  BooleanProperty m_isNewTgcDigit; // to select new TgcDigit (bcTag added)

protected:

  ServiceHandle<ITGCcablingServerSvc> m_tgc_cabling_server;

  SG::WriteHandleKey<TgcRdoContainer> m_rdoContainerKey{this,"OutputObjectName","TGCRDO","WriteHandleKey for Output TgcRdoContainer"};
  SG::ReadHandleKey<TgcDigitContainer> m_digitContainerKey{this,"InputObjectName","TGC_DIGITS","ReadHandleKey for Input TgcDigitContainer"};
  const ITGCcablingSvc * m_cabling{};
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};

  std::string  m_cablingType;

};

#endif
