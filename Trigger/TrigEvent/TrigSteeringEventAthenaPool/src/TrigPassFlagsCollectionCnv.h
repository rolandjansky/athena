/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGPASSFLAGSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGPASSFLAGSCONTAINER_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigSteeringEvent/TrigPassFlagsCollection.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCollection_p1.h"

typedef TrigPassFlagsCollection_p1   TrigPassFlagsCollection_PERS;

typedef T_AthenaPoolCustomCnv<TrigPassFlagsCollection, TrigPassFlagsCollection_PERS> TrigPassFlagsCollectionCnvBase;

class TrigPassFlagsCollectionCnv : public TrigPassFlagsCollectionCnvBase {

  friend class CnvFactory<TrigPassFlagsCollectionCnv>;

 protected:

  TrigPassFlagsCollectionCnv( ISvcLocator *svcloc ): TrigPassFlagsCollectionCnvBase(svcloc){}

  virtual TrigPassFlagsCollection_PERS *createPersistent( TrigPassFlagsCollection *transObj);
  virtual TrigPassFlagsCollection      *createTransient();

};

#endif //TRIGPASSBITSCONTAINER_CNV_H

