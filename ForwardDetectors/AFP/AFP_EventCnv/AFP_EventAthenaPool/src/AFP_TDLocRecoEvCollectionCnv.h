/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDLocRecoEvCollectionCnv_h
#define AFP_TDLocRecoEvCollectionCnv_h 

#include "AFP_LocRecoEv/AFP_TDLocRecoEvCollection.h"
#include "AFP_EventTPCnv/AFP_TDLocRecoEvCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

 
typedef AFP_TDLocRecoEvCollection_p1  AFP_TDLocRecoEvCollection_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;
 
class AFP_TDLocRecoEvCollectionCnv : public T_AthenaPoolCustomCnv <AFP_TDLocRecoEvCollection, AFP_TDLocRecoEvCollection_PERS>
 {
   friend class CnvFactory<AFP_TDLocRecoEvCollectionCnv>;
 
 protected: 
  
  AFP_TDLocRecoEvCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <AFP_TDLocRecoEvCollection, AFP_TDLocRecoEvCollection_PERS >(svcloc) {} 

  AFP_TDLocRecoEvCollection_PERS*  createPersistent (AFP_TDLocRecoEvCollection *transCont);
  AFP_TDLocRecoEvCollection*       createTransient ();
    
 };
 
#endif // AFP_TDLocRecoEvCollectionCnv_h
