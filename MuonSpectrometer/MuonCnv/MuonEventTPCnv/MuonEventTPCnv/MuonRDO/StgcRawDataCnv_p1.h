/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef STGCRAWDATACNV_P1_H
#define STGCRAWDATACNV_P1_H

//-----------------------------------------------------------------------------
//
// file:   sTgcPrepDataCnv_p1.h
//
//-----------------------------------------------------------------------------
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "MuonRDO/sTGC_RawData.h"
#include "StgcRawData_p1.h"

class MsgStream;

namespace Muon {
  class StgcRawDataCnv_p1
     : public T_AthenaPoolTPCnvBase< sTGC_RawData, StgcRawData_p1 >
  {
  public:
    StgcRawDataCnv_p1() {}

    void persToTrans( const StgcRawData_p1 *persObj, sTGC_RawData* transObj, MsgStream &log ) override final;
                    
    void transToPers( const sTGC_RawData* transObj, StgcRawData_p1 *persObj, MsgStream &log ) override final;
  
    virtual sTGC_RawData* createTransient(const StgcRawData_p1* persObj, MsgStream& log) override final;
  };
}

#endif
