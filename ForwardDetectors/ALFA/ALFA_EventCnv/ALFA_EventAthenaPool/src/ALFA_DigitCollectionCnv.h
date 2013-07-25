/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_DigitCollectionCnv_h
#define ALFA_DigitCollectionCnv_h 

#include "ALFA_RawEv/ALFA_DigitCollection.h"
#include "ALFA_EventTPCnv/ALFA_DigitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

 
typedef ALFA_DigitCollection_p1  ALFA_DigitCollection_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;
 
class ALFA_DigitCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_DigitCollection, ALFA_DigitCollection_PERS>
 {
   friend class CnvFactory<ALFA_DigitCollectionCnv>;
 
 protected: 
  
  ALFA_DigitCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_DigitCollection, ALFA_DigitCollection_PERS >(svcloc) {} 

  ALFA_DigitCollection_PERS*  createPersistent (ALFA_DigitCollection *transCont);
  ALFA_DigitCollection*       createTransient ();
    
 };
 
#endif // ALFA_DigitCollectionCnv_h
