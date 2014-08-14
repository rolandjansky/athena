/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGMISSINGETCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGMISSINGETCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p3.h"
 
typedef TrigMissingETContainer_p3   TrigMissingETContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigMissingETContainer, TrigMissingETContainer_PERS> TrigMissingETContainerCnvBase;
 
class TrigMissingETContainerCnv : public TrigMissingETContainerCnvBase {
  
  friend class CnvFactory<TrigMissingETContainerCnv>;
 
  protected:
   
  TrigMissingETContainerCnv( ISvcLocator *svcloc ) : TrigMissingETContainerCnvBase(svcloc) {}
   
   virtual TrigMissingETContainer_PERS *createPersistent( TrigMissingETContainer *transObj);
   virtual TrigMissingETContainer      *createTransient();
  
};//end of class definitions
  
 
#endif
