/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGELECTRONCONTAINERCNV_H
#define TRIGEVENTATHENAPOOL_TRIGELECTRONCONTAINERCNV_H

#include "GaudiKernel/MsgStream.h"
// transient class
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticleTPCnv/TrigElectronContainer_p3.h"

typedef TrigElectronContainer_p3  TrigElectronContainer_PERS;
typedef T_AthenaPoolCustomCnv<TrigElectronContainer, TrigElectronContainer_PERS> TrigElectronContainerCnvBase;

class TrigElectronContainerCnv  : public TrigElectronContainerCnvBase { 

  friend class CnvFactory<TrigElectronContainerCnv>;

  protected:
  
   TrigElectronContainerCnv(ISvcLocator* svcloc): TrigElectronContainerCnvBase(svcloc){}

   virtual TrigElectronContainer_PERS*  createPersistent(TrigElectronContainer* transCont);
   virtual TrigElectronContainer*       createTransient ();

};


#endif
