/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDIGITTORDO_H
#define MMDIGITTORDO_H

#include "AthenaBaseComps/AthReentrantAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"

#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonDigitContainer/MmDigitContainer.h"
#include "MuonIdHelpers/IMuonIdHelperSvc.h"

/////////////////////////////////////////////////////////////////////////////

class MM_DigitToRDO : public AthReentrantAlgorithm
{

public:

  MM_DigitToRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~MM_DigitToRDO() = default;
  virtual StatusCode initialize() override final;
  virtual StatusCode execute(const EventContext& ctx) const override final;

private:
  ServiceHandle<Muon::IMuonIdHelperSvc> m_idHelperSvc {this, "MuonIdHelperSvc", "Muon::MuonIdHelperSvc/MuonIdHelperSvc"};
  SG::WriteHandleKey<Muon::MM_RawDataContainer> m_rdoContainer{this, "OutputObjectName", "MMRDO","WriteHandleKey for Output MM_RawDataContainer"};
  SG::ReadHandleKey<MmDigitContainer> m_digitContainer{this, "InputObjectName", "MM_DIGITS", "ReadHAndleKey for Input MmDigitContainer"};
};

#endif
