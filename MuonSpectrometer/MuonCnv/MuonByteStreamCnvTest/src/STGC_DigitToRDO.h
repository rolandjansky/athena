/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCDIGITTORDO_H
#define STGCDIGITTORDO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

class sTgcIdHelper;

/////////////////////////////////////////////////////////////////////////////

class STGC_DigitToRDO : public AthReentrantAlgorithm
{

public:

  STGC_DigitToRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~STGC_DigitToRDO() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:

  const sTgcIdHelper*  m_idHelper{};
  SG::WriteHandleKey<Muon::STGC_RawDataContainer> m_rdoContainer{this, "OutputObjectName", "sTGCRDO", "WriteHandleKey for Output STGC_RawDataContainer"};
  SG::ReadHandleKey<sTgcDigitContainer> m_digitContainer{this, "InputObjectName", "sTGC_DIGITS", "ReadHandleKey for Input sTgcDigitContainer"};
};

#endif
