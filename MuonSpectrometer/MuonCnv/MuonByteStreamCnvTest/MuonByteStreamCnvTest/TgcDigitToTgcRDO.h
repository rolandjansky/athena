/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDIGITTOTGCRDO_H
#define TGCDIGITTOTGCRDO_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "StoreGate/DataHandle.h"

#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdo.h"

class TgcIdHelper;
class ITGCcablingServerSvc;
class ITGCcablingSvc;

/////////////////////////////////////////////////////////////////////////////

class TgcDigitToTgcRDO : public AthAlgorithm
{

public:

  TgcDigitToTgcRDO (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

  StatusCode getCabling();

  StatusCode fillTagInfo() const;

  StatusCode fill_TGCdata();
  TgcRdo * getTgcRdo(const TgcRawData * rawData);

  std::map<uint16_t, TgcRdo *> m_tgcRdoMap;

  BooleanProperty m_isNewTgcDigit; // to select new TgcDigit (bcTag added)

protected:

  ServiceHandle<ActiveStoreSvc>    m_activeStore;
  ServiceHandle<ITGCcablingServerSvc> m_tgc_cabling_server; 

  TgcRdoContainer      * m_rdoContainer;
  const ITGCcablingSvc * m_cabling;
  const TgcIdHelper    * m_tgcIdHelper;

  std::string  m_cablingType;

  std::string m_rdoContainerName;
};

#endif
