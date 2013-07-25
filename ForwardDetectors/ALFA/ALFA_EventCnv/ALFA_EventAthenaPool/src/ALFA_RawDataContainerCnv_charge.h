/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_RawDataContainerCnv_charge_H
#define ALFA_RawDataContainerCnv_charge_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "ALFA_RawEv/ALFA_RawDataContainer_charge.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainerCnv_charge_p1.h"
#include "ALFA_EventTPCnv/ALFA_RawDataContainer_charge_p1.h"
#include "GaudiKernel/MsgStream.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/CnvFactory.h"


typedef ALFA_RawDataContainer_charge_p1 ALFA_RawDataContainer_charge_PERS;

typedef T_AthenaPoolCustomCnv<ALFA_RawDataContainer_charge,ALFA_RawDataContainer_charge_PERS> ALFA_RawDataContainerCnvBase_charge;


class ALFA_RawDataContainerCnv_charge: public ALFA_RawDataContainerCnvBase_charge {

  
  friend class CnvFactory<ALFA_RawDataContainerCnv_charge>;
  
 protected:
  
  ALFA_RawDataContainerCnv_charge(ISvcLocator* svcloc): ALFA_RawDataContainerCnvBase_charge(svcloc) {}
    
    virtual ALFA_RawDataContainer_charge_PERS* createPersistent(ALFA_RawDataContainer_charge* transCont);
    virtual ALFA_RawDataContainer_charge*      createTransient ();
    
   
};

#endif


