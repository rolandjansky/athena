/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDIGITTOTGCRDO_H
#define TGCDIGITTOTGCRDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/sTGC_RawDataContainer.h"

class sTgcIdHelper;

/////////////////////////////////////////////////////////////////////////////

class sTGC_DigitToTGC_RDO : public AthAlgorithm
{

public:

  sTGC_DigitToTGC_RDO (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

protected:
  
  const sTgcIdHelper*  m_idHelper;
  SG::WriteHandleKey<sTGC_RawDataContainer> m_rdoContainer{this,"RDOContainerName","sTGCRDO","sTGC_RawDataContainer handle key"};
};

#endif
