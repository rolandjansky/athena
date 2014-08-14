/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGPHOTONCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGPHOTONCONTAINER_CNV_H
 
 
#include "GaudiKernel/MsgStream.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigParticleTPCnv/TrigPhotonContainer_p3.h"

typedef TrigPhotonContainer_p3   TrigPhotonContainer_PERS;
 
typedef T_AthenaPoolCustomCnv<TrigPhotonContainer, TrigPhotonContainer_PERS> TrigPhotonContainerCnvBase;
 
class TrigPhotonContainerCnv : public TrigPhotonContainerCnvBase {
  
  friend class CnvFactory<TrigPhotonContainerCnv>;
 
  protected:
   
  TrigPhotonContainerCnv( ISvcLocator *svcloc ) : TrigPhotonContainerCnvBase(svcloc){}
   
   virtual TrigPhotonContainer_PERS *createPersistent( TrigPhotonContainer *transObj);
   virtual TrigPhotonContainer      *createTransient();
 
};//end of class definitions
  
 
 #endif //TRIGPHOTONCONTAINER_CNV_H
