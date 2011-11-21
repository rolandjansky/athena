/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RAWDATACONTAINERCNV_P1_H
#define LUCID_RAWDATACONTAINERCNV_P1_H

#include "LUCID_RawDataContainer_p1.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"
#include "LUCID_EventTPCnv/LUCID_RawDataCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

class LUCID_RawDataContainerCnv_p1: public T_AthenaPoolTPCnvBase<LUCID_RawDataContainer, LUCID_RawDataContainer_p1> {
  
 public:
  
  LUCID_RawDataContainerCnv_p1() {}
  
  virtual void persToTrans(const LUCID_RawDataContainer_p1* persObj, LUCID_RawDataContainer*   transObj, MsgStream& log);
  virtual void transToPers(const LUCID_RawDataContainer*   transObj, LUCID_RawDataContainer_p1* persObj, MsgStream& log);
};

template<> class T_TPCnv<LUCID_RawDataContainer, LUCID_RawDataContainer_p1>: public LUCID_RawDataContainerCnv_p1 {
  
 public:
};

#endif 
