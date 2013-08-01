/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_RawDataContainerCnv_H
#define LUCID_RawDataContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "LUCID_RawEvent/LUCID_RawDataContainer.h"
#include "LUCID_EventTPCnv/LUCID_RawDataContainerCnv_p1.h"
#include "GaudiKernel/MsgStream.h"

typedef LUCID_RawDataContainer_p1 LUCID_RawDataContainer_PERS;

typedef T_AthenaPoolCustomCnv < LUCID_RawDataContainer, LUCID_RawDataContainer_PERS > LUCID_RawDataContainerCnvBase;

class LUCID_RawDataContainerCnv: public LUCID_RawDataContainerCnvBase {
  
  friend class CnvFactory<LUCID_RawDataContainerCnv>;
  
 protected:
  
  LUCID_RawDataContainerCnv(ISvcLocator* svcloc): LUCID_RawDataContainerCnvBase(svcloc) {}
    
    virtual LUCID_RawDataContainer_PERS* createPersistent(LUCID_RawDataContainer* transCont);
    virtual LUCID_RawDataContainer*      createTransient ();
};

#endif


