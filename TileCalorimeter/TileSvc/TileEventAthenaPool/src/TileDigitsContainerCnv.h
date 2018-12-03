/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILEDIGITSCONTAINERCNV
#define TILEDIGITSCONTAINERCNV

#include "TileEvent/TileDigitsContainer.h"
#include "TileTPCnv/TileDigitsContainer_p1.h"
#include "TileTPCnv/TileDigitsContainer_p2.h"
#include "TileTPCnv/TileDigitsContainer_p3.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// typedef to the latest persistent version
typedef TileDigitsContainer_p3  TileDigitsContainer_PERS;

class TileDigitsContainerCnv  : public T_AthenaPoolCustomCnv<TileDigitsContainer, TileDigitsContainer_PERS > {
  friend class CnvFactory<TileDigitsContainerCnv>;
protected:
  TileDigitsContainerCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<TileDigitsContainer, TileDigitsContainer_PERS >( svcloc) {}
  TileDigitsContainer_PERS*  createPersistent(TileDigitsContainer* transCont);
  TileDigitsContainer*       createTransient ();
};


#endif
