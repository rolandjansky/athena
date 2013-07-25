/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrODEvCollectionCnv_h
#define ALFA_LocRecCorrODEvCollectionCnv_h 

#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrODEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrODEvCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

 
typedef ALFA_LocRecCorrODEvCollection_p1  ALFA_LocRecCorrODEvCollection_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;
 
class ALFA_LocRecCorrODEvCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_LocRecCorrODEvCollection, ALFA_LocRecCorrODEvCollection_PERS>
 {
   friend class CnvFactory<ALFA_LocRecCorrODEvCollectionCnv>;
 
 protected: 
  
  ALFA_LocRecCorrODEvCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_LocRecCorrODEvCollection, ALFA_LocRecCorrODEvCollection_PERS >(svcloc) {} 

  ALFA_LocRecCorrODEvCollection_PERS*  createPersistent (ALFA_LocRecCorrODEvCollection *transCont);
  ALFA_LocRecCorrODEvCollection*       createTransient ();
    
 };
 
#endif // ALFA_DigitCollectionCnv_h
