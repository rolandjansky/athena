/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTHITCOLLECTIONCNV_P3_H
#define TRTHITCOLLECTIONCNV_P3_H

// SiHitCollectionCnv_p2, T/P separation of Si Hits
// author Robert Duxfield <r.duxfield@sheffield.ac.uk>

#include "InDetSimEvent/TRTUncompressedHitCollection.h"
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "TRT_HitCollection_p3.h"


class TRT_HitCollectionCnv_p3 : public T_AthenaPoolTPCnvBase<TRTUncompressedHitCollection, TRT_HitCollection_p3>
{
 public:

  TRT_HitCollectionCnv_p3() {};

  virtual TRTUncompressedHitCollection* createTransient(const TRT_HitCollection_p3* persObj, MsgStream &log);

  virtual void persToTrans(const TRT_HitCollection_p3* persCont,
			   TRTUncompressedHitCollection* transCont,
			   MsgStream &log) ;
  virtual void transToPers(const TRTUncompressedHitCollection* transCont,
			   TRT_HitCollection_p3* persCont,
			   MsgStream &log) ;
};

#endif
