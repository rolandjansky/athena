/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGEVENTATHENAPOOL_TRIGPASSBITSCONTAINER_CNV_H
#define TRIGEVENTATHENAPOOL_TRIGPASSBITSCONTAINER_CNV_H

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "TrigSteeringEvent/TrigPassBitsCollection.h"
#include "TrigSteeringEventTPCnv/TrigPassBitsCollection_p1.h"

#include <memory>

typedef TrigPassBitsCollection_p1   TrigPassBitsCollection_PERS;
typedef T_AthenaPoolCustomCnv<TrigPassBitsCollection, TrigPassBitsCollection_PERS> TrigPassBitsCollectionCnvBase;
struct TrigPassBitsCollection_impl;

class TrigPassBitsCollectionCnv : public TrigPassBitsCollectionCnvBase {

  friend class CnvFactory<TrigPassBitsCollectionCnv>;

public:
  TrigPassBitsCollectionCnv( ISvcLocator *svcloc );
  ~TrigPassBitsCollectionCnv();


protected:
  virtual TrigPassBitsCollection_PERS *createPersistent( TrigPassBitsCollection *transObj);
  virtual TrigPassBitsCollection      *createTransient();

private:
  std::unique_ptr<TrigPassBitsCollection_impl> m_impl;
};

#endif //TRIGPASSBITSCONTAINER_CNV_H

