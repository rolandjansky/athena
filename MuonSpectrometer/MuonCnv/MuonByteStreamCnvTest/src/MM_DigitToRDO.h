/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MMDIGITTORDO_H
#define MMDIGITTORDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/MM_RawDataContainer.h"
#include "MuonDigitContainer/MmDigitContainer.h"

class MmIdHelper;

/////////////////////////////////////////////////////////////////////////////

class MM_DigitToRDO : public AthAlgorithm
{

public:

  MM_DigitToRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:

protected:
  
  const MmIdHelper*  m_idHelper;
  SG::WriteHandleKey<Muon::MM_RawDataContainer> m_rdoContainer;
  SG::ReadHandleKey<MmDigitContainer> m_digitContainer;
};

#endif
