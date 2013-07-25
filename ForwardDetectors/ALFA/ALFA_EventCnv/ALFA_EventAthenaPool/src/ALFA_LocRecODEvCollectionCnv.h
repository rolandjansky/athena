/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecODEvCollectionCnv_h
#define ALFA_LocRecODEvCollectionCnv_h 

#include "ALFA_LocRecEv/ALFA_LocRecODEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_LocRecODEvCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

 
typedef ALFA_LocRecODEvCollection_p1  ALFA_LocRecODEvCollection_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;
 
class ALFA_LocRecODEvCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_LocRecODEvCollection, ALFA_LocRecODEvCollection_PERS>
 {
   friend class CnvFactory<ALFA_LocRecODEvCollectionCnv>;
 
 protected: 
  
  ALFA_LocRecODEvCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_LocRecODEvCollection, ALFA_LocRecODEvCollection_PERS >(svcloc) {} 

  ALFA_LocRecODEvCollection_PERS*  createPersistent (ALFA_LocRecODEvCollection *transCont);
  ALFA_LocRecODEvCollection*       createTransient ();
    
 };
 
#endif // ALFA_DigitCollectionCnv_h
