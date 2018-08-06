/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCDIGITTORDO_H
#define STGCDIGITTORDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/STGC_RawDataContainer.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

class sTgcIdHelper;

/////////////////////////////////////////////////////////////////////////////

class STGC_DigitToRDO : public AthAlgorithm
{

public:

  STGC_DigitToRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;
  virtual StatusCode finalize() override;

private:

protected:
  
  const sTgcIdHelper*  m_idHelper;
  SG::WriteHandleKey<Muon::STGC_RawDataContainer> m_rdoContainer;
  SG::ReadHandleKey<sTgcDigitContainer> m_digitContainer;
};

#endif
