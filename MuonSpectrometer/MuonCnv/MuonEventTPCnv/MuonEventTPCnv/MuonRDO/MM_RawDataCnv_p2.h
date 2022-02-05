/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MM_RAWDATACNV_P2_H
#define MM_RAWDATACNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonRDO/MM_RawData.h"
#include "MM_RawData_p2.h"

class MsgStream;

namespace Muon {
  class MM_RawDataCnv_p2
     : public T_AthenaPoolTPCnvBase< MM_RawData, MM_RawData_p2 >
  {
  public:
    MM_RawDataCnv_p2() {}

    void persToTrans( const MM_RawData_p2 *persObj, MM_RawData* transObj, MsgStream &log ) override final;
                    
    void transToPers( const MM_RawData* transObj, MM_RawData_p2 *persObj, MsgStream &log ) override final;
  
    virtual MM_RawData* createTransient(const MM_RawData_p2* persObj, MsgStream& log) override final;
  };
}

#endif
