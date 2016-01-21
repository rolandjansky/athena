/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGPASSBITSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGPASSBITSCONTAINER_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCollection_p1.h"

typedef TrigPassBitsCollection_p1   TrigPassBitsCollection_PERS;

typedef T_AthenaPoolCustomCnv<TrigPassBitsCollection, TrigPassBitsCollection_PERS> TrigPassBitsCollectionCnvBase;

class TrigPassBitsCollectionCnv : public TrigPassBitsCollectionCnvBase {

  friend class CnvFactory<TrigPassBitsCollectionCnv>;

 protected:

  TrigPassBitsCollectionCnv( ISvcLocator *svcloc ): TrigPassBitsCollectionCnvBase(svcloc){}

  virtual TrigPassBitsCollection_PERS *createPersistent( TrigPassBitsCollection *transObj);
  virtual TrigPassBitsCollection      *createTransient();

};

#endif //TRIGPASSBITSCONTAINER_CNV_H

