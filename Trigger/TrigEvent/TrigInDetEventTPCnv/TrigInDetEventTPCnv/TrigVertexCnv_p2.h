/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCnv_p2
 *
 * @brief transient-persistent converter for TrigVertex
 *
 * @author Dmitry Emeliyanov
 *
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCNV_P2_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCNV_P2_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "AthenaPoolUtilities/TPObjRef.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEventTPCnv/TrigVertex_p2.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p3.h"

#include<vector>

class MsgStream;


class TrigVertexCnv_p2 : public T_AthenaPoolTPCnvBase<TrigVertex, TrigVertex_p2> 
{

 public:

  typedef std::vector< const TrigInDetTrack* >   TrackInVertexVector;
    
  TrigVertexCnv_p2() : m_TrigIDtrackCnv(0) {}
                       
  virtual void persToTrans(const TrigVertex_p2* persObj, TrigVertex* transObj, MsgStream &log);
  virtual void transToPers(const TrigVertex* transObj, TrigVertex_p2* persObj, MsgStream &log);
  
  virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv )
    {
      m_TrigInDetTrackVectorCnv.setTopConverter( topCnv, 0 );
    }
  
  
  //protected:
  TrigInDetTrackCnv_p3    *m_TrigIDtrackCnv   ;
  
  typedef T_AthenaPoolTPPtrVectorCnv< TrackInVertexVector,
                                   std::vector<TPObjRef>,
                                   TrigInDetTrackCnv_p3  >   TrigInDetTrackVectorCnv_p3;
  
  TrigInDetTrackVectorCnv_p3  m_TrigInDetTrackVectorCnv;
  TrackInVertexVector         m_trackInVertexVector    ;  //transient vector of TrigInDetTrack*
};


#endif
