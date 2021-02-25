/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LUCID_SimHitCollectionCnv_H
#define LUCID_SimHitCollectionCnv_H

#include "LUCID_SimEvent/LUCID_SimHitCollection.h"
#include "LUCID_EventTPCnv/LUCID_SimHitCollection_p1.h"
#include "LUCID_EventTPCnv/LUCID_SimHitCollectionCnv_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent vesion
typedef LUCID_SimHitCollection_p1 LUCID_SimHitCollection_PERS;
typedef LUCID_SimHitCollectionCnv_p1 LUCID_SimHitCollectionCnv_PERS;

typedef T_AthenaPoolCustomCnv < LUCID_SimHitCollection, LUCID_SimHitCollection_PERS > LUCID_SimHitCollectionCnvBase;

class LUCID_SimHitCollectionCnv: public LUCID_SimHitCollectionCnvBase {

  friend class CnvFactory<LUCID_SimHitCollectionCnv>;
public:
  LUCID_SimHitCollectionCnv(ISvcLocator* svcloc): LUCID_SimHitCollectionCnvBase(svcloc) {}
protected:

  virtual LUCID_SimHitCollection_PERS* createPersistent(LUCID_SimHitCollection* transCont);
  virtual LUCID_SimHitCollection*      createTransient ();
};

#endif
