/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGCOMPOSITECONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGCOMPOSITECONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigCombinedEvent/TrigCompositeContainer.h"
#include "TrigCombinedEventTPCnv/TrigCompositeContainerCnv_p1.h"
 
typedef TrigCompositeContainer_p1   TrigCompositeContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigCompositeContainer, TrigCompositeContainer_PERS> TrigCompositeContainerCnvBase;
 
class TrigCompositeContainerCnv : public TrigCompositeContainerCnvBase
 {
  
  friend class CnvFactory<TrigCompositeContainerCnv>;
 
  protected:
   
  TrigCompositeContainerCnv( ISvcLocator *svcloc ): TrigCompositeContainerCnvBase(svcloc){}
 
   
   virtual TrigCompositeContainer_PERS *createPersistent( TrigCompositeContainer *transObj);
   virtual TrigCompositeContainer      *createTransient();
 
  };//end of class definitions
  
 
 #endif //TRIGCOMPOSITECONTAINER_CNV_H
