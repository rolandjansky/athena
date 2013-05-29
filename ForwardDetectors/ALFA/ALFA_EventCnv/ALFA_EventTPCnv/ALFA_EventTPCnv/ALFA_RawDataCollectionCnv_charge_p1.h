/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RAWDATACOLLECTIONCNV_CHARGE_P1_H
#define ALFA_RAWDATACOLLECTIONCNV_CHARGE_P1_H

#include "ALFA_RawDataCollection_charge_p1.h"
#include "ALFA_RawEv/ALFA_RawDataCollection_charge.h"
#include "ALFA_EventTPCnv/ALFA_RawDataCnv_charge_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"


class MsgStream;

typedef  T_AthenaPoolTPCnvVector< ALFA_RawDataCollection_charge, ALFA_RawDataCollection_charge_p1, ALFA_RawDataCnv_charge_p1>  ALFA_RawDataCollection_charge_Cnvp1_base_t;

class ALFA_RawDataCollectionCnv_charge_p1:  public ALFA_RawDataCollection_charge_Cnvp1_base_t{

//public T_AthenaPoolTPCnvBase<ALFA_RawDataCollection_charge, ALFA_RawDataCollection_charge_p1> {
  
 public:
  
  ALFA_RawDataCollectionCnv_charge_p1() {}
  
  virtual void persToTrans(const ALFA_RawDataCollection_charge_p1* persColl, ALFA_RawDataCollection_charge*   transColl, MsgStream& log);
  virtual void transToPers(const ALFA_RawDataCollection_charge*   transColl, ALFA_RawDataCollection_charge_p1* persColl, MsgStream& log);
};


#endif 
