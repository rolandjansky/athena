/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigInDetEventTPCnv
 * @Class  : TrigVertexCnv_p1
 *
 * @brief transient-persistent converter for TrigVertex
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigVertexCnv_p1.h,v 1.2 2009-04-01 22:08:45 salvator Exp $
 **********************************************************************************/
#ifndef TRIGINDETEVENTTPCNV_TRIGVERTEXCNV_P1_H
#define TRIGINDETEVENTTPCNV_TRIGVERTEXCNV_P1_H

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"

#include "AthenaPoolUtilities/TPObjRef.h"

#include "TrigInDetEvent/TrigVertex.h"
#include "TrigInDetEventTPCnv/TrigVertex_p1.h"
#include "TrigInDetEventTPCnv/TrigInDetTrackCnv_p1.h"

#include<vector>

class MsgStream;


class TrigVertexCnv_p1 : public T_AthenaPoolTPCnvBase<TrigVertex, TrigVertex_p1> 
{

 public:

  typedef std::vector< const TrigInDetTrack* >   TrackInVertexVector;
    
  TrigVertexCnv_p1() : m_TrigIDtrackCnv(0) {}
                       
  virtual void persToTrans(const TrigVertex_p1* persObj, TrigVertex* transObj, MsgStream &log);
  virtual void transToPers(const TrigVertex* transObj, TrigVertex_p1* persObj, MsgStream &log);
  
  virtual void initPrivateConverters( AthenaPoolTopLevelTPCnvBase *topCnv )
    {
      m_TrigInDetTrackVectorCnv.setTopConverter( topCnv, 0 );
    }
  
  
  //protected:
  TrigInDetTrackCnv_p1    *m_TrigIDtrackCnv   ;
  
  typedef T_AthenaPoolTPPtrVectorCnv< TrackInVertexVector,
                                   std::vector<TPObjRef>,
                                   TrigInDetTrackCnv_p1  >   TrigInDetTrackVectorCnv_p1;
  
  TrigInDetTrackVectorCnv_p1  m_TrigInDetTrackVectorCnv;
  TrackInVertexVector         m_trackInVertexVector    ;  //transient vector of TrigInDetTrack*
};


#endif
