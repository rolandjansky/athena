/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODHitCollectionCnv_h
#define ALFA_ODHitCollectionCnv_h 

#include "ALFA_SimEv/ALFA_ODHitCollection.h"
#include "ALFA_EventTPCnv/ALFA_ODHitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"


// Gaudi
#include "GaudiKernel/MsgStream.h"
 
// typedef to the latest persistent version

typedef ALFA_ODHitCollection_p1  ALFA_ODHitCollection_PERS;

 
class ALFA_ODHitCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_ODHitCollection, ALFA_ODHitCollection_PERS>
 {
   friend class CnvFactory<ALFA_ODHitCollectionCnv>;
 
 protected: 
  
  ALFA_ODHitCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_ODHitCollection, ALFA_ODHitCollection_PERS >(svcloc) {} 

  ALFA_ODHitCollection_PERS*  createPersistent (ALFA_ODHitCollection *transCont);
  ALFA_ODHitCollection*       createTransient ();
    
 };
 
#endif
