/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_DigitContainerCnv_H
#define LUCID_DigitContainerCnv_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "LUCID_RawEvent/LUCID_DigitContainer.h"
#include "LUCID_EventTPCnv/LUCID_DigitContainerCnv_p2.h"
#include "GaudiKernel/MsgStream.h"

typedef LUCID_DigitContainer_p2 LUCID_DigitContainer_PERS;

typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;

class LUCID_DigitContainerCnv: public LUCID_DigitContainerCnvBase {
  
  friend class CnvFactory<LUCID_DigitContainerCnv>;
  
 protected:
  
  LUCID_DigitContainerCnv(ISvcLocator* svcloc): LUCID_DigitContainerCnvBase(svcloc) {}
    
    virtual LUCID_DigitContainer_PERS* createPersistent(LUCID_DigitContainer* transCont);
    virtual LUCID_DigitContainer*      createTransient ();
};

#endif


