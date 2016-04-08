/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSCOLLECTIONCNVTLP1_H
#define HIPIXELTRACKLETSCOLLECTIONCNVTLP1_H

#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"
#include "HIRecTPCnv/HIPixelTrackletsCollection_tlp1.h"
#include "HIRecTPCnv/HIPixelTrackletsCollectionCnv_p1.h"
#include "HIRecTPCnv/HIPixelTrackletsCnv_p1.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3DCnv_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p1.h"

class HIPixelTrackletsCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter<HIPixelTrackletsCollectionCnv_p1, HIPixelTrackletsCollection_tlp1> {
 public:
  HIPixelTrackletsCollectionCnv_tlp1();
  virtual ~HIPixelTrackletsCollectionCnv_tlp1() {}

  //default methods
  virtual void setPStorage(HIPixelTrackletsCollection_tlp1* storage);
  
  virtual TPCnvTokenList_p1* getTokenListVar() { return &(getTLPersObject()->m_tokenList); };
  
 protected:
  HIPixelTrackletsCnv_p1 m_hiPixelTrackletsCnv;
  HepPoint3DCnv_p1 m_hepPoint3DCnv;
  PixelClusterCnv_p1 m_pixelClusterCnv;
  
};

template <>
class T_TPCnv<HIPixelTrackletsCollection, HIPixelTrackletsCollection_tlp1>
  : public HIPixelTrackletsCollectionCnv_tlp1
{
 public:
  T_TPCnv();
  void persToTrans(const HIPixelTrackletsCollection_tlp1* pers,
			   HIPixelTrackletsCollection* trans,
			   MsgStream& msg);
};

#endif
