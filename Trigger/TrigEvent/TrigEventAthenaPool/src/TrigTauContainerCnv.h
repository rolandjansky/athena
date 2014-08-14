/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGTAUCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGTAUCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
 
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigParticle/TrigTauContainer.h"
#include "TrigParticleTPCnv/TrigTauContainerCnv_p3.h"
 
typedef TrigTauContainer_p3   TrigTauContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigTauContainer, TrigTauContainer_PERS> TrigTauContainerCnvBase;
 
class TrigTauContainerCnv : public TrigTauContainerCnvBase
 {
  
  friend class CnvFactory<TrigTauContainerCnv>;
 
  protected:
   
  TrigTauContainerCnv( ISvcLocator *svcloc ): TrigTauContainerCnvBase(svcloc){}
 
   
   virtual TrigTauContainer_PERS *createPersistent( TrigTauContainer *transObj);
   virtual TrigTauContainer      *createTransient();
 
  };//end of class definitions
  
 
 #endif //TRIGTAUCONTAINER_CNV_H
