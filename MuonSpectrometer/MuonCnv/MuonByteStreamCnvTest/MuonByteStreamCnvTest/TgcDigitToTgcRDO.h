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
#include "MuonDigitContainer/TgcDigitContainer.h"
#include "TGCcablingInterface/ITGCcablingServerSvc.h"

class TgcIdHelper;
class ITGCcablingSvc;

/////////////////////////////////////////////////////////////////////////////

class TgcDigitToTgcRDO : public AthAlgorithm
{

public:

  TgcDigitToTgcRDO (const std::string& name, ISvcLocator* pSvcLocator);
  virtual StatusCode initialize() override;
  virtual StatusCode execute() override;

private:

  StatusCode getCabling();

  StatusCode fillTagInfo() const;

  StatusCode fill_TGCdata();
  TgcRdo * getTgcRdo(const TgcRawData * rawData);

  std::map<uint16_t, TgcRdo *> m_tgcRdoMap;

  BooleanProperty m_isNewTgcDigit; // to select new TgcDigit (bcTag added)

protected:

  ServiceHandle<ITGCcablingServerSvc> m_tgc_cabling_server; 

  SG::WriteHandleKey<TgcRdoContainer> m_rdoContainerKey{this,"OutputObjectName","TGCRDO","WriteHandleKey for Output TgcRdoContainer"};
  SG::ReadHandleKey<TgcDigitContainer> m_digitContainerKey{this,"InputObjectName","TGC_DIGITS","ReadHandleKey for Input TgcDigitContainer"};
  const ITGCcablingSvc * m_cabling;
  const TgcIdHelper    * m_tgcIdHelper;

  std::string  m_cablingType;

};

#endif
