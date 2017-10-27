/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef sTGCDIGITTORDO_H
#define sTGCDIGITTORDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/sTGC_RawDataContainer.h"
#include "MuonDigitContainer/sTgcDigitContainer.h"

class sTgcIdHelper;

/////////////////////////////////////////////////////////////////////////////

class sTGC_DigitToRDO : public AthAlgorithm
{

public:

  sTGC_DigitToRDO (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

protected:
  
  const sTgcIdHelper*  m_idHelper;
  SG::WriteHandleKey<Muon::sTGC_RawDataContainer> m_rdoContainer{this,"RDOContainerName","sTGCRDO","sTGC_RawDataContainer handle key"};
  SG::ReadHandleKey<sTgcDigitContainer> m_digitContainer{this,"DigitContainerName","sTGC_DIGITS","sTgcDigitContainer handle key"};
};

#endif
