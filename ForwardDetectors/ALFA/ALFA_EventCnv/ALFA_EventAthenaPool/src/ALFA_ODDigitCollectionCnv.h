/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODDigitCollectionCnv_h
#define ALFA_ODDigitCollectionCnv_h 

#include "ALFA_RawEv/ALFA_ODDigitCollection.h"
#include "ALFA_EventTPCnv/ALFA_ODDigitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

 
typedef ALFA_ODDigitCollection_p1  ALFA_ODDigitCollection_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;
 
class ALFA_ODDigitCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_ODDigitCollection, ALFA_ODDigitCollection_PERS>
 {
   friend class CnvFactory<ALFA_ODDigitCollectionCnv>;
 
 protected: 
  
  ALFA_ODDigitCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_ODDigitCollection, ALFA_ODDigitCollection_PERS >(svcloc) {} 

  ALFA_ODDigitCollection_PERS*  createPersistent (ALFA_ODDigitCollection *transCont);
  ALFA_ODDigitCollection*       createTransient ();
    
 };
 
#endif // ALFA_ODDigitCollectionCnv_h
