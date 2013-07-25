/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_GloRecEvCollectionCnv_h
#define ALFA_GloRecEvCollectionCnv_h 

#include "ALFA_GloRecEv/ALFA_GloRecEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_GloRecEvCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

 
typedef ALFA_GloRecEvCollection_p1  ALFA_GloRecEvCollection_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;
 
class ALFA_GloRecEvCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_GloRecEvCollection, ALFA_GloRecEvCollection_PERS>
 {
   friend class CnvFactory<ALFA_GloRecEvCollectionCnv>;
 
 protected: 
  
  ALFA_GloRecEvCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_GloRecEvCollection, ALFA_GloRecEvCollection_PERS >(svcloc) {} 

  ALFA_GloRecEvCollection_PERS*  createPersistent (ALFA_GloRecEvCollection *transCont);
  ALFA_GloRecEvCollection*       createTransient ();
    
 };
 
#endif // ALFA_GLoRecEvCollectionCnv_h
