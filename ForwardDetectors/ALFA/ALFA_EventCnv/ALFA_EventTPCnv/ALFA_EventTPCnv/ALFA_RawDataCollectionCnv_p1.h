/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATACOLLECTIONCNV_P1_H
#define ALFA_RAWDATACOLLECTIONCNV_P1_H

#include "ALFA_RawDataCollection_p1.h"
#include "ALFA_RawEv/ALFA_RawDataCollection.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class MsgStream;

typedef  T_AthenaPoolTPCnvVector< ALFA_RawDataCollection, ALFA_RawDataCollection_p1, ALFA_RawDataCnv_p1>  ALFA_RawDataCollection_Cnvp1_base_t;

class ALFA_RawDataCollectionCnv_p1:  public ALFA_RawDataCollection_Cnvp1_base_t{

//public T_AthenaPoolTPCnvBase<ALFA_RawDataCollection, ALFA_RawDataCollection_p1> {
  
 public:
  
  ALFA_RawDataCollectionCnv_p1() {}
  
  virtual void persToTrans(const ALFA_RawDataCollection_p1* persColl, ALFA_RawDataCollection*   transColl, MsgStream& log);
  virtual void transToPers(const ALFA_RawDataCollection*   transColl, ALFA_RawDataCollection_p1* persColl, MsgStream& log);
};

/*template<> class T_TPCnv<ALFA_RawDataCollection, ALFA_RawDataCollection_p1>: public ALFA_RawDataCollectionCnv_p1 {
  
 public:
 };
*/


#endif 
