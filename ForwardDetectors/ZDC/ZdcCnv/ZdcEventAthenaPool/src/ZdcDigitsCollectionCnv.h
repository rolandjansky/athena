/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZDCDIGITSCOLLECTIONCNV
#define ZDCDIGITSCOLLECTIONCNV

#include "ZdcEvent/ZdcDigitsCollection.h"
#include "ZdcEventTPCnv/ZdcDigitsCollection_p1.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// typedef to the latest persistent version
typedef ZdcDigitsCollection_p1  ZdcDigitsCollection_PERS;

typedef T_AthenaPoolCustomCnv<ZdcDigitsCollection, ZdcDigitsCollection_PERS > ZdcDigitsCollectionCnvBase;

class ZdcDigitsCollectionCnv  : public ZdcDigitsCollectionCnvBase {
  friend class CnvFactory<ZdcDigitsCollectionCnv>;
protected:
  ZdcDigitsCollectionCnv(ISvcLocator* svcloc) : ZdcDigitsCollectionCnvBase ( svcloc) {}
  virtual ZdcDigitsCollection_PERS*  createPersistent(ZdcDigitsCollection* transCont);
  virtual ZdcDigitsCollection*       createTransient ();
};


#endif
