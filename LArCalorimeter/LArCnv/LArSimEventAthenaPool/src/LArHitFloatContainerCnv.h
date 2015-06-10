/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARHITFLOATCONTAINERCNV
#define LARHITFLOATCONTAINERCNV

#include "LArSimEvent/LArHitFloatContainer.h"
#include "LArSimEventTPCnv/LArHitContainer_p1.h"
#include "LArSimEventTPCnv/LArHitContainer_p2.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
typedef LArHitContainer_p2  LArHitContainer_PERS;

class LArHitContainer;

class LArHitFloatContainerCnv  : public T_AthenaPoolCustomCnv<LArHitFloatContainer, LArHitContainer_PERS > {
  friend class CnvFactory<LArHitFloatContainerCnv>;
protected:
  LArHitFloatContainerCnv(ISvcLocator* svcloc) :  T_AthenaPoolCustomCnv<LArHitFloatContainer, LArHitContainer_PERS >( svcloc) {}
  LArHitContainer_PERS*  createPersistent(LArHitFloatContainer* transCont);
  LArHitFloatContainer*  createTransient ();
  LArHitFloatContainer* copyLArHitToFloat(const LArHitContainer* double_cont);
};

#endif
