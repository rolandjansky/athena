/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFPEVENTATHENAPOOL_AFPCONTAINERCNV_P1_H
#define AFPEVENTATHENAPOOL_AFPCONTAINERCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "AFP_RawEv/AFP_RawDataContainer.h"
#include "AFP_RawDataCollectionCnv_p1.h"
#include "AFP_RawDataContainer_p1.h"


class MsgStream;
	
typedef T_AthenaPoolTPCnvVector< AFP_RawDataContainer, AFP_RawDataContainer_p1, AFP_RawDataCollectionCnv_p1 > AFP_RawDataContainer_Cnvp1_base_t;

	
class AFP_RawDataContainerCnv_p1
  : public AFP_RawDataContainer_Cnvp1_base_t
 {
 public:
   AFP_RawDataContainerCnv_p1() {}
   
   virtual void persToTrans(const AFP_RawDataContainer_p1* persCont, AFP_RawDataContainer*   transCont, MsgStream& log);
   virtual void transToPers(const AFP_RawDataContainer*   transCont, AFP_RawDataContainer_p1* persCont, MsgStream& log);
};
#endif
