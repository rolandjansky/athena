/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRT_HITCOLLECTIONCNV
#define TRT_HITCOLLECTIONCNV

#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollection_p1.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p1.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollection_p2.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p2.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollection_p3.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p3.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollection_p4.h"
#include "InDetSimEventTPCnv/InDetHits/TRT_HitCollectionCnv_p4.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
// Gaudi
#include "GaudiKernel/MsgStream.h"

// typedef to the latest persistent version
//typedef TRT_HitCollection_p1     TRT_HitCollection_PERS;
//typedef TRT_HitCollectionCnv_p1  TRT_HitCollectionCnv_PERS;
//typedef TRT_HitCollection_p2     TRT_HitCollection_PERS;
//typedef TRT_HitCollectionCnv_p2  TRT_HitCollectionCnv_PERS;
//typedef TRT_HitCollection_p3     TRT_HitCollection_PERS;
//typedef TRT_HitCollectionCnv_p3  TRT_HitCollectionCnv_PERS;
typedef TRT_HitCollection_p4     TRT_HitCollection_PERS;
typedef TRT_HitCollectionCnv_p4  TRT_HitCollectionCnv_PERS;

class TRTUncompressedHitCollectionCnv  : public T_AthenaPoolCustomCnv<TRTUncompressedHitCollection, TRT_HitCollection_PERS > {
  friend class CnvFactory<TRTUncompressedHitCollectionCnv>;
public:
  TRTUncompressedHitCollectionCnv(ISvcLocator* svcloc) :
        T_AthenaPoolCustomCnv<TRTUncompressedHitCollection, TRT_HitCollection_PERS >( svcloc) {}
protected:
  TRT_HitCollection_PERS*  createPersistent(TRTUncompressedHitCollection* transCont);
  TRTUncompressedHitCollection*       createTransient ();
};

#endif
