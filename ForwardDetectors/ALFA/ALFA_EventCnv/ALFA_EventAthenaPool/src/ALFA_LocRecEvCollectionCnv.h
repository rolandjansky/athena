/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecEvCollectionCnv_h
#define ALFA_LocRecEvCollectionCnv_h 

#include "ALFA_LocRecEv/ALFA_LocRecEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_LocRecEvCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

 
typedef ALFA_LocRecEvCollection_p1  ALFA_LocRecEvCollection_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;
 
class ALFA_LocRecEvCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_LocRecEvCollection, ALFA_LocRecEvCollection_PERS>
 {
   friend class CnvFactory<ALFA_LocRecEvCollectionCnv>;
 
 protected: 
  
  ALFA_LocRecEvCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_LocRecEvCollection, ALFA_LocRecEvCollection_PERS >(svcloc) {} 

  ALFA_LocRecEvCollection_PERS*  createPersistent (ALFA_LocRecEvCollection *transCont);
  ALFA_LocRecEvCollection*       createTransient ();
    
 };
 
#endif // ALFA_DigitCollectionCnv_h
