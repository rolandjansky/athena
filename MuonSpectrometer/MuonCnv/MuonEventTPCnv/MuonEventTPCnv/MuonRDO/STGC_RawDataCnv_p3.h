/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGC_RAWDATACNV_P3_H
#define STGC_RAWDATACNV_P3_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonRDO/STGC_RawData.h"
#include "STGC_RawData_p3.h"

class MsgStream;

namespace Muon {
  class STGC_RawDataCnv_p3
     : public T_AthenaPoolTPCnvBase< STGC_RawData, STGC_RawData_p3 >
  {
  public:
    STGC_RawDataCnv_p3() {}

    void persToTrans( const STGC_RawData_p3 *persObj, STGC_RawData* transObj, MsgStream &log ) override final;
                    
    void transToPers( const STGC_RawData* transObj, STGC_RawData_p3 *persObj, MsgStream &log ) override final;
  
    virtual STGC_RawData* createTransient(const STGC_RawData_p3* persObj, MsgStream& log) override final;
  };
}

#endif
