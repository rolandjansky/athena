/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RawDataContainerCnv_H
#define ALFA_RawDataContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "ALFA_RawEv/ALFA_RawDataContainer.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainerCnv_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainer_p1.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/CnvFactory.h"


typedef ALFA_RawDataContainer_p1 ALFA_RawDataContainer_PERS;

typedef T_AthenaPoolCustomCnv<ALFA_RawDataContainer,ALFA_RawDataContainer_PERS> ALFA_RawDataContainerCnvBase;


class ALFA_RawDataContainerCnv: public ALFA_RawDataContainerCnvBase {

  
  friend class CnvFactory<ALFA_RawDataContainerCnv>;
  
 protected:
  
  ALFA_RawDataContainerCnv(ISvcLocator* svcloc): ALFA_RawDataContainerCnvBase(svcloc) {}
    
    virtual ALFA_RawDataContainer_PERS* createPersistent(ALFA_RawDataContainer* transCont);
    virtual ALFA_RawDataContainer*      createTransient ();
    
   
};

#endif


