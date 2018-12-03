/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef SIHITCOLLECTIONCNV
#define SIHITCOLLECTIONCNV

#include "InDetSimEvent/SiHitCollection.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollection_p1.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollectionCnv_p1.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollection_p2.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollectionCnv_p2.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollection_p3.h"
#include "InDetSimEventTPCnv/InDetHits/SiHitCollectionCnv_p3.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"
// typedef to the latest persistent version
//typedef SiHitCollection_p1     SiHitCollection_PERS;
//typedef SiHitCollectionCnv_p1  SiHitCollectionCnv_PERS;
typedef SiHitCollection_p2     SiHitCollection_PERS;
typedef SiHitCollectionCnv_p2  SiHitCollectionCnv_PERS;

class SiHitCollectionCnv  : public T_AthenaPoolCustomCnv<SiHitCollection, SiHitCollection_PERS > {
  friend class CnvFactory<SiHitCollectionCnv>;
protected:
  SiHitCollectionCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<SiHitCollection, SiHitCollection_PERS >( svcloc) {}
  SiHitCollection_PERS*  createPersistent(SiHitCollection* transCont);
  SiHitCollection*       createTransient ();
};


#endif
