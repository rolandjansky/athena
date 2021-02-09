/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCRAWDATACNV_P4_H
#define MUON_CSCRAWDATACNV_P4_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"  
#include "MuonEventAthenaPool/CscRawData_p4.h"
#include "MuonRDO/CscRawData.h"
#include "StoreGate/DataHandle.h"
#include "MuonIdHelpers/CscIdHelper.h"

class MsgStream;

/**
Transient/Persistent converter for CscRawData class
*/
class CscRawDataCnv_p4  : public T_AthenaPoolTPCnvBase<CscRawData, CscRawData_p4>
{
public:
  CscRawDataCnv_p4() {}
  virtual void		persToTrans(const CscRawData_p4* persObj, CscRawData* transObj, MsgStream &log);
  virtual void		transToPers(const CscRawData* transObj, CscRawData_p4* persObj, MsgStream &log);
private:
  bool initialize();
  bool m_init=false;
  const DataHandle<CscIdHelper> m_cscIdHelper=nullptr;
};


#endif

