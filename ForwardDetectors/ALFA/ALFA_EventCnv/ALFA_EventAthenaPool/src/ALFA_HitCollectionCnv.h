/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_HitCollectionCnv_h
#define ALFA_HitCollectionCnv_h 

#include "ALFA_SimEv/ALFA_HitCollection.h"
#include "ALFA_EventTPCnv/ALFA_HitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

//#include "AthenaPoolCnvSvc/AthenaPoolCnvTPExtension.h"  // for TL classes
//#include "AthenaPoolCnvSvc/T_AthenaPoolExtendingCnv.h"  // for TL classes
//#include "ALFA_SimEvTPCnv/ALFA_HitCollectionCnv_tlp1.h" // for TL classes

// Gaudi
#include "GaudiKernel/MsgStream.h"
 
// typedef to the latest persistent version

// typedef ALFA_HitCollection_tlp1  ALFA_HitCollection_PERS; // for TL classes
typedef ALFA_HitCollection_p1  ALFA_HitCollection_PERS;


//typedef T_AthenaPoolCustomCnv < ALFA_HitCollection, ALFA_HitCollection_PERS > ALFA_HitCollectionCnvBase;
//typedef T_AthenaPoolExtendingCnv < ALFA_HitCollection, ALFA_HitCollection_PERS > ALFA_HitCollectionCnvBase; // for TL classes


/*class ALFA_HitCollectionCnv : public ALFA_HitCollectionCnvBase //, public AthenaPoolCnvTPExtension
 {
   friend class CnvFactory<ALFA_HitCollectionCnv>;
 
 protected:  
  
  
  ALFA_HitCollectionCnv(ISvcLocator* svcloc) : ALFA_HitCollectionCnvBase(svcloc) {} 
  ~ALFA_HitCollectionCnv(); 
  
  virtual ALFA_HitCollection_PERS*  createPersistent (ALFA_HitCollection *transCont);
  virtual ALFA_HitCollection*       createTransient ();
  
  // virtual AthenaPoolTopLevelTPCnvBase*  getTopLevelTPCnv() { return &m_TPConverter; } // for TL classes
  
//  ALFA_HitCollectionCnv_p1 m_TPConverter;
    
 };*/
 
class ALFA_HitCollectionCnv : public T_AthenaPoolCustomCnv <ALFA_HitCollection, ALFA_HitCollection_PERS>
 {
   friend class CnvFactory<ALFA_HitCollectionCnv>;
 
 protected: 
  
  ALFA_HitCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_HitCollection, ALFA_HitCollection_PERS >(svcloc) {} 

  ALFA_HitCollection_PERS*  createPersistent (ALFA_HitCollection *transCont);
  ALFA_HitCollection*       createTransient ();
    
 };
 
#endif
