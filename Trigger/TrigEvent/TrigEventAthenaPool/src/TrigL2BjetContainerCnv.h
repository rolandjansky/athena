/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGL2BJETCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGL2BJETCONTAINER_CNV_H

#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticleTPCnv/TrigL2BjetContainer_p3.h"

typedef TrigL2BjetContainer_p3   TrigL2BjetContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigL2BjetContainer, TrigL2BjetContainer_PERS> TrigL2BjetContainerCnvBase;
 
class TrigL2BjetContainerCnv : public TrigL2BjetContainerCnvBase {
  
  friend class CnvFactory<TrigL2BjetContainerCnv>;
 
  protected:
   
   TrigL2BjetContainerCnv( ISvcLocator *svcloc ) : TrigL2BjetContainerCnvBase(svcloc){}
  
   virtual TrigL2BjetContainer_PERS *createPersistent( TrigL2BjetContainer *transObj);
   virtual TrigL2BjetContainer      *createTransient();

 };//end of class definitions
  
 
#endif 
