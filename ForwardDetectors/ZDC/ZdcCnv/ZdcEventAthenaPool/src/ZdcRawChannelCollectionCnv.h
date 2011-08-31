/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCRAWCHANNELCOLLECTIONCNV
#define ZDCRAWCHANNELCOLLECTIONCNV

#include "ZdcEvent/ZdcRawChannelCollection.h"
#include "ZdcEventTPCnv/ZdcRawChannelCollection_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// typedef to the latest persistent version
typedef ZdcRawChannelCollection_p1  ZdcRawChannelCollection_PERS;

class ZdcRawChannelCollectionCnv  : public T_AthenaPoolCustomCnv<ZdcRawChannelCollection, ZdcRawChannelCollection_PERS > {
  friend class CnvFactory<ZdcRawChannelCollectionCnv>;
protected:
  ZdcRawChannelCollectionCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<ZdcRawChannelCollection, ZdcRawChannelCollection_PERS >( svcloc) {}
  ZdcRawChannelCollection_PERS*  createPersistent(ZdcRawChannelCollection* transCont);
  ZdcRawChannelCollection*       createTransient ();
};


#endif
