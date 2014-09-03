/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MUON_CSCRAWDATACNV_P3_H
#define MUON_CSCRAWDATACNV_P3_H



#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"  
#include "MuonEventAthenaPool/CscRawData_p3.h"
#include "MuonRDO/CscRawData.h"

class MsgStream;

/**
Transient/Persistent converter for CscRawData class
*/
class CscRawDataCnv_p3  : public T_AthenaPoolTPCnvBase<CscRawData, CscRawData_p3>
{
public:
  CscRawDataCnv_p3() {}
  
  virtual void		persToTrans(const CscRawData_p3* persObj, CscRawData* transObj, MsgStream &log);
  virtual void		transToPers(const CscRawData* transObj, CscRawData_p3* persObj, MsgStream &log);
};


#endif

