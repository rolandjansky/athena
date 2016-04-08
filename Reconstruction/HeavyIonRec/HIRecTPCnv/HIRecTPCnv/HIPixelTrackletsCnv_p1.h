/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HIPIXELTRACKLETSCNV_P1_H
#define HIPIXELTRACKLETSCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "HIGlobal/HIPixelTracklets.h"
#include "HIRecTPCnv/HIPixelTracklets_p1.h"
#include "InDetEventTPCnv/InDetPrepRawData/PixelClusterCnv_p1.h"
#include "InDetPrepRawData/PixelCluster.h"
#include "TrkEventTPCnv/TrkEventPrimitives/HepPoint3DCnv_p1.h"

class MsgStream;

class HIPixelTrackletsCnv_p1 : public T_AthenaPoolTPCnvBase<HIPixelTracklets,HIPixelTracklets_p1> {
  typedef InDet::PixelCluster PixelCluster;
 public:
  HIPixelTrackletsCnv_p1():m_hepPoint3DCnv(0) {};
  
  void persToTrans(const HIPixelTracklets_p1 *persObj, HIPixelTracklets *transObj, MsgStream &log);
  void transToPers(const HIPixelTracklets *transObj, HIPixelTracklets_p1 *persObj, MsgStream &log);
  virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv) {
    m_clusterVectorCnv.setTopConverter(topCnv,0);
  }
  
 private:
  HepPoint3DCnv_p1* m_hepPoint3DCnv;
  PixelClusterCnv_p1* m_pixelClusterCnv;
  typedef T_AthenaPoolTPPolyVectorCnv< std::vector<PixelCluster*>, std::vector<TPObjRef>, ITPConverterFor<PixelCluster> > clusterForTrkletsVectorCnv_p1; 
  //typedef T_AthenaPoolTPPtrVectorCnv< std::vector<PixelCluster*>, std::vector<TPObjRef>, PixelClusterCnv_p1> clusterForTrkletsVectorCnv_p1;
  clusterForTrkletsVectorCnv_p1 m_clusterVectorCnv;
    
};

#endif
