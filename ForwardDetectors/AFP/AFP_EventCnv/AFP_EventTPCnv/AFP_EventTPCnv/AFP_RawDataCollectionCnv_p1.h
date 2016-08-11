/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_RAWDATACOLLECTIONCNV_P1_H
#define AFP_RAWDATACOLLECTIONCNV_P1_H
	
#include "AFP_RawDataCollection_p1.h"
#include "AFP_RawEv/AFP_RawDataCollection.h"
#include "AFP_EventTPCnv/AFP_RawDataCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

class MsgStream;

typedef  T_AthenaPoolTPCnvVector< AFP_RawDataCollection, AFP_RawDataCollection_p1, AFP_RawDataCnv_p1>  AFP_RawDataCollection_Cnvp1_base_t;

class AFP_RawDataCollectionCnv_p1:  public AFP_RawDataCollection_Cnvp1_base_t{

//public T_AthenaPoolTPCnvBase<AFP_RawDataCollection, AFP_RawDataCollection_p1> {
 
 public:
 
  AFP_RawDataCollectionCnv_p1() {}
 
  virtual void persToTrans(const AFP_RawDataCollection_p1* persColl, AFP_RawDataCollection*   transColl, MsgStream& log);
  virtual void transToPers(const AFP_RawDataCollection*   transColl, AFP_RawDataCollection_p1* persColl, MsgStream& log);
};
#endif 
