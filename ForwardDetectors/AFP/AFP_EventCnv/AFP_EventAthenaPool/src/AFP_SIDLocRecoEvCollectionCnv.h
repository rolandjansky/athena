/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDLocRecoEvCollectionCnv_h
#define AFP_SIDLocRecoEvCollectionCnv_h 

#include "AFP_LocRecoEv/AFP_SIDLocRecoEvCollection.h"
#include "AFP_EventTPCnv/AFP_SIDLocRecoEvCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

 
typedef AFP_SIDLocRecoEvCollection_p1  AFP_SIDLocRecoEvCollection_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;
 
class AFP_SIDLocRecoEvCollectionCnv : public T_AthenaPoolCustomCnv <AFP_SIDLocRecoEvCollection, AFP_SIDLocRecoEvCollection_PERS>
 {
   friend class CnvFactory<AFP_SIDLocRecoEvCollectionCnv>;
 
 protected: 
  
  AFP_SIDLocRecoEvCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <AFP_SIDLocRecoEvCollection, AFP_SIDLocRecoEvCollection_PERS >(svcloc) {} 

  AFP_SIDLocRecoEvCollection_PERS*  createPersistent (AFP_SIDLocRecoEvCollection *transCont);
  AFP_SIDLocRecoEvCollection*       createTransient ();
    
 };
 
#endif // AFP_SIDLocRecoEvCollectionCnv_h
