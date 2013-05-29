/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFAEVENTATHENAPOOL_ALFACONTAINERCNV_P1_H
#define ALFAEVENTATHENAPOOL_ALFACONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_RawDataCollectionCnv_p1.h"
#include "ALFA_RawDataContainer_p1.h"


class MsgStream;

typedef T_AthenaPoolTPCnvVector< ALFA_RawDataContainer, ALFA_RawDataContainer_p1, ALFA_RawDataCollectionCnv_p1 > ALFA_RawDataContainer_Cnvp1_base_t;


class ALFA_RawDataContainerCnv_p1
  : public ALFA_RawDataContainer_Cnvp1_base_t
 {
 public:
   ALFA_RawDataContainerCnv_p1() {}
   
   virtual void persToTrans(const ALFA_RawDataContainer_p1* persCont, ALFA_RawDataContainer*   transCont, MsgStream& log);
   virtual void transToPers(const ALFA_RawDataContainer*   transCont, ALFA_RawDataContainer_p1* persCont, MsgStream& log);
};

#endif
