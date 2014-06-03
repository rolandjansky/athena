/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEHITCONTAINERCNV
#define TILEHITCONTAINERCNV

#include "TileEvent/TileHitContainer.h"
#include "TileTPCnv/TileHitContainer_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// typedef to the latest persistent version
typedef TileHitContainer_p1  TileHitContainer_PERS;

class TileHitContainerCnv  : public T_AthenaPoolCustomCnv<TileHitContainer, TileHitContainer_PERS > {
  friend class CnvFactory<TileHitContainerCnv>;
protected:
  TileHitContainerCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<TileHitContainer, TileHitContainer_PERS >( svcloc) {}
  TileHitContainer_PERS*  createPersistent(TileHitContainer* transCont);
  TileHitContainer*       createTransient ();
};

#endif
