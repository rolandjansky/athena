/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_HitCollectionCnv_h
#define ALFA_HitCollectionCnv_h 

#include "ALFA_SimEv/ALFA_HitCollection.h"
#include "ALFA_EventTPCnv/ALFA_HitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
 
// typedef to the latest persistent version
typedef ALFA_HitCollection_p1  ALFA_HitCollection_PERS;


 
class ALFA_HitCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_HitCollection, ALFA_HitCollection_PERS>
 {
   friend class CnvFactory<ALFA_HitCollectionCnv>;
 
 protected: 
  
  ALFA_HitCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_HitCollection, ALFA_HitCollection_PERS >(svcloc) {} 

  ALFA_HitCollection_PERS*  createPersistent (ALFA_HitCollection *transCont);
  ALFA_HitCollection*       createTransient ();
    
 };
 
#endif
