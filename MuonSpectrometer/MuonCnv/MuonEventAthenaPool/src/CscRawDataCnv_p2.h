/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCRAWDATACNV_P2_H
#define MUON_CSCRAWDATACNV_P2_H

/*
Transient/Persistent converter for CscRawData class
Author: Marcin Nowak
        CERN, January 2006
*/

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"  
#include "MuonEventAthenaPool/CscRawData_p2.h"
#include "MuonRDO/CscRawData.h"
#include "StoreGate/DataHandle.h"
#include "MuonIdHelpers/CscIdHelper.h"

class MsgStream;

class CscRawDataCnv_p2  : public T_AthenaPoolTPCnvBase<CscRawData, CscRawData_p2>
{
public:
  CscRawDataCnv_p2() {}
  
  virtual void		persToTrans(const CscRawData_p2* persObj, CscRawData* transObj, MsgStream &log);
  virtual void		transToPers(const CscRawData* transObj, CscRawData_p2* persObj, MsgStream &log);
private:
  bool initialize();
  bool m_init=false;
  const DataHandle<CscIdHelper> m_cscIdHelper=nullptr;
};


#endif

