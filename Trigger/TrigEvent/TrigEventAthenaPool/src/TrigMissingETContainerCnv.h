/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGMISSINGETCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGMISSINGETCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "TrigMissingEtEvent/TrigMissingETContainer.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_p3.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainerCnv_tlp1.h"
#include "TrigMissingEtEventTPCnv/TrigMissingETContainerCnv_p3.h"
 
typedef TrigMissingETContainer_p3   TrigMissingETContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigMissingETContainer, TrigMissingETContainer_PERS> TrigMissingETContainerCnvBase;
 
class TrigMissingETContainerCnv : public TrigMissingETContainerCnvBase {
  
  friend class CnvFactory<TrigMissingETContainerCnv>;
 
  protected:
   
public:
  TrigMissingETContainerCnv( ISvcLocator *svcloc ) : TrigMissingETContainerCnvBase(svcloc) {}
protected:
   
   virtual TrigMissingETContainer_PERS *createPersistent( TrigMissingETContainer *transObj);
   virtual TrigMissingETContainer      *createTransient();
private:
   TrigMissingETContainerCnv_tlp1 m_converter_tlp1;
   TrigMissingETContainerCnv_p3   m_converter;

  
};//end of class definitions
  
 
#endif
