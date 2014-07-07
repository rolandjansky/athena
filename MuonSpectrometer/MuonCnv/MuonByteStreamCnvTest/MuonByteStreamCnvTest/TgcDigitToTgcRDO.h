/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TGCDIGITTOTGCRDO_H
#define TGCDIGITTOTGCRDO_H

#include <map>
#include <inttypes.h>

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/Property.h"
#include "StoreGate/DataHandle.h"
#include "Identifier/Identifier.h"

#include "MuonIdHelpers/TgcIdHelper.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "TGCcablingInterface/ITGCcablingSvc.h"

#include "MuonRDO/TgcRdoContainer.h"
#include "MuonRDO/TgcRdo.h"

class ActiveStoreSvc;
class MsgStream;
class TgcIdHelper;
class ITagInfoMgr;


/////////////////////////////////////////////////////////////////////////////

class TgcDigitToTgcRDO : public Algorithm
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

  ActiveStoreSvc    * m_activeStore;
  ServiceHandle<StoreGateSvc> m_EvtStore;
  ServiceHandle<ITGCcablingServerSvc> m_tgc_cabling_server; 

  TgcRdoContainer      * m_rdoContainer;
  const ITGCcablingSvc * m_cabling;
  const TgcIdHelper    * m_tgcIdHelper;
  ITagInfoMgr          * m_tagInfoMgr;   // Tag Info Manager

  MsgStream* m_log;
  bool m_debug;
  bool m_verbose;

  std::string  m_cablingType;

  std::string m_rdoContainerName;
};

#endif
