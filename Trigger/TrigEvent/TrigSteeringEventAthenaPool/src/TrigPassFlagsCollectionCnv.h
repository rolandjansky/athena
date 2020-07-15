/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGPASSFLAGSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGPASSFLAGSCONTAINER_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigSteeringEvent/TrigPassFlagsCollection.h"
#include "TrigSteeringEventTPCnv/TrigPassFlagsCollection_p1.h"

#include <memory>

typedef TrigPassFlagsCollection_p1   TrigPassFlagsCollection_PERS;
typedef T_AthenaPoolCustomCnv<TrigPassFlagsCollection, TrigPassFlagsCollection_PERS> TrigPassFlagsCollectionCnvBase;
struct TrigPassFlagsCollectionCnv_impl;

class TrigPassFlagsCollectionCnv : public TrigPassFlagsCollectionCnvBase {

  friend class CnvFactory<TrigPassFlagsCollectionCnv>;

public:
  TrigPassFlagsCollectionCnv( ISvcLocator *svcloc );
  ~TrigPassFlagsCollectionCnv();
protected:

  virtual TrigPassFlagsCollection_PERS *createPersistent( TrigPassFlagsCollection *transObj);
  virtual TrigPassFlagsCollection      *createTransient();

private:
  std::unique_ptr<TrigPassFlagsCollectionCnv_impl> m_impl;
};

#endif //TRIGPASSBITSCONTAINER_CNV_H

