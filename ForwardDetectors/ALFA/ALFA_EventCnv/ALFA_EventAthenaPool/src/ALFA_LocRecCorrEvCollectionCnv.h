/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_LocRecCorrEvCollectionCnv_h
#define ALFA_LocRecCorrEvCollectionCnv_h 

#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvCollection.h"
#include "ALFA_EventTPCnv/ALFA_LocRecCorrEvCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

 
typedef ALFA_LocRecCorrEvCollection_p1  ALFA_LocRecCorrEvCollection_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;
 
class ALFA_LocRecCorrEvCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_LocRecCorrEvCollection, ALFA_LocRecCorrEvCollection_PERS>
 {
   friend class CnvFactory<ALFA_LocRecCorrEvCollectionCnv>;
 
 protected: 
  
  ALFA_LocRecCorrEvCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_LocRecCorrEvCollection, ALFA_LocRecCorrEvCollection_PERS >(svcloc) {} 

  ALFA_LocRecCorrEvCollection_PERS*  createPersistent (ALFA_LocRecCorrEvCollection *transCont);
  ALFA_LocRecCorrEvCollection*       createTransient ();
    
 };
 
#endif // ALFA_DigitCollectionCnv_h
