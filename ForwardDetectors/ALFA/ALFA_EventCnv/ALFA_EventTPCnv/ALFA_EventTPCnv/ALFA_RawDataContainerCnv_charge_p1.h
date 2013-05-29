/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFAEVENTATHENAPOOL_ALFACONTAINERCNV_CHARGE_P1_H
#define ALFAEVENTATHENAPOOL_ALFACONTAINERCNV_CHARGE_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ALFA_RawEv/ALFA_RawDataContainer_charge.h"
#include "ALFA_RawDataCollectionCnv_charge_p1.h"
#include "ALFA_RawDataContainer_charge_p1.h"


class MsgStream;

typedef T_AthenaPoolTPCnvVector< ALFA_RawDataContainer_charge, ALFA_RawDataContainer_charge_p1, ALFA_RawDataCollectionCnv_charge_p1 > ALFA_RawDataContainer_charge_Cnvp1_base_t;


class ALFA_RawDataContainerCnv_charge_p1
  : public ALFA_RawDataContainer_charge_Cnvp1_base_t
 {
 public:
   ALFA_RawDataContainerCnv_charge_p1() {}
   
   virtual void persToTrans(const ALFA_RawDataContainer_charge_p1* persCont, ALFA_RawDataContainer_charge*   transCont, MsgStream& log);
   virtual void transToPers(const ALFA_RawDataContainer_charge*   transCont, ALFA_RawDataContainer_charge_p1* persCont, MsgStream& log);
};

#endif
