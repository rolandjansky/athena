/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSCOLLECTIONCNV_P1_H
#define HIPIXELTRACKLETSCOLLECTIONCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "HIRecTPCnv/HIPixelTrackletsCollection_p1.h"

#include "HIRecTPCnv/HIPixelTrackletsCnv_p1.h"
#include "HIGlobal/HIPixelTrackletsCollection.h"

class MsgStream;

class HIPixelTrackletsCollectionCnv_p1: public T_AthenaPoolTPPolyVectorCnv<HIPixelTrackletsCollection,HIPixelTrackletsCollection_p1,ITPConverterFor<HIPixelTracklets> > {
 public: 
  HIPixelTrackletsCollectionCnv_p1() {};
  //virtual void persToTrans(const HIPixelTrackletsCollection_p1 *persObj, HIPixelTrackletsCollection *transObj, MsgStream& log);
  //virtual void transToPers(const HIPixelTrackletsCollection *transObj, HIPixelTrackletsCollection_p1 *persObj, MsgStream& log);
};


/* template<> */
/* class T_TPCnv<HIPixelTrackletsCollection, HIPixelTrackletsCollection_p1>  */
/*   : public HIPixelTrackletsCollectionCnv_p1 { */
/*  public: */
/*   }; */

#endif
