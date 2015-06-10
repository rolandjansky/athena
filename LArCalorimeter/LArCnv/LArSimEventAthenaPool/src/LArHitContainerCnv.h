/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHITCONTAINERCNV
#define LARHITCONTAINERCNV

#include "LArSimEvent/LArHitContainer.h"
#include "LArSimEventTPCnv/LArHitContainer_p1.h"
#include "LArSimEventTPCnv/LArHitContainer_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef LArHitContainer_p2  LArHitContainer_PERS;

class LArHitContainerCnv  : public T_AthenaPoolCustomCnv<LArHitContainer, LArHitContainer_PERS > {
  friend class CnvFactory<LArHitContainerCnv>;
protected:
  LArHitContainerCnv(ISvcLocator* svcloc) :  T_AthenaPoolCustomCnv<LArHitContainer, LArHitContainer_PERS >( svcloc) {}
  LArHitContainer_PERS*  createPersistent(LArHitContainer* transCont);
  LArHitContainer*       createTransient ();
};

//#include "LArHitContainerCnv.icc"

#endif
