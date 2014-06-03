/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TILERAWCHANNELCONTAINERCNV
#define TILERAWCHANNELCONTAINERCNV

#include "TileEvent/TileRawChannelContainer.h"
#include "TileTPCnv/TileRawChannelContainer_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// typedef to the latest persistent version
typedef TileRawChannelContainer_p1  TileRawChannelContainer_PERS;

class TileRawChannelContainerCnv  : public T_AthenaPoolCustomCnv<TileRawChannelContainer, TileRawChannelContainer_PERS > {
  friend class CnvFactory<TileRawChannelContainerCnv>;
protected:
  TileRawChannelContainerCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<TileRawChannelContainer, TileRawChannelContainer_PERS >( svcloc) {}
  TileRawChannelContainer_PERS*  createPersistent(TileRawChannelContainer* transCont);
  TileRawChannelContainer*       createTransient ();
};


#endif
