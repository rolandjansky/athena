/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef VXCANDIDATE_CNV_P2_H
#define VXCANDIDATE_CNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "VxVertex/VxCandidate.h"
#include "TrkEventTPCnv/VxVertex/VxCandidate_p1.h"
#include "TrkEventTPCnv/VxVertex/RecVertexCnv_p2.h"
#include "TrkEventTPCnv/VxVertex/VxTrackAtVertexCnv_p1.h"

 class MsgStream;
 
class VxCandidateCnv_p2 : public T_AthenaPoolTPCnvBase< Trk::VxCandidate, Trk::VxCandidate_p1> {                            
  public:
   VxCandidateCnv_p2():m_recVertexConverter(0), m_vxTrackAtVertexConverter(0){}
  
    void  persToTrans(const Trk::VxCandidate_p1 * persObj, Trk::VxCandidate * transObj, MsgStream &log);
    void  transToPers(const Trk::VxCandidate * transObj, Trk::VxCandidate_p1 * persObj, MsgStream &log); 
    
    virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv ) {
       m_vxTrkAtVrtCnv.setTopConverter( topCnv, 0 );
    }
    
  private:
   
   RecVertexCnv_p2 * m_recVertexConverter;
   // VxTrackAtVertexCnv_p1 * m_vxTrackAtVertexConverter; // don't see it used anywhere ?!
   ITPConverterFor<Trk::VxTrackAtVertex> * m_vxTrackAtVertexConverter; // used now for looping by ourselves
   typedef T_AthenaPoolTPPolyVectorCnv< std::vector<Trk::VxTrackAtVertex*>, std::vector<TPObjRef>, ITPConverterFor<Trk::VxTrackAtVertex> >   VxTrkAtVrtVectorCnv_p1;
   VxTrkAtVrtVectorCnv_p1 m_vxTrkAtVrtCnv;

 };
#endif 
