/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATACNV_P1_H
#define AFP_RAWDATACNV_P1_H
	
#include "AFP_RawData_p1.h"
#include "AFP_RawEv/AFP_RawData.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
	
class MsgStream;
class AFP_RawDataCnv_p1: public T_AthenaPoolTPCnvConstBase<AFP_RawData, AFP_RawData_p1> {

 public:
  using base_class::persToTrans;
  using base_class::transToPers;


  AFP_RawDataCnv_p1() {}

  virtual void persToTrans(const AFP_RawData_p1* persObj, AFP_RawData*    transObj, MsgStream &log) const override;
  virtual void transToPers(const AFP_RawData*   transObj, AFP_RawData_p1* persObj , MsgStream &log) const override;
};

#endif
