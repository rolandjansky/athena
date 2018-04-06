/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_RAWDATACNV_P1_H
#define MM_RAWDATACNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonRDO/MM_RawData.h"
#include "MM_RawData_p1.h"

class MsgStream;

namespace Muon {
  class MM_RawDataCnv_p1
     : public T_AthenaPoolTPCnvBase< MM_RawData, MM_RawData_p1 >
  {
  public:
    MM_RawDataCnv_p1() {}

    void persToTrans( const MM_RawData_p1 *persObj, MM_RawData* transObj, MsgStream &log ) override final;
                    
    void transToPers( const MM_RawData* transObj, MM_RawData_p1 *persObj, MsgStream &log ) override final;
  
    virtual MM_RawData* createTransient(const MM_RawData_p1* persObj, MsgStream& log) override final;
  };
}

#endif
