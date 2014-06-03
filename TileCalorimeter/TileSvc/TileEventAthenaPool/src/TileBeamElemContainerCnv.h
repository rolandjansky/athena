/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEBEAMELEMCONTAINERCNV
#define TILEBEAMELEMCONTAINERCNV

#include "TileEvent/TileBeamElemContainer.h"
#include "TileTPCnv/TileBeamElemContainer_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// typedef to the latest persistent version
typedef TileBeamElemContainer_p1  TileBeamElemContainer_PERS;

class TileBeamElemContainerCnv  : public T_AthenaPoolCustomCnv<TileBeamElemContainer, TileBeamElemContainer_PERS > {
  friend class CnvFactory<TileBeamElemContainerCnv>;
protected:
  TileBeamElemContainerCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<TileBeamElemContainer, TileBeamElemContainer_PERS >( svcloc) {}
  TileBeamElemContainer_PERS*  createPersistent(TileBeamElemContainer* transCont);
  TileBeamElemContainer*       createTransient ();
};


#endif


